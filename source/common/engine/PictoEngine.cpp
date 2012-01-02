/*! * \file PictoEngine.cpp
 * \ingroup picto_engine
 * \brief
 */
#include <QApplication>
#include <QTime>
#include <QtConcurrentRun>
#include <QMutexLocker>

#include "PictoEngine.h"

#include "../timing/Timestamper.h"
#include "../storage/RewardDataUnit.h"
#include "../storage/PropertyDataUnit.h"
#include "../storage/FrameDataUnit.h"
#include "../memleakdetect.h"


namespace Picto {
	namespace Engine {


PictoEngine::PictoEngine() :
	timingType_(PictoEngineTimingType::precise),
	propTable_(NULL),
	slave_(false),
	userIsOperator_(false),
	lastTimePropChangesRequested_("0.0"),
	lastTimeStateDataRequested_("0.0"),
	firstCurrStateUpdate_(true),
	currBehavUnit_(QSharedPointer<BehavioralDataUnit>(new BehavioralDataUnit())),
	runningPath_(""),
	lastFrameId_(-1),
	engineCommand_(StopEngine)
{
	bExclusiveMode_ = true;
	setSessionId(QUuid());
}

void PictoEngine::setTimingControl(PictoEngineTimingType::PictoEngineTimingType _timingType)
{
	timingType_ = _timingType;
}

QList<QSharedPointer<RenderingTarget> > PictoEngine::getRenderingTargets()
{
	return renderingTargets_;
}

void PictoEngine::addRenderingTarget(QSharedPointer<RenderingTarget> target)
{	
	renderingTargets_.append(target);
}

bool PictoEngine::hasVisibleRenderingTargets()
{
	foreach(QSharedPointer<RenderingTarget> target, renderingTargets_)
	{
		if(target->getVisualTarget()->isVisible())
			return true;
	}
	return false;
}

QSharedPointer<SignalChannel> PictoEngine::getSignalChannel(QString name)
{
	return signalChannels_.value(name, QSharedPointer<SignalChannel>());
}

void PictoEngine::addSignalChannel(QString name, QSharedPointer<SignalChannel> channel)
{
	signalChannels_.insert(name, channel);
}

//! \brief Starts every signal channel in the engine
void PictoEngine::startAllSignalChannels()
{
	foreach(QSharedPointer<SignalChannel> channel, signalChannels_)
	{
		channel->start();
	}
}

//! \brief Stops every signal channel in the engine
void PictoEngine::stopAllSignalChannels()
{
	foreach(QSharedPointer<SignalChannel> channel, signalChannels_)
	{
		channel->stop();
	}
}

void PictoEngine::generateEvent(unsigned int eventCode)
{
	if(!eventCodeGenerator_.isNull())
		eventCodeGenerator_->sendEvent(eventCode);
}


//! \brief Issues a reward and sends notfication to the server.  quantity is the number of ms to supply reward.  minRewardPeriod is the time between start of one reward and start of the next.
void PictoEngine::giveReward(int channel, int quantity, int minRewardPeriod)
{
	if(rewardController_.isNull())
		return;

	Timestamper stamper;
	//double timestamp = stamper.stampSec();
	//double lastMsgTime = 0;

	//Since we don't want the server to timeout the director, we make sure that we
	//update the server at least once per second by running th giveReward function
	//in as separate thread.
	QFuture<void> future = QtConcurrent::run(rewardController_.data(),&RewardController::giveReward,channel,quantity,minRewardPeriod,!(dataCommandChannel_.isNull() || slave_));
	//while(!future.isFinished())
	//{
	//	QCoreApplication::processEvents();
	//	if(stamper.stampSec() > (lastMsgTime+1))
	//	{
	//		if((!dataCommandChannel_.isNull()) && (!slave_))
	//		{
	//			QString status = "running";
	//			int engCmd = getEngineCommand();
	//			switch(engCmd)
	//			{
	//			case Engine::PictoEngine::PlayEngine:
	//				status = "running";
	//				break;
	//			case Engine::PictoEngine::PauseEngine:
	//				status = "paused";
	//				break;
	//			case Engine::PictoEngine::StopEngine:
	//				status = "stopped";
	//				break;
	//			}
	//			QString updateCommandStr = "COMPONENTUPDATE "+getName()+":"+status+" PICTO/1.0";
	//			QSharedPointer<Picto::ProtocolCommand> updateCommand(new Picto::ProtocolCommand(updateCommandStr));
	//			dataCommandChannel_->sendCommand(updateCommand);
	//		}
	//		lastMsgTime = stamper.stampSec();
	//	}
	//}
	if(dataCommandChannel_.isNull())
		return;

	if(slave_)
		return;

	//appendDeliveredRewards(QSharedPointer<RewardDataUnit>(new RewardDataUnit(quantity,channel,timestamp)));

	QString status;
	int engCmd = getEngineCommand();
	if(engCmd != PlayEngine)
	{	//The experiment is not currently running.  We are responsible for
		//sending reward updates.
		switch(engCmd)
		{
		case Engine::PictoEngine::PlayEngine:
			return;
		case Engine::PictoEngine::PauseEngine:
			status = "paused";
			break;
		case Engine::PictoEngine::StopEngine:
			status = "stopped";
			break;
		default:
			return;
		}
		//If we got here, we're not running.  Wait for reward to be given, then send reward data to server.
		while(!future.isFinished())
			QCoreApplication::processEvents();
		QString dataCommandStr = "PUTDATA " + getName() + ":" + status + " PICTO/1.0";
		QSharedPointer<Picto::ProtocolCommand> dataCommand(new Picto::ProtocolCommand(dataCommandStr));

		QByteArray dataXml;
		QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&dataXml));

