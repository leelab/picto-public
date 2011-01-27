#include "PictoWorkstationSimulator.h"
#include "../../common/network/serverdiscoverer.h"
#include "PictoServerSimulator.h"
#include "PictoDirectorSimulator.h"
#include <QXMLStreamReader>
#include <QUuid>
using namespace PictoSim;
using namespace Workstation;

PictoWorkstationSimulator::PictoWorkstationSimulator(PictoSystemState* systemState, QString deviceName) : PictoDeviceSimulator(systemState, deviceName)
{
}

QString PictoWorkstationSimulator::GetDeviceTypeName()
{
	return "WORKSTATION_SIMULATOR";
}

void PictoWorkstationSimulator::Init()
{
	// Tcp Socket must be created in the thread where it will be used
	tcpSocket_ = QSharedPointer<QTcpSocket>(new QTcpSocket());
	observerID_ = QUuid::createUuid().toString();
	iAmAuthorized_ = false;
}

void PictoWorkstationSimulator::Deinit()
{
	// tcp socket must be deleted in the thread where it was created.
	tcpSocket_ = QSharedPointer<QTcpSocket>();
}

void PictoWorkstationSimulator::Act(QSharedPointer<SimActionDesc> actionDesc)
{
	QString message;
	switch(actionDesc->type_)
	{
	case DISCOVERSERVER:
		// Get the server's IP Address and port, then connect my TcpSocket to it.
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
	case DIRECTORLIST:
		// Get a list of directors from the Server.  Check response for errors while building an internal list of directors.
		SendMessage("DIRECTORLIST / PICTO/1.0\r\n\r\n", tcpSocket_);
		ReadIncomingMessage("PICTO/1.0 200 OK",message,tcpSocket_,10000);
		BuildIPMap(message);
		break;
	case PROXYLIST:
		// Get a list of proxy servers from the Server.  Check response for errors while building an internal list of proxy servers.
		SendMessage("PROXYLIST / PICTO/1.0\r\n\r\n", tcpSocket_);
		ReadIncomingMessage("PICTO/1.0 200 OK",message,tcpSocket_,10000);
		BuildProxyMap(message);
		break;
	case STARTSESSION:
		{
			// Build a STARTSESSION message with the desired directory and proxy server by looking them up in the tables that
			// we built in the DIRECTORLIST and PROXYLIST actions.  This action must occur after those actions and the 
			// test will fail if it isn't.
			QSharedPointer<StartSessionDesc> desc = actionDesc.staticCast<StartSessionDesc>();
			QString directorName = desc->directorName_;
			QString proxyName = desc->proxyName_;
			QString directorIP;
			QString proxyIndex = "-1";
			session_ = "{00000000-0000-0000-0000-000000000000}";
			if(ipMap_.contains(directorName))
				directorIP = ipMap_[directorName].toString();
			else
				QFAIL( QString("TEST DEFINITION ERROR: Workstation simulator %1 could not find the IP address for %2. \n"\
							   "Make sure that all Directors have sent DIRECTORUPDATE messages and that DIRECTORLIST "\
							   "was called before using STARTSESSION." ).arg(GetDeviceName()).arg(directorName).toAscii());
			if((proxyName != "") && !proxyMap_.contains(proxyName))
			{
				QFAIL( QString("TEST DEFINITION ERROR: Workstation simulator %1 could not find the proxy index for %2.\n"\
							   "Make sure that all Proxy Servers have sent ANNOUNCE messages and that PROXYLIST "\
							   "was called before using STARTSESSION." ).arg(GetDeviceName()).arg(directorName).toAscii());
			}
			else if (proxyMap_.contains(proxyName))
			{
				proxyIndex = proxyMap_[proxyName];
			}
			SendMessage(QString("STARTSESSION %1/%2 PICTO/1.0\r\nContent-Length:63\r\nObserver-ID:%3\r\nSession-ID:{00000000-0000-0000-0000-000000000000}\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?><Experiment></Experiment>").arg(directorIP).arg(proxyIndex).arg(observerID_), tcpSocket_);
			ReadIncomingMessage("PICTO/1.0 200 OK",message,tcpSocket_,10000);
			iAmAuthorized_ = true;
			// Get the sessionID for this session from the reply message.
			QStringList bufferList = message.split("<SessionID>");
			if(bufferList.size() > 1)
			{
				session_ = bufferList[1].split("</SessionID>")[0];
			}
		}
		break;
	case TASK:
		{
			// Check what type of task action is desired and send a corresponding message.
			QSharedPointer<TaskDesc> desc = actionDesc.staticCast<TaskDesc>();
			QString target;
			switch(desc->taskAction_)
			{
			case TaskDesc::START:
				target = "start:fakeTask";
				break;
			case TaskDesc::RESUME:
				target = "resume";
				break;
			case TaskDesc::PAUSE:
				target = "pause";
				break;
			case TaskDesc::STOP:
				target = "stop";
				break;
			case TaskDesc::REWARD:
				target = "reward:1";	// For now, always reward only once.
				break;
			}
			SendMessage(QString("TASK %1 PICTO/1.0\r\nObserver-ID:%2\r\nSession-ID:%3\r\n\r\n").arg(target).arg(observerID_).arg(session_), tcpSocket_);
			// Check response.
			if(iAmAuthorized_)
				ReadIncomingMessage("PICTO/1.0 200 OK",message,tcpSocket_,10000);
			else
				ReadIncomingMessage("PICTO/1.0 401 Unauthorized",message,tcpSocket_,10000);
		}
		break;
	case ENDSESSION:
		// End the current session and check response.
		SendMessage(QString("ENDSESSION %1 PICTO/1.0\r\nObserver-ID:%2\r\n\r\n").arg(session_).arg(observerID_), tcpSocket_);
		if(iAmAuthorized_)
			ReadIncomingMessage("PICTO/1.0 200 OK",message,tcpSocket_,10000);
		else
			ReadIncomingMessage("PICTO/1.0 401 Unauthorized",message,tcpSocket_,10000);
		break;
	case JOINSESSION:
		{
			// Join the session with the input directorName.  Get its ipaddress from the list I created during the DIRECTORLIST action.
			QSharedPointer<JoinSessionDesc> desc = actionDesc.staticCast<JoinSessionDesc>();
			QString directorName = desc->directorName_;
			QString directorIP;
			session_ = "{00000000-0000-0000-0000-000000000000}";
			if(ipMap_.contains(directorName))
				directorIP = ipMap_[directorName].toString();
			else
				QFAIL( QString("TEST DEFINITION ERROR: Workstation simulator could not find the IP address for %1. \n"\
							   "Make sure that all Directors have sent DIRECTORUPDATE messages and that DIRECTORLIST "\
							   "was called before using STARTSESSION." ).arg(directorName).toAscii());
			QString proxy = "-1";
			SendMessage(QString("JOINSESSION %1/%2 PICTO/1.0\r\nContent-Length:0\r\nObserver-ID:%3\r\nSession-ID:{00000000-0000-0000-0000-000000000000}\r\n\r\n").arg(directorIP).arg(proxy).arg(observerID_), tcpSocket_);
			// Check reply and get my new session ID
			ReadIncomingMessage("PICTO/1.0 200 OK",message,tcpSocket_,10000);
			iAmAuthorized_ = false;
			QStringList bufferList = message.split("<SessionID>");
			if(bufferList.size() > 1)
			{
				session_ = bufferList[1].split("</SessionID>")[0];
			}
		}
		break;
	case GETDATA:
		// Not Yet Implemented
		break;
	}
}
/*! \brief Goes throught the DirectorList reply message and builds up a map of DirectorNames to IPAddress while checking for errors.
 */
