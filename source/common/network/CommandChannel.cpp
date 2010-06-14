#include <QStringList>
#include <QTime>
#include <QCoreApplication>

#include "CommandChannel.h"

namespace Picto {

CommandChannel::CommandChannel(QObject *parent)
	:QObject(parent),
	status(disconnected),
	reconnect(true)
{
	//set up the sockets
	producerSocket = new QTcpSocket(this);
	consumerSocket = new QTcpSocket(this);
}

CommandChannel::CommandChannel(QHostAddress serverAddress, quint16 serverPort, QObject *parent)
	:QObject(parent),
	serverAddr(serverAddress),
	serverPort(serverPort),
	status(disconnected),
	reconnect(true)
{
	//set up the sockets
	producerSocket = new QTcpSocket(this);
	consumerSocket = new QTcpSocket(this);

	initConnection();
}

CommandChannel::~CommandChannel()
{
	if(status == connected)
		closeChannel();

	delete producerSocket;
	delete consumerSocket;
}

void CommandChannel::connectToServer(QHostAddress serverAddress, quint16 serverPort)
{
	if(status != disconnected)
		return;

	this->serverAddr = serverAddress;
	this->serverPort = serverPort;

	status = disconnected;

	initConnection();
}

void CommandChannel::initConnection()
{
	//set multipart boundary to a null string
	multipartBoundary = "";
	//even in polled mode, we're going to make a couple of connections, for events that
	//are unlikely to occur (and when they do occur, we're probably in trouble anyway,
	//so grabbing a few extra cycles will be the least of our worries)
	connect(consumerSocket, SIGNAL(disconnected()), this, SLOT(disconnectHandler()));
	connect(producerSocket, SIGNAL(disconnected()), this, SLOT(disconnectHandler()));

	//connect to the server
	producerSocket->connectToHost(serverAddr, serverPort, QIODevice::ReadWrite);
	consumerSocket->connectToHost(serverAddr, serverPort, QIODevice::ReadWrite);

	if(consumerSocket->waitForConnected(5000) && producerSocket->waitForConnected(5000))
	{
		status = connected;
	}
	else
	{
		status = disconnected;
	}
}


/*!	\brief Closes the channel (also called from the destructor)
 *
 */
void CommandChannel::closeChannel()
{
	reconnect = false;
	producerSocket->close();
	consumerSocket->close();
}


//! Checks the network for any new incoming responses, adds them to the queue, and returns
//! the number of responses in the queue.  Used when the channel is in polled mode
int CommandChannel::incomingResponsesWaiting()
{
	//before looking for incoming responses, we need to give the
	//socket a chance to update its buffers, waitForReadyRead
	//accomplishes this.
	consumerSocket->waitForReadyRead(0);
	readIncomingResponse();
	return incomingResponseQueue.size();
}

//! Checks the network for any new incoming commands, adds them to the queue, and returns
//! the number of responses in the queue. Used when the channel is in polled mode
int CommandChannel::incomingCommandsWaiting()
{
	readIncomingCommand();
	return incomingCommandQueue.size();
}

//! Returns the top response in the queue, or a null pointer if there are no
//! responses in the queue (this will always happen if you aren't in polled mode)
QSharedPointer<ProtocolResponse> CommandChannel::getResponse()
{
	if(incomingResponseQueue.empty())
		return QSharedPointer<ProtocolResponse>();
	else
		return incomingResponseQueue.takeFirst();
}

//! Returns the top command in the queue, or a null pointer if there are no
//! commands in the queue (this will always happen if you aren't in polled mode)
QSharedPointer<ProtocolCommand> CommandChannel::getCommand()
{
	if(incomingCommandQueue.empty())
		return QSharedPointer<ProtocolCommand>();
	else
		return incomingCommandQueue.takeFirst();
}

/*! \brief Waits for a response to arrive
 *
 *	Since the networking code works best with some sort of event loop, this
 *	function creates one for us.  Note that if the timeout timer is 0, we still
 *	call process events.
 *
 *	The function returns true if there is a waiting response.
 *
 *	This function is designed to work if the channel is in polling mode. If
 *	the channel isn't in polling mode, the function immediately returns false.
 */
bool CommandChannel::waitForResponse(int timeout)
{
	if(timeout == 0)
	{
		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		if(incomingResponsesWaiting() > 0)
			return true;
		else
			return false;
	}
	else
	{
		QTime timeoutTimer;
		timeoutTimer.start();

		while(true)
		{
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

			if(incomingResponsesWaiting()>0)
			{
				return true;
			}

			if(timeoutTimer.elapsed() > timeout)
			{
				return false;
			}
		}
	}

}


//! Reads an incoming response and either emits the incomingCommand signal or adds it to the queue
void CommandChannel::readIncomingResponse()
{
	int bytesRead;
	while(consumerSocket->bytesAvailable() > 0)
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse());

		//if we're expecting a multipart part response, we should go ahead and set the
		//boundary string for the response.
		if(!multipartBoundary.isEmpty())
			response->setMultiPartBoundary(multipartBoundary);

		bytesRead = response->read(consumerSocket);

