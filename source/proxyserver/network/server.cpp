#include "server.h"

#include <QNetworkInterface>
#include <QStringList>
//#include <QSqlDatabase>
//#include <QSqlQuery>
#include <QCoreApplication>
#include <QSqlRecord>
#include <QVariant>

#include "serverthread.h"

Server::Server(quint16 port, QSharedPointer<ServerProtocols> _protocols, QObject *parent) :
	protocols(_protocols),
	serverPort(port),
	QTcpServer(parent)
{
	QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();
	if(!hostAddresses.empty())
	{
		serverAddress.setAddress(hostAddresses[0].toIPv4Address());
	}

	listen(serverAddress, port);

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(port);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

void Server::incomingConnection(int socketDescriptor)
{
    ServerThread *thread = new ServerThread(socketDescriptor, protocols, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(thread, SIGNAL(activity()), parent(), SLOT(serverActivity()));
    thread->start();
}

/*! /todo: The Proxy server doesn't need to process pending datagrams, since it never receives any...*/
void Server::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams())
	{
        QByteArray datagram;
		QHostAddress senderAddress;
		quint16 senderPort;

        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);

		QUdpSocket udpResponseSocket;

		QString message = datagram.data();

		QString method, target, protocolName, protocolVersion;

		QStringList tokens = message.split(QRegExp("[ ][ ]*"));
		if(tokens.count() == 3)
		{
			method = tokens[0];
			target = tokens[1];

			int protocolVersionPosition = tokens[2].indexOf('/');
			if(protocolVersionPosition != -1)
			{
				protocolName = tokens[2].left(protocolVersionPosition);
				protocolVersion = tokens[2].mid(protocolVersionPosition+1);
			}

		}
	}
}
