#include <QStringList>
#include <QTime>
#include <QCoreApplication>

#include "CommandChannel.h"

namespace Picto {

CommandChannel::CommandChannel(QObject *parent)
	:QObject(parent),
	status_(disconnected),
	reconnect_(true)
{
	//set up the socket
	consumerSocket_ = new QTcpSocket(this);

	connect(consumerSocket_, SIGNAL(disconnected()), this, SLOT(disconnectHandler()));
}

CommandChannel::CommandChannel(QHostAddress serverAddress, quint16 serverPort_, QObject *parent)
	:QObject(parent),
	serverAddr_(serverAddress),
	serverPort_(serverPort_),
	status_(disconnected),
	reconnect_(true)
{
	//set up the socket
	consumerSocket_ = new QTcpSocket(this);

	connect(consumerSocket_, SIGNAL(disconnected()), this, SLOT(disconnectHandler()));

	initConnection();
}

CommandChannel::~CommandChannel()
{
	if(status_ == connected)
		closeChannel();

	delete consumerSocket_;
}

void CommandChannel::connectToServer(QHostAddress serverAddress, quint16 serverPort_)
{
	if(status_ != disconnected)
		return;

	this->serverAddr_ = serverAddress;
	this->serverPort_ = serverPort_;

	status_ = disconnected;

	initConnection();
}

void CommandChannel::initConnection()
{
	//set multipart boundary to a null string
	multipartBoundary_ = "";

	//connect to the server
	consumerSocket_->connectToHost(serverAddr_, serverPort_, QIODevice::ReadWrite);

	if(consumerSocket_->waitForConnected(5000))
	{
		status_ = connected;
		emit channelConnected();
	}
	else
	{
		status_ = disconnected;
	}
}


/*!	\brief Closes the channel (also called from the destructor)
 *
 */
void CommandChannel::closeChannel()
{
	reconnect_ = false;
	consumerSocket_->close();
}


//! Checks the network for any new incoming responses, adds them to the queue, and returns
//! the number of responses in the queue.  Used when the channel is in polled mode
int CommandChannel::incomingResponsesWaiting()
{
	//before looking for incoming responses, we need to give the
	//socket a chance to update its buffers, waitForReadyRead
	//accomplishes this.
	consumerSocket_->waitForReadyRead(0);
	readIncomingResponse();
	return incomingResponseQueue_.size();
}

//! Returns the top response in the queue, or a null pointer if there are no
//! responses in the queue (this will always happen if you aren't in polled mode)
QSharedPointer<ProtocolResponse> CommandChannel::getResponse()
{
	if(incomingResponseQueue_.empty())
		return QSharedPointer<ProtocolResponse>();
	else
		return incomingResponseQueue_.takeFirst();
}



/*! \brief Waits for a response to arrive
 *
 *	Since the networking code works best with some sort of event loop, this
 *	function creates one for us.  Note that if the timeout timer is 0, we still
 *	call process events.
 *
 *	The function returns true if there is a waiting response.
 */
bool CommandChannel::waitForResponse(int timeout)
{
	if(timeout == 0)
	{
		//QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		if(incomingResponsesWaiting() > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		QTime timeoutTimer;
		timeoutTimer.start();

		while(true)
		{
			//QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

			if(incomingResponsesWaiting() > 0)
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


//! Reads an incoming response and adds it to the queue
void CommandChannel::readIncomingResponse()
{
	int bytesRead;
	while(consumerSocket_->bytesAvailable() > 0)
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse());

		//if we're expecting a multipart part response, we should go ahead and set the
		//boundary string for the response.
		if(!multipartBoundary_.isEmpty())
			response->setMultiPartBoundary(multipartBoundary_);

		bytesRead = response->read(consumerSocket_);

		//if there is an error, bytes read will be negative
		if(bytesRead >= 0)
		{
			if(response->getMultiPart() == MultiPartResponseType::MultiPartInitial)
				multipartBoundary_ = response->getMultiPartBoundary();
			else if(!multipartBoundary_.isEmpty())
				multipartBoundary_="";

			//Check to see if this is a registered response
			QString commandId = response->getFieldValue("Command-ID");
			if(!commandId.isEmpty())
			{
				if(pendingCommands_.contains(QUuid(commandId)))
				{
					pendingCommands_.remove(QUuid(commandId));
					incomingResponseQueue_.push_back(response);
				}
			}
			else
			{
				incomingResponseQueue_.push_back(response);
			}
		}
		else
		{
			Q_ASSERT(false); //This is for debugging purposes only
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
	//This is an incredibly dumb call.
	//
	//If the CommandChannel is being used in a situation with no event loop, the
	//sockets behave oddly.  When the socket loses its connection, it doesn't realize it.
	//This means that we can't reliably check for the connection by calling socket::state().
	//A call to socket->flush also fails to work.
	//Instead, I call waitForReadyRead, which has the side effect of realizing that the connection
	//is dead, emmitting the disconnected signal, and then calling the reconnect_ slot.
	//
	//Surely there is a better way to do this, but I can't find one and I've already spent
	//way too long dealing with this issue.
	consumerSocket_->waitForReadyRead(0);

	//We always add a session-ID field, even if it's a null value
	command->setFieldValue("Session-ID",sessionId_.toString());

	if(command->write(consumerSocket_) < 1)
	{
		qDebug("CommandChannel::sendCommand failed to send requested command ");
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

/*! \Called when a socket emits a disconnect signal
 *
 *	There are two scenarios in which this handler gets called:
 *		1. We are closing the channel
 *		2. There was some sort of error
 *	In the first case, we should let the disconnect happen, but in the second
 *	case we need to reconnect_ immediately.  The reconnect_ variable is
 *	used to differentiate between the situations.
 */
void CommandChannel::disconnectHandler()
{
	status_ = disconnected;

	if(!reconnect_)
	{
		emit channelDisconnected();
		return;
	}

	//We could get this signal from either socket...
	QTcpSocket *socket = (QTcpSocket*)QObject::sender();
	
	//assume the disconnect is due to error and attermpt to reconnect_
	socket->connectToHost(serverAddr_, serverPort_, QIODevice::ReadWrite);

	if(socket->waitForConnected(1000))
	{
		status_ = connected;
	}
	else
	{
		emit channelDisconnected();
		status_ = disconnected;
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

