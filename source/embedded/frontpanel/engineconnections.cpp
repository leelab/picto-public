#include <QHostAddress>
#include <QTcpSocket>

#include <QTextStream>

#include "engineconnections.h"
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
