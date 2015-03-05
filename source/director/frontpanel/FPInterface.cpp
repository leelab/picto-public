#include <QTextStream>
#include <QRegExp>

#include "../../common/globals.h"
#include "FPInterface.h"

/*! \brief Constructs a FPInterface object.
 *	\details Creates QTcpSocket interfaces and sets up Picto::ProtocolCommandHander object to handle the command interface.
 */
FPInterface::FPInterface()
{
	//Set up network connections
	commSocket = new QTcpSocket();
	commSocket->connectToHost(QHostAddress::LocalHost, quint16(Picto::portNums->getLCDCommandPort()), QIODevice::ReadWrite);
	while(!commSocket->waitForConnected(20000))
		commSocket->connectToHost(QHostAddress::LocalHost, quint16(Picto::portNums->getLCDCommandPort()), QIODevice::ReadWrite);


	eventSocket = new QTcpSocket();
	eventSocket->connectToHost(QHostAddress::LocalHost, quint16(Picto::portNums->getLCDEventPort()), QIODevice::ReadWrite);
	while(!eventSocket->waitForConnected(20000))
		eventSocket->connectToHost(QHostAddress::LocalHost, quint16(Picto::portNums->getLCDEventPort()), QIODevice::ReadWrite);

	//set up the command handlers
	fpgetCommandHandler_ = QSharedPointer<FPGETCommandHandler>(new FPGETCommandHandler);
	commandHandlers[fpgetCommandHandler_->method()] = fpgetCommandHandler_;

	fpputCommandHandler_ = QSharedPointer<FPPUTCommandHandler>(new FPPUTCommandHandler);
	connect(fpputCommandHandler_.data(),SIGNAL(setName(QString)),this,SLOT(setName(QString)));
	connect(fpputCommandHandler_.data(),SIGNAL(setRewardDuration(int,int)),this,SLOT(setRewardDuration(int,int)));
	connect(fpputCommandHandler_.data(),SIGNAL(setFlushDuration(int,int)),this,SLOT(setFlushDuration(int,int)));
	commandHandlers[fpputCommandHandler_->method()] = fpputCommandHandler_;

	fprewardCommandHandler_ = QSharedPointer<FPREWARDCommandHandler>(new FPREWARDCommandHandler);
	connect(fprewardCommandHandler_.data(),SIGNAL(giveReward(int)),this,SLOT(giveReward(int)));
	commandHandlers[fprewardCommandHandler_->method()] = fprewardCommandHandler_;

	fpstartflushCommandHandler_ = QSharedPointer<FPFLUSHCommandHandler>(new FPFLUSHCommandHandler);
	connect(fpstartflushCommandHandler_.data(),SIGNAL(flush(int)),this,SLOT(flush(int)));
	commandHandlers[fpstartflushCommandHandler_->method()] = fpstartflushCommandHandler_;
}

FPInterface::~FPInterface()
{
}

/*! \brief Causes the commands coming in from the front panel to be read, handled, and responded to.
 *	\details This function Should be called a lot, like every frame to avoid lag in the control panel.
 */
void FPInterface::doIncomingCommands()
{

	//For some reason, when we're running an experiment only waitForReadyRead()
	//works, and when we're idle only canReadLine() works.  This is worth some
	//investivation at some point.
	if(!commSocket->waitForReadyRead(0) && !commSocket->canReadLine())
		return;

	QSharedPointer<Picto::ProtocolCommand> command;
	command = readCommand();

	QSharedPointer<Picto::ProtocolResponse> response;
	response = processCommand(command);

	//send out response
	deliverResponse(response);
}

/*! \brief Called when someone on the Director side changed the Director name.  Stores the value so that 
 *	it will be available to the Front Panel when requested.
 */
void FPInterface::nameWasChanged()
{
	fpgetCommandHandler_->setName(getName());
}

/*! \brief Called when someone on the Director side changed the IP address.  Stores the value so that 
 *	it will be available to the Front Panel when requested.
 */
void FPInterface::ipWasChanged()
{
	fpgetCommandHandler_->setIpAddress(getIP());
}

/*! \brief Called when someone on the Director side changed the default reward duration.  Stores the value so that 
 *	it will be available to the Front Panel when requested.
 */
void FPInterface::rewardDurationWasChanged(int controller)
{
	fpgetCommandHandler_->setRewardDuration(controller,getRewardDuration(controller));
}

/*! \brief Called when someone on the Director side changed the default flush duration.  Stores the value so that 
 *	it will be available to the Front Panel when requested.
 */
void FPInterface::flushDurationWasChanged(int controller)
{
	fpgetCommandHandler_->setFlushDuration(controller,getFlushDuration(controller));
}

/*! \brief Called when the Director status changes.  Stores the value so that 
 *	it will be available to the Front Panel when requested.
 */
void FPInterface::statusWasChanged(QString status)
{
	fpgetCommandHandler_->setStatus(getStatus());
}

/*! \brief Reads in a command from the Command socket and returns it.
*/
QSharedPointer<Picto::ProtocolCommand> FPInterface::readCommand()
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

/*! \brief Accepts a Picto::ProtocolCommand, checks its syntax and if acceptable sends it into a Picto::ProtocolCommandHandler
 *	for processing, then returns the Picto::ProtocolResponse generated by the command handler.
 */
QSharedPointer<Picto::ProtocolResponse> FPInterface::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
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

/*! \brief Sends the input Picto::ProtocolResponse to the Front Panel using the Command Socket.
 *	\details Given the simplicity of the commands going back and forth between 
 *	the front panel and the FPInterface, this code assumes that the response
 *	consists of simple headers and possibly content.  We do not handle
 *	streaming response, multi-part responses, etc...
 */
void FPInterface::deliverResponse(QSharedPointer<Picto::ProtocolResponse> response)
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

	return;
}

/*! \brief Sends an ENGEVENT command through the event channel.
 *	\details The command's content will be the passed in XML fragment.
 */
void FPInterface::sendFPInterfaceEvent(QString xmlFragment)
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




