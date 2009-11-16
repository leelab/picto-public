#include <QRegExp>

#include <QTextStream>

#include "eventchannelthread.h"
#include "../../common/protocol/protocolcommandhandler.h"


EventChannelThread::EventChannelThread(FrontPanelInfo *panelInfo, QObject *parent)
	: QThread(parent),
	  panelInfo(panelInfo),
	  timeoutInterval(500)
{
}

void EventChannelThread::run()
{
	//the socket is alread connected, so we just need to look for ready reads
	QTcpSocket *socket = panelInfo->getEventSocket();
	connect(socket, SIGNAL(readyRead()), this, SLOT(readCommand()));

	//create an instance of the ENGEVEN command handler
	engeventCommandHandler = new ENGEVENTCommandHandler();
	engeventCommandHandler->setPanelInfo(panelInfo);

	QThread::exec();

	//we should really never get to here, since the front panel is started
	//on boot and never actually quits, so this thread will be started upon 
	//connecting to the engine, and only quit when PictoBox loses power.
	delete engeventCommandHandler;

	socket->disconnectFromHost();
	if(socket->state() != QTcpSocket::UnconnectedState)
	{
		socket->waitForDisconnected();
	}
}

void EventChannelThread::readCommand()
{

	QTcpSocket *socket = panelInfo->getEventSocket();

	while(socket->canReadLine())
	{
		QString pendingCommand("");
		QString currentLine("");
		QRegExp newLineRegExp("[\r\n]+");

		//read in the command and headers
		while(socket->canReadLine() && !newLineRegExp.exactMatch(currentLine))
		{
			currentLine = socket->readLine();
			pendingCommand += currentLine;
		}

		//create the command
		QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(pendingCommand));
		
		//read in the content
		int remainingContentLength = command->getFieldValue("Content-Length").toInt();

		while(command->isPendingContent())
		{
			if(socket->bytesAvailable() == 0 && !socket->waitForReadyRead(timeoutInterval))
			{
				//We'll just ignore incoming commands if the engine can't be bothered
				//to send us the full command in a timely fashion.  (That'll teach it!)
				return;
			}
			else
			{
				remainingContentLength = command->addToContent(socket->read(remainingContentLength));

			}
		}

		//process the command
		QSharedPointer<Picto::ProtocolResponse> response;
		processCommand(command);

		emit newEventRead();
	}
}

//Since the envent channel is truly a one-way channel, we don't need to create
//responses.  The process command function basically runs the command handler
//for ENGEVENT commands and ignores all other commands.  

//Note that command handlers have a response return type, but we are simply ignoring
//the returned values.  As such, command handlers used exclusivey in this function 
//should probably return a null response, since the responses will never get delivered.
void EventChannelThread::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	if(command->getMethod() == "ENGEVENT")
	{
		engeventCommandHandler->processCommand(command);
	}
	else
	{
	}
}

