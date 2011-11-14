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
	// Tcp server must be created in the thread where it will be used
	tcpServer_ = QSharedPointer<QTcpServer>(new QTcpServer());
	timerRunning_ = false;
}

void PictoProxyServerSimulator::Deinit()
{
	// tcp socket must be deleted in the thread where it was created.
	tcpSocket_ = QSharedPointer<QTcpSocket>();
	// tcp server must be deleted in the thread where it was created.
	tcpServer_ = QSharedPointer<QTcpServer>();
}

void PictoProxyServerSimulator::Act(QSharedPointer<SimActionDesc> actionDesc)
{
	QString message;
	switch(actionDesc->type_)
	{
	case DISCOVERSERVER:
		// In the current design, this isn't strictly necessary since the Picto Server is the Proxy Server's
		// client.  Hopefully, we will fix this, in which case the ProxyServer will need to discover the server.
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
			// Get my ip address
			QHostAddress serverAddress;
			QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();
			if(!hostAddresses.empty())
			{
				serverAddress.setAddress(hostAddresses[0].toIPv4Address());
			}
			// Listen on my ip address (which we just found) on port 42420 (which is the ProxyServer's port)
			tcpServer_->listen(serverAddress,42420);

			// I send out an ANNOUNCE message to the broadcast address so that anyone who cares knows that I started
			// In practice, only the PictoServer is listening
			QUdpSocket udpSendSocket;
			QByteArray datagram = QString("ANNOUNCE %1:%2 ACQ/1.0\r\n\r\n").arg(GetDeviceName()).arg(42420).toAscii();
			udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
										QHostAddress::Broadcast, SERVERPORT);
			// Wait for the PictoServer to connect to me.  Make sure to call InterruptableSleep() every so often
			// so that the testbench can break me out of this action if necessary.
			while( tcpSocket_.isNull() && !tcpServer_->waitForNewConnection(500) && !InterruptableSleep(0) );
			// Someone connected to me.  I only care about the first one since its the server so I get a pointer to the socket
			// and stop listening for more connections.
			tcpSocket_ = QSharedPointer<QTcpSocket>(tcpServer_->nextPendingConnection());
			
			while(!InterruptableSleep(500))
			{	//Wait for a GET message from the PictoServer.  When it arrives, check it and send back data, then wait again.
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
		// Tell the network that I'm shutting down.
		QUdpSocket udpSendSocket;
		QByteArray datagram = QString("DEPART %1:%2 ACQ/1.0\r\n\r\n").arg(GetDeviceName()).arg(42420).toAscii();

		udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
										QHostAddress::Broadcast, SERVERPORT);
		break;
	}
}
/*! \brief Sends the next defined event to the Picto Server when it's timestamp says that its ready.
 *	The first time that this function is called, it sends the first event in the list right away. 
 *	After that, it waits for the time eventn.timestamp - event0.timestamp to arrive before sending
 *	future events.  All events that occured before the current time are grouped together in one
 *	message.
 */
void PictoProxyServerSimulator::SendLatestData(QSharedPointer<SimActionDesc> actionDesc)
{
	QSharedPointer<AnnounceDesc> action = actionDesc.staticCast<AnnounceDesc>();
	
	// Create message templates
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

	// If this is the first message that I'm sending, start a timer and record the starting timestamp.
	if(action->msgs_.size() && !timerRunning_)
	{
		timerRunning_ = true;
		msg = action->msgs_.front();
		startDataTime_ = msg->timestamp_;
		timer_.start();
	}

	// Add events to the list until the timestamp hasn't arrived yet or there are no more events in the list.
	while( action->msgs_.size() 
		&& (((msg = action->msgs_.front())->timestamp_ - startDataTime_ - timer_.elapsed()) < 0) )
	{
		action->msgs_.pop_front();
		numEvents++;
		// Check for the message type and build the event accordingly.
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
	// Fill in the various fields that are a function of the message body, then send the message to the Picto Server.
	xmlMessage = xmlMessage.arg(body).arg(numEvents);
	acqMessage = acqMessage.arg(xmlMessage.length()).arg(timer_.currentTime().toString()).arg(xmlMessage);
	SendMessage(acqMessage,tcpSocket_);
}