		xmlWriter->writeStartElement("Data");
		QList<QSharedPointer<RewardDataUnit>> rewards = getDeliveredRewards();
		foreach(QSharedPointer<RewardDataUnit> reward,rewards)
		{
			reward->toXml(xmlWriter);
		}
		xmlWriter->writeEndElement();

		dataCommand->setContent(dataXml);
		dataCommand->setFieldValue("Content-Length",QString::number(dataXml.length()));
		QUuid commandUuid = QUuid::createUuid();

		dataCommandChannel_->sendRegisteredCommand(dataCommand);
		dataCommandChannel_->processResponses(0);
	}


}

QList<QSharedPointer<RewardDataUnit>> PictoEngine::getDeliveredRewards()
{
	if(rewardController_.isNull())
		return QList<QSharedPointer<RewardDataUnit>>();
	return rewardController_->getDeliveredRewards();
}


void PictoEngine::addChangedProperty(QSharedPointer<Property> changedProp)
{
	if(slave_)
		return;
	if(!propPackage_)
		propPackage_ = QSharedPointer<PropertyDataUnitPackage>(new PropertyDataUnitPackage());
	Timestamper stamper;
	//If the changedProp has no parent, its a UI parameter.  Set the path as such.
	propPackage_->addData(changedProp->getAssetId(),changedProp->toUserString());
}
//! \brief Retrieves the latest package of changed properties.
//! Note that a package can only be retrieved once after which a new package is created.
QSharedPointer<PropertyDataUnitPackage> PictoEngine::getChangedPropertyPackage()
{
	QSharedPointer<PropertyDataUnitPackage> returnVal = propPackage_;

	//Remove these properties from the engine and let the caller deal with them
	propPackage_.clear();

	//Reset the timestamps of these properties to the latest first phosphor value
	if(returnVal)
		returnVal->setActionFrame(getLastFrameId());

	//Return the list of properties
	return returnVal;
}

void PictoEngine::addStateTransitionForServer(QSharedPointer<Transition> stateTrans)
{
	if(!stateDataPackage_)
		stateDataPackage_ = QSharedPointer<StateDataUnitPackage>(new StateDataUnitPackage());
	stateDataPackage_->addTransition(stateTrans);
}

QSharedPointer<StateDataUnitPackage> PictoEngine::getStateDataPackage()
{
	QSharedPointer<StateDataUnitPackage> returnVal = stateDataPackage_;
	
	//Remove these StateDataUnits from the engine and let the caller deal with them
	stateDataPackage_.clear();

	//Reset the timestamps of these StateDataUnits to the latest first phosphor value
	if(returnVal)
		returnVal->setActionFrame(getLastFrameId());

	return returnVal;
}

