#include <QStringList>
#include <qDebug>


#include "CommandChannel.h"

namespace Picto {

CommandChannel::CommandChannel(QObject *parent)
	:QObject(parent),
	polledMode(false),
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
	polledMode(false),
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

	//if we're not in polled mode, listen on the sockets for incoming data
	if(!polledMode)
	{
		connect(producerSocket, SIGNAL(readyRead()), this, SLOT(readIncomingCommand()));
		connect(consumerSocket, SIGNAL(readyRead()), this, SLOT(readIncomingResponse()));
	}

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

/*	\brief places the channel into polled or automatic mode depending on the passed in value.
 *
 *	This is used to control the way the channel handels traffic.
 *
 *	In POLLED mode, the program waits for the user to call incomingCommandsWaiting
 *	or incomingResponsesWaiting, and then checks the network socket, and enqueues
 *	all of the commands and responses that have arrived since the previous calls
 *	to the functions.  This mode is good if the end user requires more complete
 *	control over the event loop.  (i.e. If you don't want your code to be randomly
 *	interrupted, use this mode.
 *
 *	In AUTOMATIC mode, the user connects the incomingCommand and incomingResponse
 *	signals emitted by the CommandChannel to a slot somewhere else.  Then when 
 *	commands and responses arrive on the channel, they are automatically handled
 *	This is easier and more efficient than polling, but will result in random i
 *	interruptions of the executing code. 
 */
void CommandChannel::pollingMode(bool polling)
{
	if(polledMode == polling)
		return;

	polledMode = polling;
	if(polledMode)
	{
		//Disconnect the signals (this may fail since it's possible that nothing 
		//has been connected yet...)
		disconnect(producerSocket, SIGNAL(readyRead()), 0, 0);
		disconnect(consumerSocket, SIGNAL(readyRead()), 0, 0);
	}
	else
	{
		//This connection may fail if it already exists, or if the socket objects 
		//haven't been created yet
		connect(producerSocket, SIGNAL(readyRead()), this, SLOT(readIncomingCommand()));
		connect(consumerSocket, SIGNAL(readyRead()), this, SLOT(readIncomingResponse()));
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
	if(!polledMode)
		return 0;

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
	if(!polledMode)
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

//! Returns the top command in the queue, or a null pointer if there are no
//! commands in the queue (this will always happen if you aren't in polled mode)
QSharedPointer<ProtocolCommand> CommandChannel::getCommand()
{
	if(!polledMode || incomingCommandQueue.empty())
		return QSharedPointer<ProtocolCommand>();
	else
		return incomingCommandQueue.takeFirst();
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

			if(polledMode)
				incomingResponseQueue.push_back(response);
			else
				emit incomingResponse(response);
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
			if(polledMode)
				incomingCommandQueue.push_back(command);
			else
				emit incomingCommand(command);
		}
	}
}

/*! \brief Sends a command over the channel
 *
 *	sendCommand sends the passed in command over the commandChannel.  This can 
 *	be used a a stand-alone function that gets called on-demand, or as a slot
 *	that gets connected to a signal for  automatic command sending.
 */
void CommandChannel::sendCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	if(command->write(consumerSocket) < 1)
		qDebug("CommandChannel::sendCommand failed to send requested command");
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

//! Sends a message to the debug stream in the event of a socekt error
void CommandChannel::errorHandler(QAbstractSocket::SocketError err)
{
	if(QObject::sender() == consumerSocket)
		qDebug()<<"CommandChannel::errorHandler - Consumer socket error:"<<err;
	else
		qDebug()<<"CommandChannel::errorHandler - Poducer socket error:"<<err;

	/*! \todo Maybe we should do some actual error handling here?  This is a pretty
	 *  simplistic implementation.  On the other hand, the connection to the server
	 *  should be pretty robust...
	 */
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
		emit droppedConnection();
	}

}


}; //namespace Picto

