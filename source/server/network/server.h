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
//#include "../connections/ServerConfig.h"

/*!	\brief The Server object managing the actual incoming connections and ServerThread generation for the PictoServer.
 *
 *	\details The server handles incoming and outgoing network connections.  Luckily for us, QT
 *	already has a server class, so we just extended it.  It should be noted that there
 *	are two types of connections that are being dealt with here.  The first is a UDP broadcast.  When a UDP broadcast 
 *	is observed, the server object handles it directly.  These are used only for components to DISCOVER the server
 *	IP Address and Port.  They are handled by responding with an ANNOUNCE UDP message that contains that information. 
 *	The other kind of connection is a TCP connection.  When a TCP connection arrives, a new thread is spun up to handle 
 *	incoming commands on that thread.
 *
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(quint16 port, QSharedPointer<ServerProtocols> _protocols, QObject *parent = 0);
	~Server();

private slots:
    void processPendingDatagrams();
	void endThread();

private:
	QHostAddress serverAddress;
	quint16 serverPort;

	void incomingConnection(qintptr socketDescriptor);

	QUdpSocket *udpSocket;
	QSharedPointer<ServerProtocols> protocols;

	const static int maxConnections = 500;

	//ServerConfig config_;
};

#endif