////! \brief Gets the latest property changes from the server and applies them to the local properties.
//void PictoEngine::updatePropertiesFromServer()
//{
//	return;
//	if(propTable_.isNull())
//		return;
//	//Collect the data from the server
//	QString commandStr = QString("GETDATA PropertyDataUnitPackage:%1 PICTO/1.0").arg(lastTimePropChangesRequested_);
//	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(commandStr));
//	QSharedPointer<Picto::ProtocolResponse> response;
//
//	slaveCommandChannel_->sendCommand(command);
//	//No response
//	if(!slaveCommandChannel_->waitForResponse(1000))
//		return;
//
//	response = slaveCommandChannel_->getResponse();
//
//	//Response not 200:OK
//	if(response->getResponseCode() != Picto::ProtocolResponseType::OK)
//		return;
//	
//	QByteArray xmlFragment = response->getContent();
//	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));
//
//	while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "Data");
//
//	if(xmlReader->atEnd())
//		return;
//
//	PropertyDataUnitPackage propData;
//
//	xmlReader->readNext();
//	while(!xmlReader->isEndElement() && xmlReader->name() != "Data" && !xmlReader->atEnd())
//	{
//		if(xmlReader->name() == "PropertyDataUnitPackage")
//		{
//			propData.fromXml(xmlReader);
//		}
//		else
//		{
//			return;
//		}
//	}
//
//	while(propData.length() > 0)
//	{
//		// Push the data into our signal channel
//		QSharedPointer<Picto::PropertyDataUnit> dataPoint;
//		dataPoint = propData.takeFirstDataPoint();
//
//		propTable_->updatePropertyValue(dataPoint->index_,dataPoint->value_);
//		//qDebug(QString("Received Prop: %1\nval:\n%2\n\n").arg(dataPoint->path_,dataPoint->value_).toAscii());
//		lastTimePropChangesRequested_ = dataPoint->time_;
//	}
//}


bool PictoEngine::updateCurrentStateFromServer()
{
	//Clear any old responses out of the response queue
	while(slaveCommandChannel_->incomingResponsesWaiting())
		slaveCommandChannel_->getResponse();
	if(!slaveCommandChannel_->assureConnection(100))
		return false;

	//Collect the data from the server
	QString commandStr = QString("GETDATA CurrentState:%1 PICTO/1.0").arg(lastTimeStateDataRequested_);
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(commandStr));
	QSharedPointer<Picto::ProtocolResponse> response;

	slaveCommandChannel_->sendRegisteredCommand(command);
	QString commandID = command->getFieldValue("Command-ID");
	//qDebug(QString("Sent command: %1 at Time:%2").arg(commandID).arg(command->getFieldValue("Time-Sent")).toAscii());

	do
	{
		QCoreApplication::processEvents();
		if(!slaveCommandChannel_->waitForResponse(1000))
		{
			return false;
		}
		response = slaveCommandChannel_->getResponse();
		//if(response)
		//	qDebug(QString("Received command: %1 sent by server at Time:%2").arg(response->getFieldValue("Command-ID")).arg(response->getFieldValue("Time-Sent")).toAscii());
	}while(!response || response->getFieldValue("Command-ID") != commandID);

	////No response
	//if(!slaveCommandChannel_->waitForResponse(1000))
	//	return false;

	//response = slaveCommandChannel_->getResponse();

	//Response not 200:OK
	if(response->getResponseCode() != Picto::ProtocolResponseType::OK)
		return false;
	
	QByteArray xmlFragment = response->getContent();
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));

	while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "Data");

	if(xmlReader->atEnd())
		return false;

	PropertyDataUnitPackage propData;

	xmlReader->readNext();
	while(!xmlReader->isEndElement() && xmlReader->name() != "Data" && !xmlReader->atEnd())
	{
		QString currUnitTime = "-1.0";
		if(xmlReader->name() == "PDU")
		{
			PropertyDataUnit unit;
			unit.fromXml(xmlReader);
			propData.addData(unit.index_,unit.value_);
			//currUnitTime = unit.time_;
		}
		else if(xmlReader->name() == "BDU")
		{
			currBehavUnit_->fromXml(xmlReader);
			getSignalChannel("PositionChannel")->insertValue("xpos",currBehavUnit_->x);
			getSignalChannel("PositionChannel")->insertValue("ypos",currBehavUnit_->y);
			getSignalChannel("PositionChannel")->insertValue("time",currBehavUnit_->t.toDouble());
			//currUnitTime = currBehavUnit_->t;
		}
		else if(xmlReader->name() == "SDU")
		{
			if(!currStateUnit_)
				currStateUnit_ = QSharedPointer<StateDataUnit>(new StateDataUnit());
			currStateUnit_->fromXml(xmlReader);
			//currUnitTime = currStateUnit_->getTime();
		}
		else if(xmlReader->name() == "FDU")
		{
			FrameDataUnit unit;
			unit.fromXml(xmlReader);
			currUnitTime = unit.time;		//Both frame and rewards (while pause or stopped) can cause state update
		}
		else if(xmlReader->name() == "RDU")
		{
			RewardDataUnit unit;
			unit.fromXml(xmlReader);
			currUnitTime = unit.getTime();	//Both frame and rewards (while pause or stopped) can cause state update
			if(!firstCurrStateUpdate_)	
				giveReward(unit.getChannel(),unit.getDuration(),0);
		}
		if(currUnitTime.toDouble() > lastTimeStateDataRequested_.toDouble())
			lastTimeStateDataRequested_ = currUnitTime;
		xmlReader->readNext();
	}

	while(propData.length() > 0)
	{
		// Push the data into our signal channel
		QSharedPointer<Picto::PropertyDataUnit> dataPoint;
		dataPoint = propData.takeFirstDataPoint();

		propTable_->updatePropertyValue(dataPoint->index_,dataPoint->value_);
		//qDebug(QString("Received Prop: %1\nval:\n%2\n\n").arg(dataPoint->path_,dataPoint->value_).toAscii());
	}
	firstCurrStateUpdate_ = false;
	return true;
}

