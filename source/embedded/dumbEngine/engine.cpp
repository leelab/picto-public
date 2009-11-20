#include <QTextStream>
#include <QRegExp>

#include "../../common/globals.h"
#include "engine.h"
#include "protocol/FPGETCommandHandler.h"
#include "protocol/FPPUTCommandHandler.h"
#include "protocol/FPREWARDCommandHandler.h"
#include "protocol/FPSTARTFLUSHCommandHandler.h"
#include "protocol/FPSTOPFLUSHCommandHandler.h"

Engine::Engine(QString boxName, QHostAddress addr, QObject *parent) :
	QObject(parent),
	boxName(boxName),
	boxAddress(addr),
	currTrial(1),
	currBlock(1)
{
	QTextStream out(stdout);

	rewardDurations[0] = 100;
	rewardDurations[1] = 200;
	rewardDurations[2] = 300;
	rewardDurations[3] = 400;

	flushDurations[0] = 10;
	flushDurations[1] = 15;
	flushDurations[2] = 20;
	flushDurations[3] = 25;

	flushTimeRemain[0] = -1;
	flushTimeRemain[1] = -1;
	flushTimeRemain[2] = -1;
	flushTimeRemain[3] = -1;


	//Set up network connections
	commSocket = new QTcpSocket();
	commSocket->connectToHost(QHostAddress::LocalHost, 42422, QIODevice::ReadWrite);
	if(!commSocket->waitForConnected(20000))
		out<<"Command socket failed to connect\n";
	else
		out<<"Command socket connected\n";


	eventSocket = new QTcpSocket();
	eventSocket->connectToHost(QHostAddress::LocalHost, 42421, QIODevice::ReadWrite);
	if(!eventSocket->waitForConnected(20000))
		out<<"Event socket failed to connect\n";
	else
		out<<"Event socket connected\n";

	//set up the command handlers
	QSharedPointer<FPGETCommandHandler> fpgetCommandHandler(new FPGETCommandHandler);
	fpgetCommandHandler->setEngine(this);
	commandHandlers[fpgetCommandHandler->method()] = fpgetCommandHandler;

	QSharedPointer<FPPUTCommandHandler> fpputCommandHandler(new FPPUTCommandHandler);
	fpputCommandHandler->setEngine(this);
	commandHandlers[fpputCommandHandler->method()] = fpputCommandHandler;

	QSharedPointer<FPREWARDCommandHandler> fprewardCommandHandler(new FPREWARDCommandHandler);
	fprewardCommandHandler->setEngine(this);
	commandHandlers[fprewardCommandHandler->method()] = fprewardCommandHandler;

	QSharedPointer<FPSTARTFLUSHCommandHandler> fpstartflushCommandHandler(new FPSTARTFLUSHCommandHandler);
	fpstartflushCommandHandler->setEngine(this);
	commandHandlers[fpstartflushCommandHandler->method()] = fpstartflushCommandHandler;

	QSharedPointer<FPSTOPFLUSHCommandHandler> fpstopflushCommandHandler(new FPSTOPFLUSHCommandHandler);
	fpstopflushCommandHandler->setEngine(this);
	commandHandlers[fpstopflushCommandHandler->method()] = fpstopflushCommandHandler;

	//set up the daqboard
	daqBoard = new Picto::PictoBoxDaqBoard();

	unsigned int rewardDurations[] = {100,100,100,100};
	if(!daqBoard->initRewardController(4,rewardDurations))
		out<<"Failed to initialize reward controller";
	if(!daqBoard->initEventLines())
		out<<"Failed to initialize event lines";

}

Engine::~Engine()
{
	delete daqBoard;
}

void Engine::runEngine(int trialsPerBlock, int blocks)
{
	if(!commSocket->isValid())
		return;
	if(!eventSocket->isValid())
		return;

	this->trialsPerBlock = trialsPerBlock;
	blocksToRun = blocks;

	trialTimer = new QTimer(this);
	trialTimer->setInterval(2000);
	connect(trialTimer,SIGNAL(timeout()), this, SLOT(endTrial()));

	interTrialTimer = new QTimer(this);
	interTrialTimer->setInterval(500);
	connect(interTrialTimer,SIGNAL(timeout()), this, SLOT(startTrial()));
	interTrialTimer->start();
	
	frameTimer = new QTimer(this);
	frameTimer->setInterval(17);
	connect(frameTimer,SIGNAL(timeout()), this, SLOT(doFrame()));

	flushingTimer = new QTimer(this);
	flushingTimer->setInterval(1000);
	connect(flushingTimer, SIGNAL(timeout()), this, SLOT(decrementFlushTime()));

	//reset counter
	eventCodeCounter = 0;

	//Tell the front panel that we are starting
	QString eventXml = QString("<event type=\"statuschange\" status=\"running\" />");
	sendEngineEvent(eventXml);

	eventXml = QString("<event type=\"experimentstart\" />");
	sendEngineEvent(eventXml);
}

