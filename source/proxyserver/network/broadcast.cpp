#include "broadcast.h"

#include <QUdpSocket>

void announce(QString proxyName)
{
	//Announce our prescence to the world (or at least our network)
	QUdpSocket udpSendSocket;

	QByteArray datagram = QString("ANNOUNCE %1 ACQ/1.0").arg(proxyName).toAscii();

	udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
								QHostAddress::Broadcast, 42420);

}

void depart(QString proxyName)
{
	//Announce our departure to the world (or at least our network)
	QUdpSocket udpSendSocket;

	QByteArray datagram = QString("DEPART %1 ACQ/1.0").arg(proxyName).toAscii();

	udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
								QHostAddress::Broadcast, 42420);

}