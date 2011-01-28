#include "PictoServerSimulator.h"
#include <QNetworkInterface>
using namespace PictoSim;
using namespace Server;
//Not Yet Implemented
PictoServerSimulator::PictoServerSimulator(PictoSystemState* systemState, QString deviceName) : PictoDeviceSimulator(systemState, deviceName)
{}
QString PictoServerSimulator::GetDeviceTypeName()
{
	return "SERVER_SIMULATOR";
}

void PictoServerSimulator::Init()
{
	// Tcp server must be created in the thread where it will be used
	tcpServer_ = QSharedPointer<QTcpServer>(new QTcpServer());
}


void PictoServerSimulator::Act(QSharedPointer<SimActionDesc> actionDesc)
{

	QString message;
	switch(actionDesc->type_)
	{
	case INTERFACEDIRECTOR:
		{
			//Get Discovered
			udpSocket_ = QSharedPointer<QUdpSocket>(new QUdpSocket());
			udpSocket_->bind(42424);
			while(!udpSocket_->hasPendingDatagrams() && InterruptableSleep(200));
			QHostAddress hostAddr;
			QByteArray incoming;
			quint16 port = 0;
			incoming.resize(udpSocket_->pendingDatagramSize());
			udpSocket_->readDatagram(incoming.data(),incoming.size(),&hostAddr,&port);
			QByteArray datagram = QString("ANNOUNCE %1:%2 ACQ/1.0\r\n\r\n").arg(GetDeviceName()).arg(42424).toAscii();
			udpSocket_->writeDatagram(datagram.data(), datagram.size(),
			hostAddr, port);

			// Get my ip address
			QHostAddress serverAddress;
			QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();
			if(!hostAddresses.empty())
			{
				serverAddress.setAddress(hostAddresses[0].toIPv4Address());
			}
			// Listen on my ip address (which we just found) on port 42424 (which is the server's port)
			tcpServer_->listen(serverAddress,42424);

			// Wait for the PictoDirector to connect to me.  Make sure to call InterruptableSleep() every so often
			// so that the testbench can break me out of this action if necessary.
			while( tcpSocket_.isNull() && !tcpServer_->waitForNewConnection(500) && !InterruptableSleep(0) );
			// Someone connected to me.  I assume that we're only running me and the director, so I only care about the first one and 
			// I get a pointer to the socket and stop listening for more connections.
			tcpSocket_ = QSharedPointer<QTcpSocket>(tcpServer_->nextPendingConnection());
			
			while(!InterruptableSleep(500))
			{	//Wait for a DIRECTORUPDATE message from the PictoDirector.  When it arrives, check it and send back data, then wait again.
				ReadIncomingMessage("",message,tcpSocket_,100,false);
				if(message == "")
					continue;

				QString expected = "UPDATEDIRECTOR";
				if(!message.startsWith(expected))
				{
					QVERIFY2(message == expected, (GetDeviceTypeName() +" received an unexpected or badly formed message.\nActual: " 
					+ message + "\nExpected: " + expected).toAscii());
				}
				else
				{
					SendLatestMessages(actionDesc);
				}
			}
		}
		break;
	case INTERFACEPROXY:
		{
		}
		break;
	case INTERFACEWORKSTATION:
		{
		}
		break;
	}
}

void PictoServerSimulator::Deinit()
{
	// Tcp server must be deleted in the thread where it will be created
	tcpServer_ = QSharedPointer<QTcpServer>();
	// tcp socket must be deleted in the thread where it was created.
	tcpSocket_ = QSharedPointer<QTcpSocket>();
	udpSocket_ = QSharedPointer<QUdpSocket>();

}

void PictoServerSimulator::SendLatestMessages(QSharedPointer<SimActionDesc> actionDesc)
{
	if(actionDesc->msgs_.size() == 0)
		return;
	QString msgTemplate = "PICTO/1.0 200 %1";
	QString msgStr;
	QSharedPointer<SimMsgDesc> msg = actionDesc->msgs_.front();
	switch(msg->type_)
	{
	case InterfaceDirectorDesc::NEWSESSION:
		msgStr = msgTemplate.arg("NEWSESSION");
		break;
	case InterfaceDirectorDesc::ENDSESSION:
		msgStr = msgTemplate.arg("ENDSESSION");
		break;
	case InterfaceDirectorDesc::LOADEXP:
		msgStr = msgTemplate.arg("LOADEXP");
		break;
	case InterfaceDirectorDesc::START:
		msgStr = msgTemplate.arg("START");
		break;
	case InterfaceDirectorDesc::STOP:
		msgStr = msgTemplate.arg("STOP");
		break;
	case InterfaceDirectorDesc::PAUSE:
		msgStr = msgTemplate.arg("PAUSE");
		break;
	}
	msgStr += "\r\n\r\n";
	if(timer_.elapsed() < msg->timestamp_)
		return;
	actionDesc->msgs_.pop_front();
	SendMessage(msgStr,tcpSocket_);
	timer_.start();
}