		//if there is an error, bytes read will be negative
		if(bytesRead >= 0)
		{
			if(response->getMultiPart() == MultiPartResponseType::MultiPartInitial)
				multipartBoundary = response->getMultiPartBoundary();
			else if(!multipartBoundary.isEmpty())
				multipartBoundary="";

			//Check to see if this is a registered response
			QString commandId = response->getFieldValue("Command-ID");
			if(!commandId.isNull())
				pendingCommands_.remove(QUuid(commandId));

			incomingResponseQueue.push_back(response);
		}
		else
		{
			Q_ASSERT(false); //This is for debugging purposes only
		}
	}

}

//! Reads an incoming command and either emits the incomingCommand signal or adds it to the queue
void CommandChannel::readIncomingCommand()
{
	int bytesRead;
	while(producerSocket->bytesAvailable() > 0)
	{
		QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand());
		bytesRead = command->read(producerSocket);

		//if there is an error, bytes read will be negative
		if(bytesRead >= 0)
		{
			incomingCommandQueue.push_back(command);
		}
	}
}

/*! \brief Sends a command over the channel
 *
 *	sendCommand sends the passed in command over the commandChannel.  This can 
 *	be used a a stand-alone function that gets called on-demand, or as a slot
 *	that gets connected to a signal for  automatic command sending.
 *
 *	WARNING: This command permanently adds a Session
 */
bool CommandChannel::sendCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//We always add a session-ID field, even if it's a null value
	command->setFieldValue("Session-ID",sessionId_.toString());

	if(command->write(consumerSocket) < 1)
	{
		qDebug("CommandChannel::sendCommand failed to send requested command");
		return false;
	}
	return true;
}

/*	\brief Sends a command and then registers it so we can watch for the matching response
 *
 *	This function is used when you want to be able to check at a later time to determine
 *	if a response has been issued for your command.  A command sent this way 
 *	has a "Command-ID" field appended.  The command is also added to the pending
 *	commands map (using the Command-ID UUID as a key).  Sending a command
 *	"registered" allows us to see if a response was received for that command
 *	and to resend any commands which didn't receive responses.
 */
bool CommandChannel::sendRegisteredCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QUuid commandId = QUuid::createUuid();
	command->setFieldValue("Command-ID",commandId);
	pendingCommands_[commandId] = command;
	return sendCommand(command);
}

/*! \brief Sends a response over the channel
 *
 *	sendResponse sends the response in command over the commandChannel.  This can 
 *	be used a a stand-alone function that gets called on-demand, or as a slot
 *	that gets connected to a signal for  automatic response sending.
 */
void CommandChannel::sendResponse(QSharedPointer<Picto::ProtocolResponse> response)
{
	if(response->write(producerSocket) < 1)
		qDebug("CommandChannel::sendResponse failed to send requested response");

}

/*! \Called when a socket emits a disconnect signal
 *
 *	There are two scenarios in which this handler gets called:
 *		1. We are closing the channel
 *		2. There was some sort of error
 *	In the first case, we should let the disconnect happen, but in the second
 *	case we need to reconnect immediately.  The reconnect variable is
 *	used to differentiate between the situations.
 */
void CommandChannel::disconnectHandler()
{
	status = disconnected;

	if(!reconnect)
		return;

	//We could get this signal from either socket...
	QTcpSocket *socket = (QTcpSocket*)QObject::sender();

	//assume the disconnect is due to error and attermpt to reconnect
	socket->connectToHost(serverAddr, serverPort, QIODevice::ReadWrite);

	if(socket->waitForConnected(1000))
	{
		status = connected;
	}
	else
	{
		status = disconnected;
	}

}

/*!	\brief Resends all registered commands for which responses have not been received
 *
 *	Every time a registered command is sent, it is added to the list of pending
 *	commands.  When a registered response is received, the corresponding command is 
 *	removed from that list.  This function clears out the pending commands list,
 *	and resends all of the registered commands that have not yet received.
 */
void CommandChannel::resendPendingCommands()
{
	/////////TESTING
	/*FILE* outFile;
	outFile = fopen("pendingCommands.txt","w");
	fprintf(outFile, "Resending pending commands...\n");
	fprintf(outFile, "  %i commands to send\n\n",pendingCommands_.size());

	QMap<QUuid,QSharedPointer<ProtocolCommand> >::const_iterator i = pendingCommands_.begin();

	while(i != pendingCommands_.end())
	{
		QString method = i.value()->getMethod();
		QString target = i.value()->getTarget();
		QString content = i.value()->getContent();

		QString cmd = QString("  %1 %2\n%3\n\n").arg(method).arg(target).arg(content);
		fprintf(outFile, cmd.toAscii());
		i++;
	}*/
	//QString debug = QString("Pending command count: %1").arg(pendingCommands_.size());
	//Q_ASSERT_X(false,"",debug.toAscii());

	//////////////////////////////////////////////


	QList<QSharedPointer<ProtocolCommand> > commandsToResend;
	foreach(QSharedPointer<ProtocolCommand> command, pendingCommands_)
	{
		commandsToResend.append(command);
	}

	pendingCommands_.clear();

	foreach(QSharedPointer<ProtocolCommand> command, commandsToResend)
	{
		sendRegisteredCommand(command);
	}
}

}; //namespace Picto

