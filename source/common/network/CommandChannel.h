#ifndef _COMMANDCHANNEL_H_
#define _COMMANDCHANNEL_H_

#include <QObject>
#include <QList>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QHostAddress>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QUuid>
#include <QDateTime>

#include "../common.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../controlelements/FrameResolutionTimer.h"
#include "../protocol/ProtocolResponseHandler.h"
#include "ComponentStatusManager.h"

namespace Picto {


/*!	\brief A channel for sending/receiving commands/responses over the network
 *	\details This discussion needs to start with a warning.  Warning! This is a very messy class.  This class
 *	has a lot of history and its design has suffered significantly from that history... it needs to be fixed.  I will
 *	write more about this at the end.  But now, getting back to the description:
 *
 *	The CommandChannel object abstracts the communication between any 
 *  two devices which communicate via network protocols.  For example,
 *  PictoDirector would use a CommandChannel to send ProtocolCommand objects with behavioral data to the PictoServer.
 *
 *	The channel is bidirectional, but is intended to be used by the client in 
 *	a client/server pair.  The channel can send ProtocolCommand objects and receive
 *	ProtocolResponse objects.
 *
 *	The CommandChannel can be connected to a server in multiple ways:
 *		- The CommandChannel can be passed a host address and port in the constructor - Currently never used
 *		- The assureConnection() function can be used (which internally calls discoverServer() to detect a
 *			Picto Server and connect to it.
 *
 *  To use the object, the owner calls the incomingResponsesWaiting() function.  This 
 *  checks the command socket for any incoming data, and then reports back the
 *	number of responses queed up for processing.  The readResponse() 
 *	function can then be called for each item in the queue. For outgoing commands, the sendCommand() function is called directly.
 *
 *	The sendRegisteredCommand() function can be used to keep track of which commands have been recieved and processed by the server.  
 *	Registered commands for which responses were not received can be resent with resendPendingCommands().  This system is currently
 *	used by ComponentInterface objects.
 *
 *  The CommandChannel suffers significantly since it uses QTcpSockets to send commands over a network while for historical
 *	reasons, the heaviest users of the CommandChannel, ComponentInterface objects don't use the Qt Event Loop which is really required for
 *	QTcpSockets to run smoothly.  For this reason, there is a lot of general code complexity and round about function based processing for things like assuring a 
 *	network connection ( assureConnection() ), discovering a Picto Server IP Address and Port ( discoverServer() ) and making sure server
 *	ProtocolResponse messages are correctly handled ( processResponses() ).  This also leads to interfaces to components like ProtocolResponseHander objects and
 *	ComponentStatusManager objects that the CommandChannel should really know nothing about.  In an ideal world, all of
 *	these types of things would be handled by signals and slots that would be managed from the Qt Event Loop.  For more detail on this issue, see the functions
 *	mentioned above, particularly processResponses().
 *
 *	Refactoring of the Picto experimental run system to use the Qt Event loop and cleaning up this class accordingly should be considered a Priority.
 *	Until that happens, we can easily point to using the assureConnection(), sendCommand(), waitForResponse(), pendingResponses(), numIncomingResponses(), and
 *	getResponse() functions to send a ProtocolCommand, wait for, and get a returned ProtocolResponse.  Things start getting complicated pretty fast though, and its best at this 
 *	point to just look at an existing example of CommandChannel usage in the Picto Code, like the RemoteViewer.  ComponentInterface objects use the CommandChannel in
 *	a particularly central and complex fashion.  It is for ComponentInterface objects that the CommandChannel needs to include pointers to ProtocolResponseHandler and 
 *	ComponentStatusManager objects.  For further incite into what is going on here, look at the processResponses() documentation.
 *	\sa processResponses(), ComponentStatusManager, ProtocolResponseHandler, Engine::PictoEngine
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API CommandChannel : public QObject
#else
class CommandChannel : public QObject
#endif
{
	Q_OBJECT

public:
	CommandChannel(QUuid sourceId, QString sourceType, QObject *parent = 0);
	CommandChannel(QUuid sourceId, QString sourceType, QHostAddress serverAddress, quint16 serverPort_, QObject *parent = 0);
	virtual ~CommandChannel();

	int incomingResponsesWaiting();

	bool sendCommand(QSharedPointer<Picto::ProtocolCommand> command);
	bool sendRegisteredCommand(QSharedPointer<Picto::ProtocolCommand> command, bool enabledResend = true);
	QSharedPointer<ProtocolResponse> getResponse();

	void setStatusManager(QSharedPointer<ComponentStatusManager> statusManager);
	void addResponseHandler(QSharedPointer<ProtocolResponseHandler> responseHandler, bool replaceExisting = true);
	bool processResponses(int timeoutMs);
	
	/*! \brief Returns the number of ProtocolResponse objects that were recieved over the socket but not yet processed.
	*/
	int numIncomingResponses(){return incomingResponseQueue_.size();};

	bool waitForResponse(int timeout=0);

	/*! \brief Every time we send a registered command, we keep it in a lookup table until a matching response
	 *	comes in indicating that it was received and processed.  This function returns the number of commands of
	 *	this type that are waiting for responses.
	*/
	int pendingResponses() { return pendingCommands_.size(); }; 
	QDateTime resendPendingCommands();

	void setSessionId(QUuid sessionId);
	/*! \brief Returns the Session Id of the session making use of this CommandChannel.
	*/
	QUuid getSessionId(){return sessionId_;};
	void clearSessionId();
	/*! \brief The current status of this channel
	*/
	typedef enum
	{
		connected,	//!<	This channel is currently connected to the server
		disconnected	//!< This channel is not currently connected to the server
	} ChannelStatus;

	/*! \brief Returns the current ChannelStatus of this channel.
	*/
	ChannelStatus getChannelStatus() { return status_; };

	/*! \brief Returns true if this channel is currently connected to the server (ie. getChannelStatus() returns connected.*/
	bool isConnected() { return status_ == connected; };
	bool assureConnection(int acceptableTimeoutMs = 0);

	void closeChannel();
	QHostAddress getIpAddress();
public slots:
	void connectToServer();

signals:
	/*! \brief Emitted when an attempt to connect to the server fails.*/
	void connectAttemptFailed();
	/*! \brief Emitted whenever the connection to the server is disconnected, whether by intention or not.*/
	void channelDisconnected();

private slots:
	void disconnectHandler();

private:
	void readIncomingCommand();
	void readIncomingResponse();
	bool discoverServer(int timeoutMs);


	QHostAddress serverAddr_;
	quint16 serverPort_;

	QSharedPointer<QTcpSocket> consumerSocket_;
	QSharedPointer<QUdpSocket> discoverySocket_;
	quint16 discoveryPort_;

	ChannelStatus status_;

	//should we attempt to reconnect_ in the event of a disconnect?
	bool reconnect_;

	qulonglong currRegCmdID_;
	QList<QSharedPointer<ProtocolResponse> > incomingResponseQueue_;

	QString multipartBoundary_;

	QMap<qulonglong,QSharedPointer<ProtocolCommand> > pendingCommands_;
	QUuid sessionId_;
	QUuid sourceId_;
	QString sourceType_;

	QWeakPointer<ComponentStatusManager> statusManager_;						//This pointers must be weak to aviod circular references
	QMap<QString,QSharedPointer<ProtocolResponseHandler>> responseHandlerMap_;	
	QDateTime discoverMsgSentTime_;
	QDateTime earliestPendingCommand_;
	QDateTime lastReconnectTime_;
	int resendPendingInterval_;
	bool resendEnabled_;
};


}; //namespace Picto

#endif
