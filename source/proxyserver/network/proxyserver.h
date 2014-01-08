#ifndef _SERVER_H_
#define _SERVER_H_

#include <QTcpServer>
#include <QUdpSocket>
#include <QSharedPointer>

#include "../common/common.h"

#include "../protocol/ProxyServerProtocols.h"

/*!	\brief NO LONGER USED.  THIS SHOULD PROBABLY BE DELETED.  Original documentation apperas below.
 *
 *	\details The server part of the proxy server.
 *
 *	Since the proxy server is a real server, we need to implement the usual server
 *	functionality.  Fortunately, Qt has a server that we can just inherit from.  This
 *	is waht we are doing.  
 *
 *	It should be noted that the server is a nearly exact copy of the one used in PictoServer.
 *	There are some minor differences (mostly just simplification).  In an ideal world, there
 *	would be a single server object in the library, and we would inherit from it, but that 
 *	isn't the way things have gone, so we're stuck with two nearly identical pieces of code.
 *	Sorry.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class ProxyServer : public QTcpServer
{
    Q_OBJECT

public:
    ProxyServer(quint16 port, QSharedPointer<ProxyServerProtocols> _protocols, QObject *parent = 0);
signals:
	void activity();
private slots:
    void processPendingDatagrams();

private:
	QHostAddress serverAddress;
	quint16 serverPort;

	void incomingConnection(qintptr socketDescriptor);

	//ServerSettings * serverSettings;

	QUdpSocket *udpSocket;
	QSharedPointer<ProxyServerProtocols> protocols;

	const static int maxConnections = 500;
};

#endif
