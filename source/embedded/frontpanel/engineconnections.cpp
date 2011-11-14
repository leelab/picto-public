#include <QHostAddress>
#include <QTcpSocket>

#include <QTextStream>

#include "engineconnections.h"
#include "eventchannelthread.h"

EngineConnections::EngineConnections(FrontPanelInfo *panelInfo, QObject* parent) :
	QObject(parent),
	panelInfo(panelInfo)
{
	//Set up the command channel.  This is the connection used to send commands
	//to the engine, and read the responses from the engine
	commandChannel = new QTcpServer(this);
	commandChannel->listen(QHostAddress::LocalHost, LCDCOMMANDPORT);
	connect(commandChannel, SIGNAL(newConnection()), this, SLOT(setupCommandConnection()));

	//Set up the event channel.  This is the connection used to send events from the 
	//engine to the front panel (e.g. trial start, new block, etc)
	eventChannel = new QTcpServer(this);
	eventChannel->listen(QHostAddress::LocalHost, LCDEVENTPORT);
	connect(eventChannel, SIGNAL(newConnection()), this, SLOT(setupEventConnection()));



}
	
EngineConnections::~EngineConnections()
{
	QTcpSocket *commandSocket = panelInfo->getCommandSocket();
	if(commandSocket)
		commandSocket->close();
	panelInfo->setCommandSocket(NULL);

	QTcpSocket *eventSocket = panelInfo->getEventSocket();
	if(eventSocket)
		eventSocket->close();
	panelInfo->setEventSocket(NULL);

	commandSocket->deleteLater();
	eventSocket->deleteLater();
	commandChannel->deleteLater();
	eventChannel->deleteLater();
}


void EngineConnections::setupCommandConnection()
{
	QTcpSocket *clientConnection = commandChannel->nextPendingConnection();
	panelInfo->setCommandSocket(clientConnection);

	QTextStream out(stdout);
	out<<"Command channel connected...\n";

}

void EngineConnections::setupEventConnection()
{
	QTextStream out(stdout);
	out<<"setupEventConnection...\n";
	out.flush();

	QTcpSocket *clientConnection = eventChannel->nextPendingConnection();
	panelInfo->setEventSocket(clientConnection);

	EventChannelThread *eventThread = new EventChannelThread(panelInfo,this);
	connect(eventThread,SIGNAL(newEventRead()), this, SIGNAL(newEventRead()));
	connect(eventThread, SIGNAL(finished()), eventThread, SLOT(deleteLater()));
	eventThread->start();


}