//called at the beginning of a trial
void Engine::startTrial()
{
	//deal with event codes
	/*! \todo Sending the even needs to occur at EXACTLY the same time
	          as we mark the trial start/end in the behavioral database. */
	eventCodeCounter++;
	if(eventCodeCounter >127)
		eventCodeCounter = 0;
	daqBoard->sendEvent(eventCodeCounter);

	interTrialTimer->stop();
	trialTimer->start();
	frameTimer->start();
	currTrial++;

	//end the experiment?
	if(currBlock+1>blocksToRun && currTrial>trialsPerBlock)
	{
		QString eventXml = QString("<event type=\"experimentend\" />");
		sendEngineEvent(eventXml);

		eventXml = QString("<event type=\"statuschange\" status=\"connected\" />");
		sendEngineEvent(eventXml);

		trialTimer->stop();
		frameTimer->stop();
		return;
	}

	//change the block?
	if(currTrial > trialsPerBlock)
	{
		currTrial = 1;
		currBlock++;
		QString eventXml = QString("<event type=\"blockend\" block=\"%1\"/>").arg(currBlock);
		sendEngineEvent(eventXml);
		
		eventXml = QString("<event type=\"blockstart\" block=\"%1\"/>").arg(currBlock);
		sendEngineEvent(eventXml);

	}
	QString eventXml = QString("<event type=\"trialstart\" trial=\"%1\"/>").arg(currTrial);
	sendEngineEvent(eventXml);
}

//called at the end of a trial
void Engine::endTrial()
{
	QTextStream out(stdout);
	out<<"End trial\n";

	trialTimer->stop();
	frameTimer->stop();
	interTrialTimer->start();

	QString eventXml = QString("<event type=\"trialend\" trial=\"%1\"/>").arg(currTrial);
	sendEngineEvent(eventXml);
}

//called every frame
void Engine::doFrame()
{
	QTextStream out(stdout);

	frameTimer->stop();

	//Generate a rnadom event once every so often (once every 4 seconds)
	if(qrand() % 240 == 0)
	{
		QString eventXml = QString("<event type=\"other\">Random event</event>").arg(currTrial);
		sendEngineEvent(eventXml);
	}

	//if there's data, we can safely assume that it's a command, so
	//we'll try to parse it...
	//Note this is arranged in a way that limits us to one command per frame.
	//This was done on purpose so that the engine won't get bogged down
	//with commands.  The front panel isn't supposed to send more than
	//one command at a time anyway...
	if(incomingCommand())
	{
		QSharedPointer<Picto::ProtocolCommand> command;
		command = readCommand();


		//Handle command 
		/*out<<"Incoming Command:\n";
		QString method = command->getMethod();
		QString target = command->getTarget();
		QString protocolName = command->getProtocolName();
		QString protocolVer = command->getProtocolVersion();
		out<<" "<<method<<" "<<target<<" "<<protocolName<<"/"<<protocolVer<<"\n";

		QByteArray content = command->getContent();
		out<<" content:\n "<<content<<"\n";
		out.flush();*/

		QSharedPointer<Picto::ProtocolResponse> response;
		response = processCommand(command);

		//send out response
		deliverResponse(response);
	}
	frameTimer->start();

}

bool Engine::incomingCommand()
{
	if(commSocket->canReadLine())
		return true;
	else
		return false;
}



/*! \TODO: We need to figure out a way to make sure that we either
 *  read a whole command, or no command.  Since this occurs during a 
 *  frame, we can't spend too much time waiting for complete commands 
 *  to come in
 */
QSharedPointer<Picto::ProtocolCommand> Engine::readCommand()
{
	//grab the command
	QString pendingCommand("");
	QString currentLine("");
	QRegExp newLineRegExp("[\r\n]+");
	
	//read in the command and headers
	while(commSocket->canReadLine() && !newLineRegExp.exactMatch(currentLine))
	{
		currentLine = commSocket->readLine();
		pendingCommand += currentLine;
	}

	//create the command
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(pendingCommand));

	//read in the content.  Since this has to be done without dropping frames, 
	//and we know that incoming commands will have extremely small amounts of
	//content (and definitely won't be streaming), we're going to simply assume 
	//that the content is ready and waiting, and if it isn't, the command will
	//be sent on with insufficient content
	int remainingContentLength = command->getFieldValue("Content-Length").toInt();

	if(commSocket->bytesAvailable() >= remainingContentLength)
		command->addToContent(commSocket->read(remainingContentLength));

	return command;

}

