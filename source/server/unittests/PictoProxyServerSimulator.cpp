#include "PictoProxyServerSimulator.h"
#include "../../common/network/serverdiscoverer.h"
#include <QUdpSocket>
#include <QNetworkInterface>
using namespace PictoSim;
using namespace ProxyServer;

PictoProxyServerSimulator::PictoProxyServerSimulator(PictoSystemState* systemState, QString deviceName) : PictoDeviceSimulator(systemState, deviceName)
{}

QString PictoProxyServerSimulator::GetDeviceTypeName()
{
	return "PROXYSERVER_SIMULATOR";
}

void PictoProxyServerSimulator::Init()
{
	tcpServer_ = QSharedPointer<QTcpServer>(new QTcpServer());
	timerRunning_ = false;
}

void PictoProxyServerSimulator::Deinit()
{
	tcpSocket_ = QSharedPointer<QTcpSocket>();
	tcpServer_ = QSharedPointer<QTcpServer>();
}

void PictoProxyServerSimulator::Act(QSharedPointer<SimActionDesc> actionDesc)
{
	QString message;
	switch(actionDesc->type_)
	{
	case DISCOVERSERVER:
		systemState_->AssureDeviceExistance(systemState_->GetServerName(),SERVER);
		if(systemState_->GetDevice(systemState_->GetServerName())->isBeingTested)
		{
			Picto::ServerDiscoverer serverDiscoverer;
			serverDiscoverer.discover();
			if(!serverDiscoverer.waitForDiscovered(10000))
			{
				QFAIL((GetDeviceTypeName() + " did not recieve a response to its DISCOVER call from the Server").toAscii() );
				return;
			}
			ConnectToHost(tcpSocket_,serverDiscoverer.getAddress(),serverDiscoverer.getPort());
		}
		break;
	case ANNOUNCE:
		{
			QHostAddress serverAddress;
			QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();
			if(!hostAddresses.empty())
			{
				serverAddress.setAddress(hostAddresses[0].toIPv4Address());
			}
			tcpServer_->listen(serverAddress,42420);

			QUdpSocket udpSendSocket;
			QByteArray datagram = QString("ANNOUNCE %1:%2 ACQ/1.0\r\n\r\n").arg(GetDeviceName()).arg(42420).toAscii();

			udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
										QHostAddress::Broadcast, 42424);
			while( tcpSocket_.isNull() && !tcpServer_->waitForNewConnection(500) && !InterruptableSleep(0) );
			tcpSocket_ = QSharedPointer<QTcpSocket>(tcpServer_->nextPendingConnection());
			
			while(!InterruptableSleep(500))
			{
				ReadIncomingMessage("",message,tcpSocket_,100,false);
				if(message == "")
					continue;

				QString expected = "GET /data ACQ/1.0";
				if(!message.startsWith("GET /data ACQ/1.0"))
				{
					QVERIFY2(message == expected, (GetDeviceTypeName() +" received an unexpected or badly formed message.\nActual: " 
					+ message + "\nExpected: " + expected).toAscii());
				}
				else
				{
					SendLatestData(actionDesc);
				}
			}
		}
		break;
	case DEPART:
		QUdpSocket udpSendSocket;
		QByteArray datagram = QString("DEPART %1:%2 ACQ/1.0\r\n\r\n").arg(GetDeviceName()).arg(42420).toAscii();

		udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
										QHostAddress::Broadcast, 42424);
		break;
	}
}

void PictoProxyServerSimulator::SendLatestData(QSharedPointer<SimActionDesc> actionDesc)
{
	QSharedPointer<AnnounceDesc> action = actionDesc.staticCast<AnnounceDesc>();
	QSharedPointer<SimMsgDesc> msg;
	QString acqMessage = "ACQ/1.0 200 OK\r\nContent-Length: %1\r\nContent-Type: text/xml; "\
								"charset=\"utf-8\"\r\nDate: %2\r\n"\
								"Server: PictoProxyServer\r\n\r\n%3";
	QString xmlMessage = "<?xml version=\"1.0\"?>"\
						"<ResponseACQ1.0><device>SimulatedDevice</device>"\
						"<deviceStatus>running</deviceStatus><sampleRate>1</sampleRate>%1"\
						"<numEvents>%2</numEvents></ResponseACQ1.0>";
	QString body = "";
	int numEvents = 0;

	if(action->msgs_.size() && !timerRunning_)
	{
		timerRunning_ = true;
		msg = action->msgs_.front();
		startDataTime_ = msg->timestamp_;
		timer_.start();
	}

	while( action->msgs_.size() 
		&& (((msg = action->msgs_.front())->timestamp_ - startDataTime_ - timer_.elapsed()) < 0) )
	{
		action->msgs_.pop_front();
		numEvents++;
		switch(msg->type_)
		{
		case AnnounceDesc::SPIKE:
			body += QString("<event type=\"spike\"><timestamp>%1</timestamp><channel>%2</channel><unit></unit><wave>0</wave></event>").arg(msg->timestamp_).arg(msg.staticCast<AnnounceDesc::SpikeMsgDesc>()->channel_);
			break;
		case AnnounceDesc::EXTEVENT:
			body += QString("<event type=\"externalEvent\"><timestamp>%1</timestamp><eventCode>%2</eventCode></event>").arg(msg->timestamp_).arg(msg.staticCast<AnnounceDesc::ExtEventMsgDesc>()->eventID_);
			break;
		}
	}
	xmlMessage = xmlMessage.arg(body).arg(numEvents);
	acqMessage = acqMessage.arg(xmlMessage.length()).arg(timer_.currentTime().toString()).arg(xmlMessage);
	SendMessage(acqMessage,tcpSocket_);
}