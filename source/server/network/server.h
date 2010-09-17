#ifndef _SERVER_H_
#define _SERVER_H_

#include <QTcpServer>
#include <QUdpSocket>
#include <QSharedPointer>
#include <QHostAddress>
#include <QUuid>
#include <QMap>

#include "../common/common.h"

#include "serverthread.h"

#include "../protocol/ServerProtocols.h"
#include "../protocol/ServerProtocols.h"
#include "../connections/ServerConfig.h"

/*!	\brief The server for PictoServer
 *
 *	The server handles incoming and outgoing network connections.  Luckily for us, QT
 *	already has a server class, so we just extended it.  It should be noted that there
 *	are two types of connections that are being dealt with here.  When a UDP broadcast 
 *	is observed, the server object handles it directly.  When a TCP connection arrives, 
 *	a new thread is spun up to handle incoming commands.
 *
 *	The server makes extensive use of the ProtocolCommand and ProtocolResponse objects.
 */
class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(quint16 port, QSharedPointer<ServerProtocols> _protocols, QObject *parent = 0); 

private slots:
    void processPendingDatagrams();
	void endThread();

private:
	QHostAddress serverAddress;
	quint16 serverPort;

	void incomingConnection(int socketDescriptor);

	QUdpSocket *udpSocket;
	QSharedPointer<ServerProtocols> protocols;

	const static int maxConnections = 500;

	ServerConfig config_;
};

#endif
