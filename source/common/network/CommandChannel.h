
//! A channel for sending/receiving commands/responses over the network

/*!
 *	The CommandChannel object abstracts the communication between any 
 *  two devices which communicate via network protocols.  For example,
 *  PictoDirector would use a CommandChannel to send commands to PictoServer.
 *
 *	The channel is bidirectional, but is intended to be used by the client in 
 *	a client/server pair.  The channel can send or receive protocolCommands
 *	and ProtocolResponses
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
 *	number of responses queed up for processing.  Then the readCommand or readResponse 
 *	function can be called for each item in the queue. The advantage of this approach is
 *	that the calling object maintains nearly full control of the CPU.  The disadvantage
 *	is that it will burn cycles if there aren't any incoming responses, and it requires
 *	extra coding.  For outgoing commands, the sendCommandfunction is called directly.
 *
 *	Incoming commands are handled similarly to incoming responses, and outgoing responses
 *  are handled similarly to outgoing commands
 *
 *	The registered functions are a special subset of the send/receive functions. These are used
 *	if you need to keep track of which commands have received responses.
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

#ifndef _COMMANDCHANNEL_H_
#define _COMMANDCHANNEL_H_

#include <QObject>
#include <QList>
#include <QSharedPointer>
#include <QHostAddress>
#include <QTcpSocket>
#include <QUuid>

#include "../common.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"

namespace Picto {



#if defined WIN32 || defined WINCE
class PICTOLIB_API CommandChannel : public QObject
#else
class CommandChannel : public QObject
#endif
{
	Q_OBJECT

public:
	CommandChannel(QObject *parent = 0);
	CommandChannel(QHostAddress serverAddress, quint16 serverPort, QObject *parent = 0);
	~CommandChannel();

	int incomingResponsesWaiting();
	int incomingCommandsWaiting();

	bool sendCommand(QSharedPointer<Picto::ProtocolCommand> command);
	bool sendRegisteredCommand(QSharedPointer<Picto::ProtocolCommand> command);
	QSharedPointer<ProtocolResponse> getResponse();
	QSharedPointer<ProtocolCommand> getCommand();
	void sendResponse(QSharedPointer<Picto::ProtocolResponse> response);

	bool waitForResponse(int timeout=0);

	int pendingResponses() { return pendingCommands_.size(); };  //! Returns the number of responses we are waiting for
	void resendPendingCommands();

	void setSessionId(QUuid sessionId) { sessionId_ = sessionId; };
	void clearSessionId() { sessionId_ = QUuid(); };

	typedef enum
	{
		connected, disconnected
	} ChannelStatus;

	//!Check channel status
	ChannelStatus getChannelStatus() { return status; };

	bool isConnected() { return status == connected; };

	void closeChannel();
public slots:
	void connectToServer(QHostAddress serverAddress, quint16 serverPort);

private slots:
	void disconnectHandler();

private:
	void initConnection();
	void readIncomingCommand();
	void readIncomingResponse();


	QHostAddress serverAddr;
	quint16 serverPort;

	//We use two ports here because there are two types of communictions that
	//a CommandChannel might use.
	//	consumer: the consumer socket is used the situations in which the
	//		channel is sending commands and receiving responses
	//	producer: the consumer socket is used the situations in which the
	//		channel is receiving commands and sending responses
	//The two sockets are completely transparent to the end euser and can be used
	//more or less simultaneously.  With this approach, the command channel is
	//pretty much stateless.  For example, the following sequence of events can 
	//occur:
	//	- channel sends command A
	//	- channel receives command B
	//	- channel sends response B
	//	- channel receives response A
	// Despite the interleaved ordering, the incoming command and response queues
	// will be filled in the correct order.
	QTcpSocket *consumerSocket;
	QTcpSocket *producerSocket;

	ChannelStatus status;

	//should we attempt to reconnect in the event of a disconnect?
	bool reconnect;

	//incoming queues.  These queues are only used if we are in polled mode
	//otherwise, the incoming commands and responses are immediately sent out
	//with their signals
	QList<QSharedPointer<ProtocolCommand> > incomingCommandQueue;
	QList<QSharedPointer<ProtocolResponse> > incomingResponseQueue;

	QString multipartBoundary;

	QMap<QUuid,QSharedPointer<ProtocolCommand> > pendingCommands_;
	QUuid sessionId_;
};


}; //namespace Picto

#endif