void PictoWorkstationSimulator::BuildIPMap(QString message)
{
	ipMap_.clear();

	QStringList splitMessage = message.split("DirectorInstances");
	if(splitMessage.size() < 2)
		QFAIL("Message returned from server after DIRECTORUPDATE call was badly formed");
	QXmlStreamReader xmlReader(QString("<DirectorInstances") + splitMessage[1]);
	QString name, address;
	while(!xmlReader.atEnd())
	{
		xmlReader.readNext();

		if(xmlReader.isStartElement() && xmlReader.name() == "Director")
		{

			while(!xmlReader.atEnd())
			{
				xmlReader.readNext();

				if(xmlReader.name() == "Name" && xmlReader.isStartElement())
				{
					name = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Address" && xmlReader.isStartElement())
				{
					address = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Status" && xmlReader.isStartElement())
				{
					//director.status = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Director" && xmlReader.isEndElement())
				{
					// This is the last element name, if we got here we can insert the name, ip mapping.
					ipMap_.insert(name,QHostAddress(address));
					break;
				}
			}
		}
	}
}

/*! \brief Goes throught the ProxyList reply message and builds up a map of Proxy names to Proxy IDs while checking for errors.
 */
void PictoWorkstationSimulator::BuildProxyMap(QString message)
{
	proxyMap_.clear();

	QStringList splitMessage = message.split("Proxies");
	if(splitMessage.size() < 2)
		QFAIL("Message returned from server after DIRECTORUPDATE call was badly formed");
	QXmlStreamReader xmlReader(QString("<Proxies") + splitMessage[1]);
	QString name, id;
	while(!xmlReader.atEnd())
	{
		xmlReader.readNext();

		if(xmlReader.isStartElement() && xmlReader.name() == "Proxy")
		{

			while(!xmlReader.atEnd())
			{
				xmlReader.readNext();

				if(xmlReader.name() == "Name" && xmlReader.isStartElement())
				{
					name = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Id" && xmlReader.isStartElement())
				{
					id = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Proxy" && xmlReader.isEndElement())
				{
					// This is the last element name, if we got here we can insert the name, id mapping.
					// First lets make sure there only one of each proxy.
					if(proxyMap_.contains(name))
						QFAIL( QString("ProxyMap received at %1 from server contains multiple ids for the same proxy name").arg(GetDeviceName()).toAscii() );
					proxyMap_.insert(name,id);
					break;
				}
			}
		}
	}
}