#include <QStringList>
#include <QCoreApplication>
#include <QUuid>

#include "../globals.h"
#include "CommandChannel.h"
#include "../update/UpdateDownloader.h"
#include "../memleakdetect.h"

namespace Picto {
#define RECONNECT_POLL_INTERVAL_MS 100	//!< Appears to no longer be used... should be deleted.

/*! \brief Constructs a new CommandChannel.
 *	\details The input sourceId is a unique ID that will be used to identify the owner of this
 *	CommandChannel in all communications.  The sourceType input is a string describing the type
 *	of application that is using this command channel (ie. "DIRECTOR").  The source type is
 *	also recorded in all communications.
 */
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

/*! \brief Constructs a new CommandChannel.
 *	\details The input sourceId is a unique ID that will be used to identify the owner of this
 *	CommandChannel in all communications.  The sourceType input is a string describing the type
 *	of application that is using this command channel (ie. "DIRECTOR").  The source type is
 *	also recorded in all communications.  The input serverPort is the port on which communication
 *	will occur with the server, if it is known at the time that this CommandChannel is constructed.
 *	\note This constructor does not appear to ever be used in Picto and should probably be removed.
 */
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

/*! \brief Used to make sure that the Socket underlying this channel is closed before the channel is destroyed.
*/
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
/*! \brief Returns the IP Address from which this CommandChannel is communicating.
*/
QHostAddress CommandChannel::getIpAddress()
{
	if(consumerSocket_)
		return consumerSocket_->localAddress();
	return QHostAddress();
}
/*! \brief Checks the network for any new incoming responses, adds them to the incomingResponseQueue_, and returns
 *	the number of responses in the queue.
 *	\details Effectively, this function is just polling for the latest responses, then returning numIncomingResponses().
 */
int CommandChannel::incomingResponsesWaiting()
{
	//before looking for incoming responses, we need to give the
	//socket a chance to update its buffers, waitForReadyRead
	//accomplishes this.
	consumerSocket_->waitForReadyRead(0);
	readIncomingResponse();
	return incomingResponseQueue_.size();
}

/*! \brief Returns the top response in the incomingResponseQueue_ and removes it from the queue.
 *	\details If the incomingResponseQueue_ is empty, an empty shared pointer will be returned.
 */
QSharedPointer<ProtocolResponse> CommandChannel::getResponse()
{
	if(incomingResponseQueue_.empty())
		return QSharedPointer<ProtocolResponse>();
	else
		return incomingResponseQueue_.takeFirst();
}
/*! \brief When used in the Director, the CommandChannel works closely with a ComponentStatusManager object.  This sets a
 *	stored pointer to that object.
 *	\details Since the Director works outside of the Qt Event loop,things get a little hairy when it comes to sockets
 *	which by their very nature like to work in the background while other things are happening.  For that reason, the processResponses()
 *	function ends up becoming a kind of event loop since it is called frequently, and this function allows it to tell the ComponentStatusManager
 *	to update.  Storing a pointer to the ComponentStatusManager also allows this CommandChannel to let the ComponentStatusManager know when
 *	certain changes occur in connection status.
 */
void CommandChannel::setStatusManager(QSharedPointer<ComponentStatusManager> statusManager)
{
	statusManager_ = statusManager;
}

/*! \brief The CommandChannel maintains a table of ProtocolResponseHandler objects that take care of handling responses from
 *	the server that include commands.  This function adds a ProtocolResponseHandler to that table.
 *	\details The ProtocolResponseHandler table is indexed by the response directive that each ProtocolResponseHandler knows
 *	how to deal with.  That way, when new responses come in, in processResponses() the appropriate handler can be quickly
 *	found and used.
 */
void CommandChannel::addResponseHandler(QSharedPointer<ProtocolResponseHandler> responseHandler, bool replaceExisting)
{
	QString directive = "";
	Q_ASSERT(responseHandler);
	directive = responseHandler->method();
	if(directive != "" && (!responseHandlerMap_.contains(directive) || replaceExisting))
		responseHandlerMap_[directive] = responseHandler;
}

/*! \brief This is a REALLY IMPORTANT function that reads ProtocolResponse objects coming in from the server, maps them to appropriate ProtocolResponseHandler objects
 *	and tells them to handle the response.  Its pretty much the main event loop of ComponentInterface objects. READ THE DETAILS.....
 *	\details This is one of those functions, the ones that are hiding in the middle of a huge stack of obscure code looking like any other function even though
 *	they are actually essentially the masters of the entire application.  The history behind this function is that long, long ago, before there were smartphones,
 *	computers were pretty fast, but not quite so fast as they are now.  At that point there was a concern that if we were to use Qt's event loop architecture (which
 *	is well designed and everything but required when you build with Qt) since we didn't have total control of what was happening behind the scenes in the application
 *	code, Qt might go and spend 100 ms trying to handle some random windows event that we don't care about and cause an experiment to skip a bunch of frames which
 *	would make data about things like subject response times to stimuli less reliable, or ruin timing in precisely timed states.  For those reasons, the decision
 *	was made to work outside of the Qt event loop and run everything in our own loop thereby allowing us to ignore all machine events apart from data coming in over a 
 *	socket from the server.  At this point, though I haven't performed any tests, I believe that decision is obsolete.  Processors are very fast, Picto is running on fairly
 *	clean machines that don't have lots of other background things going on, and if the core i5 processors that we are using in Pictoboxes are good enough to
 *	run many modern video games in all of their 3d, shaded, ray traced glory at 60 frames per second, they can probably handle our little circles and squares without
 *	too much trouble, even if we do use the QT event loop.  Added to this is a recent paper: http://jn.physiology.org/content/109/1/249.full.pdf+html discussing
 *	the monkey logic software and how the Matlab language interpreter is now fast enough on a core i5 processor to acceptably deliver stimuli and track responses.  Since
 *	we are working in C++ and they are working in Matlab.  If they can do it, it would be difficult to imagine that our code couldn't do it, even with a QT event loop
 *	running.  For this reason, I hope that at some point, someone will refactor the code for ComponentInterfaces (Director and Proxy) such that they make use
 *	of the Qt event loop and can do away with this strange event loop inside the CommandChannel.  As part of this, the PictoEngine and Experiment should be refactored
 *	as well so that we never spend more than one frame with control inside the experiment.  Each frame, control should pass down from the Qt event loop, into the current
 *	state, the frame should be presented and time stamped, and control should return to the event loop for the next frame.  Obviously this will need to be tested, but for
 *	normal experiments we should not run into problems.  We can also add code to make Picto perform self checks for missed frames, verifying that the time span between
 *	control entery into the frame presentation code from one call to the next has very low variance.  This would help experiment designers catch design errors like 
 *	"if(var i=0 ;i<10;i--){...}" which would innevitably lead to skipped frames. 
 *	
 *	In any event, the purpose of this documentation is to describe the current code, and so... 
 *
 *	This function essentially became the Event Loop because it handles responses to server commands, and in the end, ComponentInterface objects are essentially slaves to the 
 *	Server.  ComponentInterfaces wait for a NEWSESSION command, they wait for a start command, they wait for the server to end the session.  For that reason, all Director/Proxy 
 *	ComponentInterface code is functionally placed underneath this one function that handles the server responses.  This is somewhat confusing.  We just said that the 
 *	Director/Proxy are slaves to the server.  Shouldn't they be the ones supplying the responses?  The answer to this is that they gather so much data and have precise timing 
 *	requirements such they need to get rid of the data on their schedule.  That means that they send the commands to the server whenever they need to and this happens frequently
 *	enough that the server can send its logical commands back inside 'responses' to the Director/Proxy's network commands and there is still never much of a noticable lag.  
 *	When the Director/Proxy are idle, they still send periodic update commands to tell the server that thet are ready to start a session, so the server always has some 
 *	response on which to piggyback a logical command.
 *	
 *	In summary, rewrite the ComponentInterface / Experimental run system please!!!!! and the activities of this function are as follows:
 *	- Gather responses to commands sent on the channel.  Hhandle them using ProtocolResponseHander objects and mark off any registered commands that they match.
 *	- Make sure that we are still connected to the server, and attempt to reconnect if necessary.
 *	- Resend any registered commands for whom no matching responses have been received verifying that the commands were processed and their data saved.
 *	This continues until either the input number of timeoutMs milliseconds has been passed, or if -1 is input (indicating that the function never times out)
 *	this looping functionality continues until the statusManager_.toStrongRef()->exitTriggered() function returns true (which always causes the function to end).
 *	
 *	The function returns true if there are no pending responses left from the server.  If there are still some responses left that were not processed, false is 
 *	returned.
 */
bool CommandChannel::processResponses(int timeoutMs)
{
		QTime timeoutTimer;
		// A negative input means loop forever (until the status manager's ComponentStatusManager::exitTriggered() returns true)
		bool loopForever = (timeoutMs < 0);
		timeoutTimer.start();	//Use this timer to keep track of when we have passed the input timeout time
		QSharedPointer<Picto::ProtocolResponse> response;
		QSharedPointer<ProtocolResponseHandler> responseHandler;
		//This value will store the next time at which we may need to resend registered commands
		//whose replies were never recived.  The fact that its initialized to the current time is confusing, but it just
		//makes sure that this value is ignored until it gets some meaningful data.
		QDateTime nextPendingMessageTime = QDateTime::currentDateTime();
		bool keepLooping = true;
		do
		{	//This loop keeps looping so long as:
			//	- either there are still registered commands that haven't recieved matching reponses, 
			//		and the next time that a registered command will have to be resent is before the input timeout time.
			//	- or this function is set to loop forever
			//	if the statusManager's exitTriggered() function returns true it will always stop.
			do
			{	//This loop keeps looping until there are no more incoming responses available to be processed
				//by their respective ProtocolResponseHander objects.

				if(!waitForResponse(0))
					break;
				//Take the latest response off of the queue.
				response = getResponse();
				//Get the appropriate ProtocolResponseHander to process the response, and call its acceptResponse() function.
				if(!response.isNull())
				{
					QString directive = response->getDecodedContent();
					int directiveEnd = directive.indexOf(' ');
					int altDirectiveEnd = directive.indexOf('\n');
					if((altDirectiveEnd > 0) && (altDirectiveEnd < directiveEnd))directiveEnd = altDirectiveEnd;
					QString statusDirective = directive.left(directiveEnd);
					//qDebug("Directive Received: " + statusDirective.toLatin1());
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
			// This will verify connection and attempt to reconnect if there's time left before the input timeout time
			if(assureConnection((timeoutMs == 0)?0:(loopForever)?100:(remainingMs<0)?0:remainingMs))
				//If there is time left before the input timeout time, send any RegisteredCommands for whom their timeout time has passed without any matching response received.
				if(timeoutTimer.elapsed() > timeoutMs)
					nextPendingMessageTime = resendPendingCommands().addSecs(resendPendingInterval_);

			// Update Status Manager
			//We do this here because, well, someone has to do it and this function gets called at least once per frame.  
			//The StatusManager does things like sending the periodic Event code, so we need to make sure it has the opportunity to do that frequently.
			if(!statusManager_.isNull())
			{
				remainingMs = timeoutMs - timeoutTimer.elapsed();
				statusManager_.toStrongRef()->update((timeoutMs<=0)?timeoutMs:(remainingMs<0)?0:remainingMs);
			}
			remainingMs = timeoutMs - timeoutTimer.elapsed();
			//If the next time at which a registered command will need to be resent is beyond the timeout time input into this function, don't
			//continue unless the function was set to loop forever
			if(!loopForever && QDateTime::currentDateTime().addMSecs(remainingMs) < nextPendingMessageTime)
				keepLooping = false;
		}while((loopForever || ((pendingResponses() > 0) && (timeoutTimer.elapsed() < timeoutMs) && keepLooping)) && !(statusManager_.toStrongRef() && statusManager_.toStrongRef()->exitTriggered()) );
		//If all Registered commands have been matched with responses, return true
		if(pendingResponses() == 0)
			return true;
		//If there are still registered commands with no matching responses, return false.
		return false;
}



/*! \brief Waits up to the input timeout (in ms) for the command channel to have a response to process.  
 *	\details If the input is 0, this function is essentially equivalent to \code return incomingResponsesWaiting() > 0; \endcode
 */
bool CommandChannel::waitForResponse(int timeout)
{
	if(timeout == 0)
	{
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


/*! \brief Reads an incoming response and adds it to the incoming response queue.  Removes any pending registered commands referenced by the response.
 *	\details The command channel offers a registered command system.  Registered are sent to the server with a "Command-ID" and a copy of the registered
 *	command is stored in a lookup table, indexed under its command id for resending if receipt is never verified within a present time window. The server keeps track of
 *	the command ids of command messages that it recieves and in its response messages sends a list of all command-ids who's data was not only received, but also saved
 *	to disk.  When the responses are recieved by the registered commands sender, the original registered commands are removed from the lookup table.  By using this system,
 *	we are assured that all registered commands will be saved to disk, even if someone does something crazy like restart the server in the middle of an experiment.
 *	Unless something catastrophic occurs that causes the server to go offline so long that the sender machine's heap overflows, we can be assured that data will
 *	be secure.
 *
 *	In this function, we read in all incoming responses.  We check them for Command-Ids and removed all registered commands with those IDs from their lookup table.
 *	We then add the responses to an incoming response queue for processing by a ProtocolResponseHandler in the processResponses() function.
 *
 *	This function also handles the Picto Auto-update system.  Whenever a response is received from the Server, it is checked for the version of the Picto server
 *	that sent it.  If the Picto Server Version is newer than the version of the local application installation, the UpdateDownloader object is created and used to 
 *	automatically update and restart the application containing this CommandChannel.
*/
void CommandChannel::readIncomingResponse()
{
	int bytesRead;
	QSharedPointer<ProtocolCommand> pendingCommand;
	QDateTime currCreatedTime;
	QString sessionId = sessionId_.toString();
	if(!statusManager_.isNull())
	{
		sessionId = statusManager_.toStrongRef()->getSessionID().toString();
	}

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
							//qDebug("serverBytesStr: " + serverBytesStr.toLatin1());
						}
						
					}
				}
			}
			incomingResponseQueue_.push_back(response);
			//If the PictoVersion doesn't match the current Picto version and we're not in a session,
			//update the application
			QString serverPictoVersion = response->getFieldValue("PictoVersion");
			if(sessionId.isEmpty() || (sessionId == QUuid().toString()) && !serverPictoVersion.isEmpty())
			{
				if(serverPictoVersion > PICTOVERSION)
				{
					//Update Picto
					QSharedPointer<UpdateDownloader> updateSystem = UpdateDownloader::getInstance();
					if(!updateSystem->update(serverAddr_.toString(),Picto::portNums->getUpdatePort()))
					{	//If update failed, clear incoming data, break the connection and leave the loop. Don't just keep trying again and again.
						consumerSocket_->disconnectFromHost();
						consumerSocket_->readAll();
						break;	
					}
				}
				else if(serverPictoVersion < PICTOVERSION)
				{
					//If the server's version is lower than mine, clear incoming data, break the connection and leave the loop. Don't just keep trying again and again.
					consumerSocket_->disconnectFromHost();
					consumerSocket_->readAll();
					break;
				}
			}
		}
		else
		{
			qDebug("Error reading data into ProtocolResponse. ERROR ID:" + QString::number(bytesRead).toLatin1());//Q_ASSERT(false); //This is for debugging purposes only
		}
	}

}

