#include <QStringList>
#include <QCoreApplication>

#include "CommandChannel.h"
#include "../memleakdetect.h"

namespace Picto {
#define RECONNECT_POLL_INTERVAL_MS 100 
CommandChannel::CommandChannel(QUuid sourceId, QString sourceType, QObject *parent)
	:QObject(parent),
	status_(disconnected),
	reconnect_(true),
	sourceId_(sourceId),
	sourceType_(sourceType),
	currRegCmdID_(Q_UINT64_C(1)),
	discoverMsgSentTime_(QDateTime::currentDateTime()),
	earliestPendingCommand_(QDateTime::currentDateTime()),
	resendEnabled_(true),
	resendPendingInterval_(10),
	lastReconnectTime_(QDateTime::currentDateTime()),
	discoveryPort_(0)
{
	//set up the socket
	consumerSocket_ = QSharedPointer<QTcpSocket>(new QTcpSocket());
	discoverySocket_ = QSharedPointer<QUdpSocket>(new QUdpSocket());
	discoveryPort_ = 0;
	connect(consumerSocket_.data(), SIGNAL(disconnected()), this, SLOT(disconnectHandler()));
	discoverMsgSentTime_ = QDateTime::currentDateTime().addSecs(-20);
}

CommandChannel::CommandChannel(QUuid sourceId, QString sourceType, QHostAddress serverAddress, quint16 serverPort_, QObject *parent)
	:QObject(parent),
	serverAddr_(serverAddress),
	serverPort_(serverPort_),
	status_(disconnected),
	reconnect_(true),
	sourceId_(sourceId),
	sourceType_(sourceType),
	currRegCmdID_(Q_UINT64_C(1)),
	discoverMsgSentTime_(QDateTime::currentDateTime()),
	earliestPendingCommand_(QDateTime::currentDateTime()),
	resendEnabled_(true),
	resendPendingInterval_(10),
	lastReconnectTime_(QDateTime::currentDateTime()),
	discoveryPort_(0)
{
	//set up the socket
	consumerSocket_ = QSharedPointer<QTcpSocket>(new QTcpSocket());
	discoverySocket_ = QSharedPointer<QUdpSocket>(new QUdpSocket());
	discoveryPort_ = 0;
	connect(consumerSocket_.data(), SIGNAL(disconnected()), this, SLOT(disconnectHandler()));
	discoverMsgSentTime_ = QDateTime::currentDateTime().addSecs(-20);
	
	//set multipart boundary to a null string
	connectToServer();
}

CommandChannel::~CommandChannel()
{
	if(status_ == connected)
		closeChannel();

	//delete consumerSocket_;
}

//! \brief Don't call from time sensitive zone
void CommandChannel::connectToServer()
{
	if(!assureConnection(200))
	{
		QCoreApplication::processEvents();
		emit connectAttemptFailed();
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

QHostAddress CommandChannel::getIpAddress()
{
	if(consumerSocket_)
		return consumerSocket_->localAddress();
	return QHostAddress();
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

void CommandChannel::setStatusManager(QSharedPointer<ComponentStatusManager> statusManager)
{
	statusManager_ = statusManager;
}
void CommandChannel::addResponseHandler(QSharedPointer<ProtocolResponseHandler> responseHandler, bool replaceExisting)
{
	QString directive = "";
	Q_ASSERT(responseHandler);
	directive = responseHandler->method();
	if(directive != "" && (!responseHandlerMap_.contains(directive) || replaceExisting))
		responseHandlerMap_[directive] = responseHandler;
}

// returns true if it finished processing all pending commands
bool CommandChannel::processResponses(int timeoutMs)
{
		QTime timeoutTimer;
		bool loopForever = (timeoutMs < 0);
		timeoutTimer.start();
		QSharedPointer<Picto::ProtocolResponse> response;
		QSharedPointer<ProtocolResponseHandler> responseHandler;
		QDateTime nextPendingMessageTime = QDateTime::currentDateTime();
		bool keepLooping = true;
		do
		{
			do
			{
				if(!waitForResponse(0))
					break;
				response = getResponse();
				if(!response.isNull())
				{
					QString directive = response->getDecodedContent();
					int directiveEnd = directive.indexOf(' ');
					int altDirectiveEnd = directive.indexOf('\n');
					if((altDirectiveEnd > 0) && (altDirectiveEnd < directiveEnd))directiveEnd = altDirectiveEnd;
					QString statusDirective = directive.left(directiveEnd);
					//qDebug("Directive Received: " + statusDirective.toAscii());
					responseHandler = responseHandlerMap_.value(statusDirective,QSharedPointer<ProtocolResponseHandler>());
					if(!responseHandler.isNull())
					{
						responseHandler->acceptResponse(response);
					}
					else
					{
						Q_ASSERT(!responseHandler.isNull());
					}
				}
			}while(numIncomingResponses() > 0);

			int remainingMs = timeoutMs - timeoutTimer.elapsed();
			if(assureConnection((timeoutMs == 0)?0:(loopForever)?100:(remainingMs<0)?0:remainingMs))	// This will verify connection and attempt to reconnect if there's time left
				if(timeoutTimer.elapsed() > timeoutMs)
					nextPendingMessageTime = resendPendingCommands().addSecs(resendPendingInterval_);

			// Update Status Manager
			if(!statusManager_.isNull())
			{
				remainingMs = timeoutMs - timeoutTimer.elapsed();
				statusManager_.toStrongRef()->update((timeoutMs<=0)?timeoutMs:(remainingMs<0)?0:remainingMs);
			}
			remainingMs = timeoutMs - timeoutTimer.elapsed();
			if(!loopForever && QDateTime::currentDateTime().addMSecs(remainingMs) < nextPendingMessageTime)
				keepLooping = false;
		}while((loopForever || ((pendingResponses() > 0) && (timeoutTimer.elapsed() < timeoutMs) && keepLooping)) && !(statusManager_.toStrongRef() && statusManager_.toStrongRef()->exitTriggered()) );
		if(pendingResponses() == 0)
			return true;
		return false;
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
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

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
	QSharedPointer<ProtocolCommand> pendingCommand;
	QDateTime currCreatedTime;
	while(consumerSocket_->bytesAvailable() > 0)
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse());

		//if we're expecting a multipart part response, we should go ahead and set the
		//boundary string for the response.
		if(!multipartBoundary_.isEmpty())
			response->setMultiPartBoundary(multipartBoundary_);

		bytesRead = response->read(consumerSocket_.data());

		//if there is an error, bytes read will be negative
		if(bytesRead >= 0)
		{
			if(response->getMultiPart() == MultiPartResponseType::MultiPartInitial)
				multipartBoundary_ = response->getMultiPartBoundary();
			else if(!multipartBoundary_.isEmpty())
				multipartBoundary_="";

			//Check to see if this is a registered response
			QString commandIdField = response->getFieldValue("Command-ID");
			if(!commandIdField.isEmpty())
			{
				QStringList commandIDs = commandIdField.split(",");
				foreach(QString commandId,commandIDs)
				{
					if(pendingCommands_.contains(commandId.toULongLong()))
					{
						pendingCommand = pendingCommands_.take(commandId.toULongLong());
						QString serverBytesStr = response->getFieldValue("Server-Bytes");
						if(serverBytesStr != "")
						{
							int serverBytes = serverBytesStr.toInt();
							if(serverBytes == 0)
								resendEnabled_ = true;
							else
								resendEnabled_ = false;
							//qDebug("serverBytesStr: " + serverBytesStr.toAscii());
						}
						
					}
				}
			}
			incomingResponseQueue_.push_back(response);
		}
		else
		{
			qDebug("Error reading data into ProtocolResponse. ERROR ID:" + QString::number(bytesRead).toAscii());//Q_ASSERT(false); //This is for debugging purposes only
		}
	}

}

/*! \brief Assures that the channel is connected, and if not starts the reconnection process.
 *	This call is non-blocking.  It checks for a connection every time the function is called
 *	and if there is no connection, it attempts to reconnect whenever it hasn't tried within reconnectIntervalMs_.  
 *	If connection is successful, it returns true and status is set accordingly.
 */
bool CommandChannel::assureConnection(int acceptableTimeoutMs)
{	
	if(consumerSocket_->state() == QAbstractSocket::ConnectedState)
	{
		//This is an incredibly dumb call.
		//
		//If the CommandChannel is being used in a situation with no event loop, the
		//sockets behave oddly.  When the socket loses its connection, it doesn't realize it.
		//This means that we can't reliably check for the connection by calling socket::state().
		//A call to socket->flush also fails to work.
		//Instead, I call waitForReadyRead, which has the side effect of realizing that the connection
		//is dead, emmitting the disconnected signal, and then calling the disconnectHandler slot.
		//
		//Surely there is a better way to do this, but I can't find one and I've already spent
		//way too long dealing with this issue.
		consumerSocket_->waitForReadyRead(0);
	}
	//Now we can really check if we're connected and if not, 
	//then if there is an acceptable timeout lets try reconnecting.
	// 
	if(consumerSocket_->state() != QAbstractSocket::ConnectedState)
	{
		status_ = disconnected;
		if(!statusManager_.isNull() && statusManager_.toStrongRef()->getStatus() == idle)
			statusManager_.toStrongRef()->setStatus(ComponentStatus::disconnected);
		if(acceptableTimeoutMs > 0)
		{
			QTime timer;
			timer.start();
			if((acceptableTimeoutMs > 50) && !discoverServer(acceptableTimeoutMs))
				return false;
			if(timer.elapsed() >= acceptableTimeoutMs)
				return false;
			consumerSocket_->connectToHost(serverAddr_, serverPort_, QIODevice::ReadWrite);
			int remainingTime = acceptableTimeoutMs - timer.elapsed();
			if(remainingTime <= 0.0)
				return false;
			consumerSocket_->waitForConnected(remainingTime);
		}
	}
	if(consumerSocket_->state() == QAbstractSocket::ConnectedState)
	{
		if(status_ != connected)
			lastReconnectTime_ = QDateTime::currentDateTime();
		status_ = connected;
		if(!statusManager_.isNull() && statusManager_.toStrongRef()->getStatus() < idle)
			statusManager_.toStrongRef()->setStatus(idle);
		return true;
	}
	return false;
}

/*!	\brief Attempts to discover server's address and port for the TCPSocket connection.
 *	The function sends a UDP discover message to the server once for each 10 seconds
 *  during which it is called without completing server discovery.  Each time is is called
 *  the function checks for a response to the UDP discover message indicating the servers
 *  ip address and port.
 *	If this function is called with a timeout less than or equal to zero it will do nothing
 *	and return false.
 */
bool CommandChannel::discoverServer(int timeoutMs)
{
	if(timeoutMs == 0)
		return false;
	if(timeoutMs < 0)
		timeoutMs = 10000;
	QTime timer;
	timer.start();
	//For some reason, the discovery socket seems to periodically forget to be bound to 
	//the port that we set.  If this happens, we close and rebind it.
	if(discoverySocket_->state() != QAbstractSocket::BoundState)
	{
		discoverySocket_->close();
		QHostAddress serverAddress(QHostAddress::Any);	
		discoveryPort_ = MINDISCOVERSERVERPORT;
		while((!discoverySocket_->bind(serverAddress, discoveryPort_)) && (discoveryPort_ < MAXDISCOVERSERVERPORT))
		{
			if(timer.elapsed() >= timeoutMs)
			{
				return false;
			}
			discoveryPort_++;
		}
	}
	if(timer.elapsed() >= timeoutMs)
		return false;

	//If a long time has passed since we sent our last DISCOVER message, send a new one.
	if(discoverMsgSentTime_.secsTo(QDateTime::currentDateTime()) >= 10)
	{
		QByteArray datagram = QString("DISCOVER %1 PICTO/1.0").arg(discoveryPort_).toAscii();
		QUdpSocket sendSocket;
		sendSocket.writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, SERVERPORT);
		discoverMsgSentTime_ = QDateTime::currentDateTime();
	}
	int msLeft = timeoutMs-timer.elapsed();
	if(msLeft <= 0)
		return false;

	//Check to see if we got any new discover messages back.
	discoverySocket_->waitForReadyRead(msLeft);
	while ((discoverySocket_->state() == QAbstractSocket::BoundState) && discoverySocket_->hasPendingDatagrams())
	{
		if(timer.elapsed() >= timeoutMs)
			return false;
		QByteArray datagram;
		datagram.resize(discoverySocket_->pendingDatagramSize());
		discoverySocket_->readDatagram(datagram.data(), datagram.size());
		QString request = datagram.data();
		QString method, targetAddress, targetPort, protocolName, protocolVersion;
		QStringList tokens = request.split(QRegExp("[ ][ ]*"));
		if(tokens.count() == 3)
		{
			method = tokens[0];

			int targetPortPosition = tokens[1].indexOf(':');
			if(targetPortPosition != -1)
			{
				targetAddress = tokens[1].left(targetPortPosition);
				serverAddr_.setAddress(targetAddress);
				targetPort = tokens[1].mid(targetPortPosition+1);
				serverPort_ = targetPort.toInt();
			}

			int protocolVersionPosition = tokens[2].indexOf('/');
			if(protocolVersionPosition != -1)
			{
				protocolName = tokens[2].left(protocolVersionPosition);
				protocolVersion = tokens[2].mid(protocolVersionPosition+1);
			}

			if(method == "ANNOUNCE" &&
			   !serverAddr_.isNull() &&
			   serverPort_ &&
			   protocolName == "PICTO")
			{
				qDebug("Time when discovered: " + QString::number(timer.elapsed()).toAscii());
				discoverySocket_->close();
				return true;
			}
		}
	}
	//qDebug("Time for discover func: " + QString::number(timer.elapsed()).toAscii() + " of max: " + QString::number(timeoutMs).toAscii());
	return false;
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
	// We need to call assureConnection every time we send a command.  It fails if there is no
	// connection and starts the process of reconnecting without blocking.
	if(!assureConnection())
		return false;

	//We also always add a source-ID field so that we can be uniquely identified by the recipient.
	command->setFieldValue("Source-ID",sourceId_.toString());

	//We also always add a source-Type field so that we our component type can be identified by the recipient
	command->setFieldValue("Source-Type",sourceType_);

	//For messages that have a status element, update it to the latest status.
	if(!statusManager_.isNull() && ((command->getMethod() == "PUTDATA") || (command->getMethod() == "COMPONENTUPDATE")))
	{
		command->setTarget(statusManager_.toStrongRef()->getName()+":"+statusManager_.toStrongRef()->getStatusAsString());
		command->setFieldValue("Session-ID",statusManager_.toStrongRef()->getSessionID().toString());
	}
	else
	{
		command->setFieldValue("Session-ID",sessionId_.toString());
	}

	if(command->write(consumerSocket_.data()) < 1)
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
bool CommandChannel::sendRegisteredCommand(QSharedPointer<Picto::ProtocolCommand> command, bool enabledResend)
{
	//Q_ASSERT_X(sessionId_ != QUuid(),"CommandChannel::sendRegisteredCommand","Registered commands may only be sent on command channels with session IDs!");
	command->setFieldValue("Command-ID",QString::number(currRegCmdID_));
	command->setFieldValue("Time-Created",QDateTime::currentDateTime().toString());
	command->setFieldValue("Time-Sent",QDateTime::currentDateTime().toString());
	if(enabledResend)
		pendingCommands_[currRegCmdID_++] = command;
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
	emit channelDisconnected();
	if(reconnect_)
	{
		// If reconnect timer isn't null, we've already started trying to reconnect,
		// and we may have been called by the assureConnection() function.
		if(status_ == connected)
			assureConnection();
		return;
	}
	status_ = disconnected;


	////We could get this signal from either socket...
	//QTcpSocket *socket = (QTcpSocket*)QObject::sender();
	//
	////assume the disconnect is due to error and attermpt to reconnect_
	//socket->connectToHost(serverAddr_, serverPort_, QIODevice::ReadWrite);

	//if(socket->waitForConnected(1000))
	//{
	//	status_ = connected;
	//}
	//else
	//{
	//	emit channelDisconnected();
	//	status_ = disconnected;
	//}

}

/*!	\brief Resends all registered commands for which responses have not been received
 *
 *	Every time a registered command is sent, it is added to the list of pending
 *	commands.  When a registered response is received, the corresponding command is 
 *	removed from that list.  This function resends all of the registered commands that 
 *	were sent over "timeoutS" seconds ago and have not yet had responses received.
 *	It returns a QDateTime that indicates when the next pending command to be sent was
 *	sent originally.
 */
QDateTime CommandChannel::resendPendingCommands()
{
	QDateTime currTime = QDateTime::currentDateTime();
	if(!resendEnabled_)
		return earliestPendingCommand_;
	if(!assureConnection(0))
		return earliestPendingCommand_;
	if(earliestPendingCommand_.secsTo(currTime) < resendPendingInterval_)
		return earliestPendingCommand_;
	QList<QSharedPointer<ProtocolCommand> > commandsToResend;
	QDateTime timeSent;
	QDateTime nextEarliestCommand = QDateTime::currentDateTime();
	foreach(QSharedPointer<ProtocolCommand> command, pendingCommands_)
	{
		// If the command was sent more than resendInterval secs ago, send it again.
		timeSent = QDateTime::fromString(command->getFieldValue("Time-Sent"));
		if(timeSent.secsTo(currTime) > resendPendingInterval_)
		{
			commandsToResend.push_back(command);
		}
		else if(timeSent < nextEarliestCommand)
		{
			nextEarliestCommand = timeSent;
		}
	}

	foreach(QSharedPointer<ProtocolCommand> command, commandsToResend)
	{
		command->setFieldValue("Time-Sent",QDateTime::currentDateTime().toString());
		sendCommand(command);
		//qDebug((QString("Sent message with ID: ") + command->getFieldValue("Command-ID")).toAscii());
	}
	earliestPendingCommand_ = nextEarliestCommand;
	return earliestPendingCommand_;
}

void CommandChannel::setSessionId(QUuid sessionId) 
{ 
	if(sessionId_ == sessionId)
		return;
	if(!statusManager_.isNull())
		statusManager_.toStrongRef()->setSessionID(sessionId);
	currRegCmdID_ = Q_UINT64_C(1);
	sessionId_ = sessionId; 
};

void CommandChannel::clearSessionId() 
{ 
	setSessionId(QUuid()); 
};


}; //namespace Picto

