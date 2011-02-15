#include "server.h"

#include "../connections/ConnectionManager.h"

#include <QNetworkInterface>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCoreApplication>
#include <QSqlRecord>
#include <QVariant>

Server::Server(quint16 port, QSharedPointer<ServerProtocols> _protocols, QObject *parent) :
	protocols(_protocols),
	serverPort(port),
	QTcpServer(parent)
{
	QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();

	//Use the first IPv4 address that isn't localhost
	//This will probably be a valid ip address, but there could still be issues...
	foreach(QHostAddress addr, hostAddresses)
	{
		QString blah = addr.toString();
		if(addr.protocol() == QAbstractSocket::IPv4Protocol && addr != QHostAddress::LocalHost)
		{
			serverAddress.setAddress(addr.toIPv4Address());
			break;
		}
	}

	listen(serverAddress, port);

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(port);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

	//I want to make sure that the ConnectionManager singleton is initially created in 
	//the server's main thread, so I create an instance here.
	ConnectionManager::Instance();
}

void Server::incomingConnection(int socketDescriptor)
{
    ServerThread *thread = new ServerThread(socketDescriptor, protocols, this);
    connect(thread, SIGNAL(finished()), this, SLOT(endThread()));

	//ConnectionManager::Instance()->addServerThread(thread);
    thread->start();
}

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

			if(method == "DISCOVER" && target.toInt() && protocolName == "PICTO")
			{
				datagram.clear();

				datagram = QString("ANNOUNCE %1:%2 PICTO/1.0").arg(serverAddress.toString())
															  .arg(serverPort)
															  .toAscii();

				udpResponseSocket.writeDatagram(datagram.data(), datagram.size(),
												senderAddress, target.toInt());
			}
		}
	}
}

//! Called when a server thread ends
void Server::endThread()
{
	ServerThread *thread = (ServerThread*)QObject::sender();

	//remove the thread from our list
	//ConnectionManager::Instance()->removeServerThread(thread);

	//call delete later
	thread->deleteLater();
}

