#include "broadcast.h"

#include <QUdpSocket>

void announce(QString proxyName, int port)
{
	//Announce our prescence to the world (or at least our network)
	QUdpSocket udpSendSocket;

	QByteArray datagram = QString("ANNOUNCE %1:%2 ACQ/1.0").arg(proxyName).arg(port).toAscii();

	udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
								QHostAddress::Broadcast, 42424);

}

void depart(QString proxyName, int port)
{
	//Announce our departure to the world (or at least our network)
	QUdpSocket udpSendSocket;

	QByteArray datagram = QString("DEPART %1:%2 ACQ/1.0").arg(proxyName).arg(port).toAscii();

	udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
								QHostAddress::Broadcast, 42424);

}