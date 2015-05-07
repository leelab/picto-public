#include <QCoreApplication>
#include <QDebug>

#include "RemoteStateUpdater.h"
#include "RemoteStateXMLHandler.h"

#include "../../common/memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new RemoteStateUpdater object.
 *	@param serverChan The CommandChannel on which this object will request the latest data from the Server.
 */
RemoteStateUpdater::RemoteStateUpdater(CommandChannel *serverChan)
: serverChan_(serverChan)
{
	Q_ASSERT(serverChan_);
	initForNewSession();

	xmlHandlers_[PropertyDataUnitHandler::getDataPackageName()] = QSharedPointer<RemoteStateXMLHandler>(new PropertyDataUnitHandler(this));
	xmlHandlers_[BehavioralDataUnitHandler::getDataPackageName()] = QSharedPointer<RemoteStateXMLHandler>(new BehavioralDataUnitHandler(this));
	xmlHandlers_[InputDataUnitHandler::getDataPackageName()] = QSharedPointer<RemoteStateXMLHandler>(new InputDataUnitHandler(this));
	xmlHandlers_[StateDataUnitHandler::getDataPackageName()] = QSharedPointer<RemoteStateXMLHandler>(new StateDataUnitHandler(this));
	xmlHandlers_[FrameDataUnitHandler::getDataPackageName()] = QSharedPointer<RemoteStateXMLHandler>(new FrameDataUnitHandler(this));
	xmlHandlers_[RewardDataUnitHandler::getDataPackageName()] = QSharedPointer<RemoteStateXMLHandler>(new RewardDataUnitHandler(this));
}

/*! \brief Initializes this object for a new Session.
 *	\details This means setting the last data requested time back to zero.
 */
void RemoteStateUpdater::initForNewSession()
{
	lastTimeStateDataRequested_ = "0.0";
}

/*! \brief Requests all new Session data from the Picto Server and emits the appropriate signals accordingly.
 *	\details This function keeps track of the latest time for which data has arrived from the Server then requests all
 *	data from the Server that occured after that time.  The function goes through the returned data and emits the
 *	appropriate StateUpdater signals for each type of new data that arrives.  This function should be called at least
 *	once per frame in order to be sure that the most up to date data is always coming into the Workstation.
 */
bool RemoteStateUpdater::updateState()
{
	//Clear any old responses out of the response queue
	while(serverChan_->incomingResponsesWaiting())
		serverChan_->getResponse();
	if(!serverChan_->assureConnection(100))
		return false;

	//Collect the data from the server
	QString commandStr = QString("GETDATA CurrentState:%1 PICTO/1.0").arg(lastTimeStateDataRequested_);
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(commandStr));
	QSharedPointer<Picto::ProtocolResponse> response;

	serverChan_->sendRegisteredCommand(command);
	QString commandID = command->getFieldValue("Command-ID");
	//qDebug(QString("Sent command: %1 at Time:%2").arg(commandID).arg(command->getFieldValue("Time-Sent")).toLatin1());

	do
	{
		QCoreApplication::processEvents();
		if(!serverChan_->waitForResponse(1000))
		{
			return false;
		}
		response = serverChan_->getResponse();
	}while(!response || response->getFieldValue("Command-ID") != commandID);

	//Response not 200:OK
	if(response->getResponseCode() != Picto::ProtocolResponseType::OK)
		return false;
	
	QByteArray xmlFragment = response->getContent();
	qDebug() << "New Server Data:" << xmlFragment;
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));

	while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "Data");

	if(xmlReader->atEnd())
		return false;

	xmlReader->readNext();
	while(!xmlReader->isEndElement() && xmlReader->name() != "Data" && !xmlReader->atEnd())
	{
		QString name = xmlReader->name().toString();
		if (xmlHandlers_.contains(name))
		{
			xmlHandlers_[name]->handle(xmlReader);
		}
		else
		{
			qDebug() << "Unknown Data Packet of Name: " << name;
		}
		xmlReader->readNext();
	}

	return true;
}

void RemoteStateUpdater::updateCurrUnitTime(QString time)
{
	if (time.toDouble() > lastTimeStateDataRequested_.toDouble())
	{
		lastTimeStateDataRequested_ = time;
	}
}