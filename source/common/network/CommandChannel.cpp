#include <QStringList>

#include "CommandChannel.h"

namespace Picto {

CommandChannel::CommandChannel(QObject *parent)
	:QObject(parent),
	polledMode(false),
	status(uninitialized),
	serverTimeout(500)		//500 ms
{
}

CommandChannel::CommandChannel(QHostAddress serverAddress, quint16 serverPort, QObject *parent)
	:QObject(parent),
	serverAddr(serverAddress),
	serverPort(serverPort),
	polledMode(false),
	status(disconnected)
{
	initConnection();
}

void CommandChannel::connectToServer(QHostAddress serverAddress, quint16 serverPort)
{
	this->serverAddr = serverAddress;
	this->serverPort = serverPort;

	status = disconnected;

	initConnection();
}

void CommandChannel::initConnection()
{
	//set up the sockets
	producerSocket = new QTcpSocket(this);
	producerSocket->connectToHost(serverAddr, serverPort, QIODevice::ReadWrite);
	consumerSocket = new QTcpSocket(this);
	consumerSocket->connectToHost(serverAddr, serverPort, QIODevice::ReadWrite);
	if(producerSocket->waitForConnected(5000) && consumerSocket->waitForConnected(5000))
		status = connected;
	else
	{
		status = disconnected;
		return;
	}

	//even in polled mode, we're going to make a couple of connections, for events that
	//are unlikely to occur (and when they do occur, we're probably in trouble anyway,
	//so grabbing a few extra cycles will be the least of our worries)
	connect(producerSocket, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(errorHandler(QAbstractSocket::SocketError)));
	connect(producerSocket, SIGNAL(disconnected()), this, SLOT(disconnectHandler()));
	connect(consumerSocket, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(errorHandler(QAbstractSocket::SocketError)));
	connect(consumerSocket, SIGNAL(disconnected()), this, SLOT(disconnectHandler()));

	//if we're not in polled mode, start listening on the socket for incoming data
	if(!polledMode)
	{
		connect(producerSocket, SIGNAL(readyRead()), this, SLOT(readIncomingCommand()));
		connect(consumerSocket, SIGNAL(readyRead()), this, SLOT(readIncomingResponse()));
	}
}

void CommandChannel::errorHandler(QAbstractSocket::SocketError err)
{
	//If we've lost the connection, start over.  
	//If that fails, emit droppedConnection
	if(consumerSocket->state() != QAbstractSocket::ConnectedState ||
		producerSocket->state() != QAbstractSocket::ConnectedState)
	{
		delete consumerSocket;
		delete producerSocket;
		status = disconnected;
		initConnection();
		if (consumerSocket->state() != QAbstractSocket::ConnectedState||
			producerSocket->state() != QAbstractSocket::ConnectedState)
			emit droppedConnection();
	}

	/*! \todo Maybe we should do some extra error handling here?  This is a pretty
	 *  simplistic implementation.  On the other hand, the connection to the server
	 *  should be pretty robust...
	 */
}

void CommandChannel::disconnectHandler()
{
	/*! \todo The disconnect handler simply emits a droppedConnection
	 *	signal.  This is inaccurate in the event that we disconnected on purpose.
	 *	Maybe this handler should just delete the socket later?
	 */
	emit droppedConnection();
}

//! Checks the network for any new incoming responses, adds them to the queue, 
//!and returns the number of responses in the queue
int CommandChannel::incomingResponsesWaiting()
{
	if(polledMode)
		return 0;

	readIncomingResponse();
	return incomingResponseQueue.size();
}

//! Checks the network for any new incoming commands, adds them to the queue, 
//!and returns the number of responses in the queue
int CommandChannel::incomingCommandsWaiting()
{
	if(polledMode)
		return 0;

	readIncomingCommand();
	return incomingCommandQueue.size();
}

//! Returns the top response in the queue, or a null pointer if there are no
//! responses in the queue (this will always happen if you aren't in polled mode)
QSharedPointer<ProtocolResponse> CommandChannel::getResponse()
{
	if(!polledMode || incomingResponseQueue.empty())
		return QSharedPointer<ProtocolResponse>();
	else
		return incomingResponseQueue.takeFirst();
}

QSharedPointer<ProtocolCommand> CommandChannel::getCommand()
{
	if(!polledMode || incomingCommandQueue.empty())
		return QSharedPointer<ProtocolCommand>();
	else
		return incomingCommandQueue.takeFirst();
}



void CommandChannel::readIncomingResponse()
{
	int bytesRead;
	while(consumerSocket->bytesAvailable() > 0)
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse());
		bytesRead = response->read(consumerSocket);

		//if there is an error, bytes read will be negative
		if(bytesRead >= 0)
		{
			if(polledMode)
				incomingResponseQueue.push_back(response);
			else
				emit incomingResponse(response);
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
			if(polledMode)
				incomingCommandQueue.push_back(command);
			else
				emit incomingCommand(command);
		}
	}
}
void CommandChannel::sendCommand(ProtocolCommand command)
{
}
void CommandChannel::sendResponse(ProtocolResponse response)
{
}

}; //namespace Picto
