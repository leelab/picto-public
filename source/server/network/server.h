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