/*! \brief Checks the channel is connected, and if not starts the reconnection process, waiting up to the input number of ms for it to complete.
 *	returns true if connected, false if disconnected.
 *	\details This call only blocks for up to the input number of ms.  It checks for a connection every time the function is called
 *	and if there is no connection, it attempts to reconnect by using discoverServer() and QTcpSocket::connectToHost().
 *	If there is an attached Status Manager, its status (disconnected or idle) is set automatically as part of this function.
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
		//Someday we need to just rewrite this whole thing properly by using the Qt Event Loop (see processResponses())
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

/*!	\brief Attempts to discover the Picto Server's address and port for the TCPSocket connection.
 *	\details The function sends a UDP discover message to the server up to once for each 10 seconds
 *  during which it is called without completing server discovery.  Each time is is called
 *  the function checks for a response to the UDP discover message indicating the servers
 *  ip address and port.
 *	If this function is called with a timeout less than or equal to zero it will do nothing
 *	and return false.
 *	If the server is discovered, the function updates all of the channel's data fields accordingly
 *	and returns true.
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
		discoveryPort_ = Picto::portNums->getMinDiscoverPort();
		while((!discoverySocket_->bind(serverAddress, discoveryPort_)) && (discoveryPort_ < Picto::portNums->getMaxDiscoverPort()))
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
		QByteArray datagram = QString("DISCOVER %1 PICTO/1.0").arg(discoveryPort_).toLatin1();
		QUdpSocket sendSocket;
		sendSocket.writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, Picto::portNums->getServerPort());
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
				qDebug("Time when discovered: " + QString::number(timer.elapsed()).toLatin1());
				discoverySocket_->close();
				return true;
			}
		}
	}
	//qDebug("Time for discover func: " + QString::number(timer.elapsed()).toLatin1() + " of max: " + QString::number(timeoutMs).toLatin1());
	return false;
}


/*! \brief Sends a ProtocolCommand over the channel
 *	\details The following fields are automatically added to the ProtocolCommand before it is sent
 *		- Source-ID:	The unique ID of the device sending this message.
 *		- Source-Type:	A string indicating the type of device that is sending this message (ie. DIRECTOR).
 *		- Session-ID:	The unique Session Id of the Session to which this ProtocolCommand is relevant
 *	Returns true if the command is sent succesfully, false otherwise.
 *	\note This function's returning true does not indicate that the receiver recieved the ProtocolCommand, only that it was succesfully sent.
 *	For a way to asynhronously be sure that the receiver recieved and saved that data included in a ProtocolCommand, use sendRegisteredCommand().
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

/*!	\brief Sends a command and registers it so we can watch for a matching response
 *	\details This function is used when you want to be able to check at a later time to determine
 *	if a response has been issued for your command.  A command sent this way 
 *	has a "Command-ID" field appended.  The command is also added to the pending
 *	commands lookup table (using the Command-ID UUID as a key).  Sending a command as
 *	"registered" allows us to see if a response was received for that command with its included data saved.
 *	We can then resend any commands which didn't receive responses in a defined time window.
 *	For more detail on how registered commands and their responses are handled, see readIncomingResponse()
 *	\sa readIncomingResponse()
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
 *	case we need to try to reconnect immediately.  The reconnect_ variable is
 *	used to differentiate between the two situations.
 *	\sa assureConnection()
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

/*!	\brief Resends all registered commands for which responses have not been received with this object's resendPendingInterval_.
 *
 *	Every time a registered command is sent, it is added to the list of pending
 *	commands.  When a registered response is received, the corresponding command is 
 *	removed from that list.  This function resends all of the registered commands that 
 *	were sent over resendPendingInterval_ seconds ago and have not yet had responses received.
 *
 *	The function checks which command will need to be resent next assuming that no response for it is received in time.  It returns a 
 *	QDateTime that indicates when the that command was sent originally so that the caller can plan when to next call this function.
 *	\sa processResponses()
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
		//qDebug((QString("Sent message with ID: ") + command->getFieldValue("Command-ID")).toLatin1());
	}
	earliestPendingCommand_ = nextEarliestCommand;
	return earliestPendingCommand_;
}

/*! \brief Sets the Session ID for which this CommandChannel will be sending data.
 *	\details The Session ID is written to any attached ComponentStatusManager.  Command-ID values
 *	for registered commands are also reset back to 1 since they only need to be unique on a per-session bases.
 */
void CommandChannel::setSessionId(QUuid sessionId) 
{ 
	if(sessionId_ == sessionId)
		return;
	if(!statusManager_.isNull())
		statusManager_.toStrongRef()->setSessionID(sessionId);
	currRegCmdID_ = Q_UINT64_C(1);
	sessionId_ = sessionId; 
};

/*! \brief Removes any Session ID that this Command Channel had stored.
 *	\details This function does not appear to be used currently.  I'm not sure that it would ever really be useful.
 */
void CommandChannel::clearSessionId() 
{ 
	setSessionId(QUuid()); 
};


}; //namespace Picto

