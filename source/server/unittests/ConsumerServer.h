#ifndef _CONSUMERSERVER_H_
#define _CONSUMERSERVER_H_

#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#include "../../common/common.h"

/*! \brief A simple server specifically for testing the CommandChannel
 *
 *	To test the command channel in "producer" mode, we need a source of commands.
 *	Intially, I tried tricking the regular server into sending out commands,
 *	but this turned out to be impossible, since the command handler can't figure out
 *  which connection is producer, and which is consumer, and the server is soley 
 *  designed to send responses (and I didn't want to hack up the server just 
 *	for testing).
 *
 *	The Consumer server works as follows:
 *		1. Wait for 2 connections two be made
 *		2. A TEST command will arrive on one of the two sockets (the
 *		   target is the number of commands being requested)
 *		3. The TEST command socket is the consumer socket for the
 *		   CommandChannel, so the other socket, must be the producer
 *		   socket.
 *		4. Send the requested number of POST commands out over the 
 *		   producer socket.
 *		5. Send a response to the TEST command containing a report 
 *		   on the test
 *
 */


class ConsumerServer : public QTcpServer
{
    Q_OBJECT

public:
    ConsumerServer(int port, QObject *parent = 0);

private slots:
	void readClient();
	void sendReport();

private:
	void incomingConnection(int socketDescriptor);
	void sendCommands();

	QHostAddress serverAddress;
	quint16 serverPort;

	const static int maxConnections = 500;

	//incomingCommands corresponds to the consumer socket of the CommandChannel
	//incomingResponses corresponds to the producer socket of the CommandChannel
	QTcpSocket *incomingCommands;
	QTcpSocket *incomingResponses;

	QTimer *timer;

	int numCommandsToSend;
	int numResponsesReceived;

};

#endif