void PictoEngine::setRunningPath(QString path)
{
	runningPath_ = path;
}

QString PictoEngine::getServerPathUpdate()
{
	if(!currStateUnit_)
		return "";
	QSharedPointer<Asset> asset = expConfig_->getAsset(currStateUnit_->getTransitionID());
	QSharedPointer<Transition> trans;
	QString result = "";
	if(asset)
	{
		Q_ASSERT(asset->inherits("Picto::Transition"));
		trans = asset.staticCast<Transition>();
		result = trans->getDestination();
	}
	else
	{
		//qDebug("PathUpdate: NO ASSET");
		return "";
	}
	QString runningPath = runningPath_;
	if(result == "NULL")
		result = "";
	if(result != "EngineAbort")
	{
		if(trans)
			result.prepend(trans->getPath());
		setRunningPath(result);
	}
	if(result == runningPath)
		result = "";
	//qDebug("PathUpdate: " + result.toAscii());
	return result;
}
QSharedPointer<Picto::BehavioralDataUnit> PictoEngine::getCurrentBehavioralData()
{
	return currBehavUnit_;
}

//! Sets the CommandChannel used for data.  Returns true if the channel's status is connected
bool PictoEngine::setDataCommandChannel(QSharedPointer<CommandChannel> commandChannel)
{
	if(commandChannel.isNull())
		return false;

	dataCommandChannel_ = commandChannel;
	if(commandChannel->getChannelStatus() == CommandChannel::disconnected)
		return false;
	else
		return true;
}

QSharedPointer<CommandChannel> PictoEngine::getDataCommandChannel()
{
	return dataCommandChannel_;
}

//! Sets the CommandChannel used for updates.  Returns true if the channel's status is connected
bool PictoEngine::setUpdateCommandChannel(QSharedPointer<CommandChannel> commandChannel)
{
	if(commandChannel.isNull())
		return false;

	updateCommandChannel_ = commandChannel;
	if(commandChannel->getChannelStatus() == CommandChannel::disconnected)
		return false;
	else
		return true;
}

QSharedPointer<CommandChannel> PictoEngine::getUpdateCommandChannel()
{
	return updateCommandChannel_;
}

void PictoEngine::setPropertyTable(QSharedPointer<PropertyTable> propTable)
{
	if(propTable_ == propTable)
		return;
	if(propTable_)
		disconnect(propTable_.data(),SIGNAL(propertyChanged(QSharedPointer<Property>)),this,SLOT(addChangedProperty(QSharedPointer<Property>)));
	propTable_ = propTable;	
	connect(propTable_.data(),SIGNAL(propertyChanged(QSharedPointer<Property>)),this,SLOT(addChangedProperty(QSharedPointer<Property>)));
}

void PictoEngine::sendAllPropertyValuesToServer()
{
	Q_ASSERT(propTable_);
	propTable_->reportChangeInAllProperties();
}

void PictoEngine::setSessionId(QUuid sessionId)
{
	sessionId_ = sessionId;
	if(!dataCommandChannel_.isNull())
		dataCommandChannel_->setSessionId(sessionId_);
	if(!updateCommandChannel_.isNull())
		updateCommandChannel_->setSessionId(sessionId_);
}

int PictoEngine::getEngineCommand()
{
	return engineCommand_;
}
 
void PictoEngine::stop()
{ 
	engineCommand_ = StopEngine; 
	stopAllSignalChannels();
}

}; //namespace Engine
}; //namespace Picto