QSharedPointer<Picto::ProtocolResponse> Engine::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QString protocolName = command->getProtocolName();
	QString protocolVersion = command->getProtocolVersion();

	//we only deal with Picto/1.0 commands
	if(protocolName != "PICTO"  && protocolVersion != "1.0")
	{
		QSharedPointer<Picto::ProtocolResponse> unknownProtocolResponse(
			new Picto::ProtocolResponse(Picto::Names->directorAppName,
										"PICTO",
										"1.0",
										Picto::ProtocolResponseType::BadRequest));

		return unknownProtocolResponse;

	}
	//The command hopefully got all of its content, if not, we can't reply..
	else if(command->isPendingContent())
	{
		QSharedPointer<Picto::ProtocolResponse> incompleteRequestResponse(
			new Picto::ProtocolResponse(Picto::Names->directorAppName,
										"PICTO",
										"1.0",
										Picto::ProtocolResponseType::IncompleteRequest));

		return incompleteRequestResponse;
	}
	//the command is good!
	else
	{
		QMap<QString, QSharedPointer<Picto::ProtocolCommandHandler>>::iterator handler;
		handler = commandHandlers.find(command->getMethod());

		if(handler == commandHandlers.end())
		{
			QSharedPointer<Picto::ProtocolResponse> notFoundResponse(
				new Picto::ProtocolResponse(Picto::Names->directorAppName,
											"PICTO",
											"1.0",
											Picto::ProtocolResponseType::NotFound));
			return notFoundResponse;
		}
		else
		{
			QSharedPointer<Picto::ProtocolResponse> response;
			response = handler.value()->processCommand(command);

			return response;
		}

	}
}

//Given the simplicity of the commands going back and forth between 
//the front panel and the engine, this code will assume that the response
//consists of simple headers and possibly content.  We do not handle
//streaming response, multi-part responses, etc...
void Engine::deliverResponse(QSharedPointer<Picto::ProtocolResponse> response)
{

	if(response.isNull())
		return;


	QString headers = response->generateHeadersString();
	headers += "\r\n";
	commSocket->write(headers.toUtf8().data(), headers.toUtf8().size());

	QByteArray content;
	if(response->hasContent())
	{
		content = response->getContent();
		commSocket->write(content.data(),content.size());
	}

	/*QTextStream out(stdout);
	out<<"Response delivered:\n";
	out<<headers<<content<<"\n";*/

	return;
}

void Engine::startFlush(int controller)
{
	QTextStream out(stdout);
	out<<"Flushing on controller "<<controller<<"\n";
	if(daqBoard->flushReward(controller, true))
	{
		flushTimeRemain[controller-1] = flushDurations[controller-1];

		//if it's not already running, start the timer
		if(!flushingTimer->isActive())
			flushingTimer->start();
	}

}

void Engine::stopFlush(int controller)
{
	QTextStream out(stdout);
	out<<"Stopping flushing on controller "<<controller<<"\n";

	flushTimeRemain[controller-1] = -1;

	daqBoard->flushReward(controller, false);

	//if none of the controllers are currently being flushed, 
	//stop the clock.
	bool allInactive = true;
	for(int i=0; i<4; i++)
		if(flushTimeRemain[i] > 0)
			allInactive = false;

	if(allInactive)
		flushingTimer->stop();
}

void Engine::decrementFlushTime()
{
	//decrement the flush time for all of the controllers that are currently
	//being flushed, and stop any where time has expired.
	for(int i=1; i<=4; i++)
	{
		if(flushTimeRemain[i-1] > 0)
			flushTimeRemain[i-1]--;
		if(flushTimeRemain[i-1] == 0)
			stopFlush(i);
	}

}

void Engine::giveReward(int controller)
{
	daqBoard->giveReward(controller);
}
int Engine::getRewardDuration(int controller) 
{
	return (unsigned int)daqBoard->getRewardDuration(controller);

}

void Engine::setRewardDuration(int controller, int duration) 
{
	daqBoard->setRewardDuration(controller,duration);
}


//This function send an ENGEVENT command through the event channel.
//The command's content will be the passed in XML fragment
void Engine::sendEngineEvent(QString xmlFragment)
{
	//prepend a start document tag so the xml reader doesn't get annoyed
	xmlFragment.prepend("<?xml version=\"1.0\"?>\n");

	//create the command
	Picto::ProtocolCommand command("ENGEVENT /frontpanel PICTO/1.0");
	QByteArray content = xmlFragment.toUtf8();
	command.setContent(content);
	command.setFieldValue("Content-Length",QString("%1").arg(content.size()));

	//send it to the front panel
	command.write(eventSocket);

}




