#include <QHostAddress>
#include <QTcpSocket>

#include <QTextStream>

#include "engineconnections.h"
#include "eventchannelthread.h"
#include "../../common/globals.h"
/*! \brief Creates an EngineConnections object which creates a commandChannel and puts it into the input FrontPanelInfo object for communication
 * with the director application.
 */
EngineConnections::EngineConnections(FrontPanelInfo *panelInfo, QObject* parent) :
	QObject(parent),
	panelInfo(panelInfo)
{
	//Set up the command channel.  This is the connection used to send commands
	//to the engine, and read the responses from the engine
	commandChannel = new QTcpServer(this);
	commandChannel->listen(QHostAddress::LocalHost, Picto::portNums->getLCDCommandPort());
	connect(commandChannel, SIGNAL(newConnection()), this, SLOT(setupCommandConnection()));

	//Set up the event channel.  This is the connection used to send events from the 
	//engine to the front panel (e.g. trial start, new block, etc)
	//Joey - This is no longer used, but I have not yet taken it out.  It would probably be a good
	//idea to do so, but this is not the time.
	eventChannel = new QTcpServer(this);
	eventChannel->listen(QHostAddress::LocalHost, Picto::portNums->getLCDEventPort());
	connect(eventChannel, SIGNAL(newConnection()), this, SLOT(setupEventConnection()));



}
	
/*! \brief Destroys the EngineConnections object, closing the FrontPanel->Director command channel in the process.
 */
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

/*! \brief Attaches the commandChannel created by this object to the panelInfo object
 */
void EngineConnections::setupCommandConnection()
{
	QTcpSocket *clientConnection = commandChannel->nextPendingConnection();
	panelInfo->setCommandSocket(clientConnection);

	QTextStream out(stdout);
	out<<"Command channel connected...";

}

/*! \brief We no longer use the objects that are set up in this function. It should probably be deleted
 * sometime soon.
 */
void EngineConnections::setupEventConnection()
{
	QTextStream out(stdout);
	out<<"Command channel connected...";
	out.flush();

	QTcpSocket *clientConnection = eventChannel->nextPendingConnection();
	panelInfo->setEventSocket(clientConnection);

	EventChannelThread *eventThread = new EventChannelThread(panelInfo,this);
	connect(eventThread,SIGNAL(newEventRead()), this, SIGNAL(newEventRead()));
	connect(eventThread, SIGNAL(finished()), eventThread, SLOT(deleteLater()));
	eventThread->start();


}