#include "server.h"

#include "../connections/ConnectionManager.h"

#include <QNetworkInterface>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCoreApplication>
#include <QSqlRecord>
#include <QVariant>
#include "../../common/memleakdetect.h"

Server::Server(quint16 port, QSharedPointer<ServerProtocols> _protocols, QObject *parent) :
	protocols(_protocols),
	serverPort(port),
	QTcpServer(parent)
{
	//Use the first IPv4 address that isn't localhost and is on an interface that is up and running
	//This will probably be a valid ip address, but there could still be issues...
	QList<QNetworkInterface> networkInterfaces = QNetworkInterface::allInterfaces();
	foreach(QNetworkInterface inter, networkInterfaces)
	{
		QList<QNetworkAddressEntry> hostAddresses = inter.addressEntries();
		if( !(inter.flags() & (QNetworkInterface::IsUp | QNetworkInterface::IsRunning)) )
			continue;
		foreach(QNetworkAddressEntry addrEntry, hostAddresses)
		{
			QHostAddress addr = addrEntry.ip();
			if(addr.protocol() == QAbstractSocket::IPv4Protocol 
				&& addr != QHostAddress::LocalHost)
			{
				serverAddress.setAddress(addr.toIPv4Address());
				break;
			}
		}
		if(!serverAddress.isNull())
			break;
	}
	Q_ASSERT_X(!serverAddress.isNull(),"Server::Server","Server does not appear to have a working network connection.");

	listen(serverAddress, port);

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(port);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

	//I want to make sure that the ConnectionManager singleton is initially created in 
	//the server's main thread, so I create an instance here.
	ConnectionManager::Instance();
}

Server::~Server()
{
	delete udpSocket;
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

