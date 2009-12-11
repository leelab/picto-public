
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
 *	The CommandChannel object can handle incoming responses two ways.  In the  
 *	polling mode, the parent object calls the incomingResponsesWaiting function.  This 
 *  checks the command socket for any incoming data, and then reports back the
 *	number of responses queed up for processing.  Then the readCommand or readResponse 
 *	function can be called for each item in the queue. The advantage of this approach is
 *	that the calling object maintains nearly full control of the CPU.  The disadvantage
 *	is that it will burn cycles if there aren't any incoming responses, and it requires
 *	extra coding.  The other approach is to let the CommandChannel notify its parent 
 *	whenever there is an incoming response.  This ansynchronous approach is done with the 
 *	incomingResponse signal.  The advantage of this is that we know instantly when a response has 
 *  arrived, but the disadvantage is that it requires the parent to be a QObject that 
 *	is willing to relinquish control of the CPU at random times.  The ansynchronous mode
 *	is the default mode, but the mode can be changed with the pollingMode function.
 *
 *	The CommandChannel object can also handle outgoing commands in two ways.  The sendCommand
 *	slot can be connected to a signal, or the sendCommand signal can be called directly.
 *
 *	Incoming commands are handled similarly to incoming responses, and outgoing responses
 *  are handled similarly to outgoing commands
 */

#ifndef _COMMANDCHANNEL_H_
#define _COMMANDCHANNEL_H_

#include <QObject>
#include <QList>
#include <QSharedPointer>
#include <QHostAddress>
#include <QTcpSocket>

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

	QSharedPointer<ProtocolResponse> getResponse();
	QSharedPointer<ProtocolCommand> getCommand();

	void pollingMode(bool polling);

	typedef enum
	{
		connected, disconnected
	} ChannelStatus;

	//!Check channel status
	ChannelStatus getChannelStatus() { return status; };

	QTcpSocket *getConsumerSocket() { return consumerSocket; };
	QTcpSocket *getProducerSocket() { return producerSocket; };




	void closeChannel();

signals:
	//! emitted when a response is received from the server
	void incomingResponse(QSharedPointer<Picto::ProtocolResponse> response);

	//!emitted when a command is received from the server
	void incomingCommand(QSharedPointer<Picto::ProtocolCommand> command);

	//!emitted when the channel has lost its connection to the remote server, and is unable to recover.
	void droppedConnection();

public slots:
	void connectToServer(QHostAddress serverAddress, quint16 serverPort);
	void sendCommand(QSharedPointer<Picto::ProtocolCommand> command);
	void sendResponse(QSharedPointer<Picto::ProtocolResponse> response);

private slots:
	void readIncomingCommand();
	void readIncomingResponse();
	void errorHandler(QAbstractSocket::SocketError err);
	void disconnectHandler();

private:
	void initConnection();


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

	bool polledMode;
	ChannelStatus status;

	//should we attempt to reconnect in the event of a disconnect?
	bool reconnect;

	//incoming queues.  These queues are only used if we are in polled mode
	//otherwise, the incoming commands and responses are immediately sent out
	//with their signals
	QList<QSharedPointer<ProtocolCommand> > incomingCommandQueue;
	QList<QSharedPointer<ProtocolResponse> > incomingResponseQueue;

	QString multipartBoundary;
};


}; //namespace Picto

#endif
