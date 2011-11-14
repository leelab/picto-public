#include "PictoServerSimulator.h"
#include <QNetworkInterface>
#include <QEventLoop>
#include "../../common/common.h"
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
	case GETDISCOVERED:
		{
			QSharedPointer<GetDiscoveredDesc> desc = actionDesc.staticCast<GetDiscoveredDesc>();
			CreateConnection(desc->timeoutMs_);
		}
		break;
	case INTERFACEDIRECTOR:
		{
			if(tcpSocket_.isNull())
				QFAIL("Test Definition Error: GETDISCOVERED action must be run in server before it interfaces another device"); 
			
			while(!InterruptableSleep(0))
			{	//Wait for a COMPONENTUPDATE message from the PictoDirector.  When it arrives, check it and send back data, then wait again.
				ReadIncomingMessage("",message,tcpSocket_,500,false);
				if(message == "")
					continue;

				QString expected = "COMPONENTUPDATE";
				if(!message.startsWith("COMPONENTUPDATE") && !message.startsWith("TRIAL") && !message.startsWith("PUTDATA"))
				{
					QVERIFY2(message == expected, (GetDeviceTypeName() +" received an unexpected or badly formed message.\nActual: " 
					+ message + "\nExpected: " + "COMPONENTUPDATE, TRIAL OR PUTDATA").toAscii());
				}
				else
				{
					if(message.startsWith("TRIAL"))
						Debug() << "TRIAL message received";
					if(message.startsWith("PUTDATA"))
						Debug() << "PUTDATA message received";
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
	// tcp socket must be deleted in the thread where it was created.
	tcpSocket_ = QSharedPointer<QTcpSocket>();
	udpSocket_ = QSharedPointer<QUdpSocket>();
	// Tcp server must be deleted in the thread where it will be created
	tcpServer_ = QSharedPointer<QTcpServer>();

}

void PictoServerSimulator::CreateConnection(int timeout)
{
	QTime timer;
	timer.start();
	// Start TCP server
	// Get my ip address
	QHostAddress serverAddress;
	QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();
	foreach(QHostAddress addr,hostAddresses)
	{
		if(addr.toString().startsWith("192."))
			serverAddress.setAddress(addr.toIPv4Address());
	}
	// Listen on my ip address (which we just found) on port SERVERPORT (which is the server's port)
	tcpServer_->listen(serverAddress,SERVERPORT);

	//Start UDP Port listening on SERVERPORT
	udpSocket_ = QSharedPointer<QUdpSocket>(new QUdpSocket());
	udpSocket_->bind(SERVERPORT);
	
	//Wait for a UDP message
	QHostAddress hostAddr;
	quint16 port = 0;
	bool gotDiscovered = false;
	while(!gotDiscovered && !InterruptableSleep(0))
	{
		if(!udpSocket_->waitForReadyRead(200))
		{
			if(timer.elapsed() > timeout)
				break;
			continue;
		}
		hostAddr = QHostAddress();
		port = 0;
		QByteArray incoming;
		incoming.resize(udpSocket_->pendingDatagramSize());
		udpSocket_->readDatagram(incoming.data(),incoming.size(),&hostAddr,&port);
		// Make sure its a DISCOVER message
		QStringList tokens = QString(incoming.data()).split(QRegExp("[ ][ ]*"));
		if((tokens.count() == 3) && tokens[0] == "DISCOVER")
		{
			port = tokens[1].toInt();
			gotDiscovered = true;
		}
	}
	if(!gotDiscovered)
		QFAIL((QString() + "Server failed to get discovered within " + QString::number(timeout) + "ms").toAscii());
	// Respond with an ANNOUNCE message
	QByteArray datagram = QString("ANNOUNCE %1:%2 PICTO/1.0").arg(serverAddress.toString())
														  .arg(42424)
														  .toAscii();
	udpSocket_->writeDatagram(datagram.data(), datagram.size(),
									hostAddr, port);


	// Wait for the a TCP connection.  Make sure to call InterruptableSleep() every so often
	// so that the testbench can break me out of this action if necessary.
	while( tcpSocket_.isNull() && !tcpServer_->waitForNewConnection(500) && !InterruptableSleep(0) );
	// Someone connected to me.  I assume that we're only running me and the device that I'm testing, so I only care about the first one and 
	// I get a pointer to the socket and stop listening for more connections.
	tcpSocket_ = QSharedPointer<QTcpSocket>(tcpServer_->nextPendingConnection());
}

void PictoServerSimulator::SendLatestMessages(QSharedPointer<SimActionDesc> actionDesc)
{
	QString msgTemplate = "PICTO/1.0 200 OK\r\n\r\n%1";
	QString msgStr;
	if(actionDesc->msgs_.size() == 0)
	{
		msgStr = msgTemplate.arg("OK");
		SendMessage(msgStr,tcpSocket_);
		return;
	}
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
	if(timer_.elapsed() >= msg->timestamp_)
	{
		actionDesc->msgs_.pop_front();
		timer_.start();
	}
	else
		msgStr = msgTemplate.arg("OK");
	SendMessage(msgStr,tcpSocket_);

}