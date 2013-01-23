#include <QStringList>
#include "ServerDiscoverer.h"
#include "../common.h"
#include "../memleakdetect.h"

namespace Picto {

ServerDiscoverer::ServerDiscoverer(QObject *parent) :
	pictoServerPort_(0),
	bFoundServer_(false),
	QObject(parent)
{
	QHostAddress serverAddress(QHostAddress::Any);
	
	port_ = MINDISCOVERSERVERPORT;

    while(!udpSocket_.bind(serverAddress, port_) && port_ < MAXDISCOVERSERVERPORT)
	{
		port_++;
	}

    connect(&udpSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

	connect(&timeoutTimer_,SIGNAL(timeout()),this,SLOT(timeout()));
}

QHostAddress ServerDiscoverer::getAddress()
{
	if(!bFoundServer_)
	{
		QHostAddress emptyAddress;

		return emptyAddress;
	}
	else
	{
		return pictoServerAddress_;
	}
}

quint16 ServerDiscoverer::getPort()
{
	if(!bFoundServer_)
	{
		return 0;
	}
	else
	{
		return pictoServerPort_;
	}
}

void ServerDiscoverer::discover(int timeout)
{
	bFoundServer_ = false;
	bRunning_ = true;

	QUdpSocket udpSendSocket;

	QByteArray datagram = QString("DISCOVER %1 PICTO/1.0").arg(port_).toLatin1();

	timeoutTimer_.setInterval(timeout);
	timeoutTimer_.setSingleShot(true);

	udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
								QHostAddress::Broadcast, SERVERPORT);
		
	timeoutTimer_.start();
}

void ServerDiscoverer::timeout()
{
	emit discoverFailed();
	bRunning_ = false;
	waitingEventLoop_.exit();
}

void ServerDiscoverer::processPendingDatagrams()
{
    while (udpSocket_.hasPendingDatagrams())
	{
		QByteArray datagram;

		datagram.resize(udpSocket_.pendingDatagramSize());
		udpSocket_.readDatagram(datagram.data(), datagram.size());

		QString request = datagram.data();

		QString method, targetAddress, targetPort, protocolName, protocolVersion;

		QStringList tokens = request.split(QRegExp("[ ][ ]*"));
		if(tokens.count() == 3)
		{
			method = tokens[0];

			int targetPortPosition = tokens[1].indexOf(':');
			if(targetPortPosition != -1)
			{
				targetAddress = tokens[1].left(targetPortPosition);
				pictoServerAddress_.setAddress(targetAddress);
				targetPort = tokens[1].mid(targetPortPosition+1);
				pictoServerPort_ = targetPort.toInt();
			}

			int protocolVersionPosition = tokens[2].indexOf('/');
			if(protocolVersionPosition != -1)
			{
				protocolName = tokens[2].left(protocolVersionPosition);
				protocolVersion = tokens[2].mid(protocolVersionPosition+1);
			}

			if(method == "ANNOUNCE" &&
			   !pictoServerAddress_.isNull() &&
			   pictoServerPort_ &&
			   protocolName == "PICTO")
			{
				bFoundServer_ = true;				
				timeoutTimer_.stop();
				bRunning_ = false;
				emit foundServer(pictoServerAddress_, pictoServerPort_);
				waitingEventLoop_.exit();
			}
		}
	}
}

bool ServerDiscoverer::waitForDiscovered(int timeout)
{
	if(bRunning_)
	{
		QTimer waitTimeout;
		connect(&waitTimeout,SIGNAL(timeout()),&waitingEventLoop_,SLOT(quit()));
		waitTimeout.setInterval(timeout);
		waitTimeout.setSingleShot(true);
		waitTimeout.start();
		waitingEventLoop_.exec();
		waitTimeout.stop();
	}

	return bFoundServer_;
}

}; //namespace Picto
