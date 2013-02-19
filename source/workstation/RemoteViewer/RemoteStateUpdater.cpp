#include <QCoreApplication>

#include "RemoteStateUpdater.h"
#include "../../common/storage/BehavioralDataUnitPackage.h"
#include "../../common/storage/RewardDataUnit.h"
#include "../../common/storage/PropertyDataUnit.h"
#include "../../common/storage/FrameDataUnit.h"
#include "../../common/storage/StateDataUnit.h"
#include "../../common/memleakdetect.h"

using namespace Picto;

RemoteStateUpdater::RemoteStateUpdater(CommandChannel *serverChan)
: serverChan_(serverChan)
{
	Q_ASSERT(serverChan_);
	initForNewSession();
}

void RemoteStateUpdater::initForNewSession()
{
	lastTimeStateDataRequested_ = "0.0";
}

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
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));


	PropertyDataUnit propUnit;
	BehavioralDataUnitPackage behavUnitPack;
	StateDataUnit stateUnit;
	FrameDataUnit frameUnit;
	RewardDataUnit rewardUnit;

	while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "Data");

	if(xmlReader->atEnd())
		return false;

	xmlReader->readNext();
	while(!xmlReader->isEndElement() && xmlReader->name() != "Data" && !xmlReader->atEnd())
	{
		QString currUnitTime = "-1.0";
		if(xmlReader->name() == "PDU")
		{
			propUnit.fromXml(xmlReader);
			if(propUnit.initValue_)
			{
				emit propertyInitValueChanged(propUnit.index_,propUnit.value_);
			}
			else
				emit propertyValueChanged(propUnit.index_,propUnit.value_);
		}
		else if(xmlReader->name() == "BDUP")
		{
			behavUnitPack.fromXml(xmlReader);
			QVector<float> data(2);
			QSharedPointer<BehavioralDataUnit> unit = behavUnitPack.takeLastDataPoint();
			data[0] = unit->x;
			data[1] = unit->y;
			emit signalChanged(behavUnitPack.getChannel()
								,behavUnitPack.getDescriptor().split(",",QString::SkipEmptyParts)
								,data);
		}
		else if(xmlReader->name() == "SDU")
		{
			stateUnit.fromXml(xmlReader);
			emit transitionActivated(stateUnit.getTransitionID());
		}
		else if(xmlReader->name() == "FDU")
		{
			frameUnit.fromXml(xmlReader);
			currUnitTime = frameUnit.time;		//Both frame and rewards (while pause or stopped) can cause state update
			emit framePresented(frameUnit.time.toDouble());
		}
		else if(xmlReader->name() == "RDU")
		{
			rewardUnit.fromXml(xmlReader);
			currUnitTime = rewardUnit.getTime();	//Both frame and rewards (while pause or stopped) can cause state update
			emit rewardSupplied(rewardUnit.getTime().toDouble(),rewardUnit.getDuration(),rewardUnit.getChannel());
		}
		if(currUnitTime.toDouble() > lastTimeStateDataRequested_.toDouble())
			lastTimeStateDataRequested_ = currUnitTime;
		xmlReader->readNext();
	}
	return true;
}