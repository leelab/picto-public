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
 *	The CommandChannel object abstracts the communication between any 
 *  two devices which communicate via network protocols.  For example,
 *  PictoDirector would use a CommandChannel to send commands to PictoServer.
 *
 *	The channel is bidirectional, but is intended to be used by the client in 
 *	a client/server pair.  The channel can send protocolCommands and receive
 *	ProtocolResponses.
 *
 *	The CommandChannel can be connected to a server in multiple ways:
 *		1. The CommandChannel can be passed a host address and port in the constructor
 *		2. The connectToServer slot can be used.  (This is particularly useful if it's connected
 *		   to the serverDiscoverers foundServer signal).
 *
 *	The CommandChannel is designed to be used mostly by the engine, so it doesn't
 *  take advantage of the event driven nature of Qt (If you decide you want to do this, an 
 *	earlier version of the object exists that uses signals and slots, and this can be found in
 *	the SVN archive).
 *
 *  To use the object, the owner calls the incomingResponsesWaiting function.  This 
 *  checks the command socket for any incoming data, and then reports back the
 *	number of responses queed up for processing.  Then the readResponse 
 *	function can be called for each item in the queue. The advantage of this approach is
 *	that the calling object maintains nearly full control of the CPU.  The disadvantage
 *	is that it will burn cycles if there aren't any incoming responses, and it requires
 *	extra coding.  For outgoing commands, the sendCommandfunction is called directly.
 *
 *	The registered functions are a special subset of the send/receive functions. These are used
 *	if you need to keep track of which commands have received responses.
 *
 *	NOTE: An earlier version of this object allowed true bidirection communication (you could receive
 *	commands and send responses.  If this functionality is desired, it can be dug out of the SVN archives.
 *
 *	!!!!!!   WARNING    !!!!!!
 *	The command channel has a weird "feature" (err bug).  Since we call proccess events in the
 *	waitForResponse function, it is entirely possible to get responses out of order.  Here's an
 *	example:
 *		1. The following code is executed:
 *				sendCommand(commandA)
 *				waitForResponse(1000)
 *		2. While waiting, some event occurs that results in this code being executed:
 *				sendCommand(commandB)
 *				waitForResponse(1000)
 *		3. Response A then arrives and gets processed by the function that sent command B
 *		4. Program crashes.
 *	To fix this, I'm going to stop making calls to QCoreApplication::processEvents().  However,
 *	this is DANGEROUS.  I seem to recall that these calls were essential for working with sockets
 *	if there isn't a real event loop, but everything seems to be working now, so who knows...
 *	(I did rerun the unit tests, and everything passed.)
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
	
	
	int numIncomingResponses(){return incomingResponseQueue_.size();};

	bool waitForResponse(int timeout=0);

	int pendingResponses() { return pendingCommands_.size(); };  //! Returns the number of responses we are waiting for
	QDateTime resendPendingCommands();

	void setSessionId(QUuid sessionId);
	QUuid getSessionId(){return sessionId_;};
	void clearSessionId();

	typedef enum
	{
		connected, disconnected
	} ChannelStatus;

	//!Check channel status_
	ChannelStatus getChannelStatus() { return status_; };

	bool isConnected() { return status_ == connected; };
	bool assureConnection(int acceptableTimeoutMs = 0);

	void closeChannel();
	QHostAddress getIpAddress();
public slots:
	void connectToServer();

signals:
	void connectAttemptFailed();
	void channelDisconnected();
	void channelConnected();

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
