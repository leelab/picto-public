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
#include "../storage/BehavioralDataUnitPackage.h"
#include "../storage/RewardDataUnit.h"
#include "../storage/PropertyDataUnit.h"
#include "../storage/FrameDataUnit.h"
#include "../storage/FrameDataUnitPackage.h"
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
	currBehavUnitPack_(QSharedPointer<BehavioralDataUnitPackage>(new BehavioralDataUnitPackage())),
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
	connect(target->getVisualTarget().data(), SIGNAL(presented(double)), this, SLOT(firstPhosphorOperations(double)));
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

void PictoEngine::addSignalChannel(QSharedPointer<SignalChannel> channel)
{
	signalChannels_.insert(channel->getName(), channel);
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

	//Rewards are added to the reward controller when this function is called.  They are actually performed just
	//after the following firstPhosphor time.
	rewardController_->addReward(channel,quantity,minRewardPeriod);
	
	//If this is the master and we're stopped, we need to make sure the reward gets sent (usually this happens on
	//new frames being displayed, but here the system just sticks with the splash screen).
	if(dataCommandChannel_.isNull())
		return;

	if(slave_)
		return;

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
		//If we got here, we're not running.  Wait for all rewards to be given, sending them to the server as they go out.
		while(rewardController_->hasPendingRewards())
		{	
			rewardController_->triggerRewards(true);
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
			QCoreApplication::processEvents();
		}
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

QList<QSharedPointer<BehavioralDataUnitPackage>> PictoEngine::getBehavioralDataPackages()
{
	//Note that the call to getDataPackage() clears out any existing values,
	//so it should only be made once per frame on each signalChannel
	QList<QSharedPointer<BehavioralDataUnitPackage>> returnList;
	QSharedPointer<BehavioralDataUnitPackage> pack;
	foreach(QSharedPointer<Picto::SignalChannel> chan,signalChannels_)
	{
		pack = chan->getDataPackage();
		if(pack)
		{
			pack->setActionFrame(getLastFrameId());
			returnList.append(pack);
		}
	}
	return returnList;
}

/*! \brief Sends the latest behavioral data to the server aligned with this frame time
 *
 *	We update the server with all of the useful behavioral data.  This includes 
 *	the time at which the most recent frame was drawn, as well as all the input
 *	coordinates from the subject and all changed properties.  This data is important
 *	so we send it as a registered command, which means that we will get confirmation
 *	when it's written to disk.  It also means we don't have to wait around for a 
 *	response before continuing experimental execution.
 */
void PictoEngine::reportNewFrame(double frameTime,int runningStateId)
{
	//Create a new frame data store
	FrameDataUnitPackage frameData;

	//Behavioral data occured during the course of the
	//previous frame and so its offsettime value is with respect to the 
	//first phosphor time of the previous frame.  The frame is stamped
	//when we call getBehavioralDataPackages, so we need to call it before
	//we call setLastFrame with the new frame id.
	QList<QSharedPointer<BehavioralDataUnitPackage>> behavPackList = getBehavioralDataPackages();

	frameData.addFrame(frameTime,runningStateId);
	setLastFrame(frameData.getLatestFrameId());

	QSharedPointer<CommandChannel> dataChannel = getDataCommandChannel();

	if(dataChannel.isNull())
		return;

	QSharedPointer<PropertyDataUnitPackage> propPack = getChangedPropertyPackage();
	QSharedPointer<StateDataUnitPackage> statePack = getStateDataPackage();

	//send a PUTDATA command to the server with the most recent behavioral data
	QSharedPointer<Picto::ProtocolResponse> dataResponse;
	QString status = "running";
	int engCmd = getEngineCommand();
	switch(engCmd)
	{
	case PlayEngine:
		status = "running";
		break;
	case PauseEngine:
		status = "paused";
		break;
	case StopEngine:
		status = "stopped";
		break;
	}
	QString dataCommandStr = "PUTDATA " + getName() + ":" + status + " PICTO/1.0";
	QSharedPointer<Picto::ProtocolCommand> dataCommand(new Picto::ProtocolCommand(dataCommandStr));

	QByteArray dataXml;
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&dataXml));

	xmlWriter->writeStartElement("Data");

	foreach(QSharedPointer<BehavioralDataUnitPackage> behavPack,behavPackList)
	{
		if(behavPack && behavPack->length())
			behavPack->toXml(xmlWriter);
	}
	if(propPack && propPack->length())
		propPack->toXml(xmlWriter);
	if(statePack && statePack->length())
		statePack->toXml(xmlWriter);
	QList<QSharedPointer<RewardDataUnit>> rewards = getDeliveredRewards();
	foreach(QSharedPointer<RewardDataUnit> reward,rewards)
	{
		reward->toXml(xmlWriter);
	}
	frameData.toXml(xmlWriter);	//Frame data must go last so that server knows when it reads
								//in a frame, that the data it has defines the state that was
								//in place at that frame.
	xmlWriter->writeEndElement();

	dataCommand->setContent(dataXml);
	dataCommand->setFieldValue("Content-Length",QString::number(dataXml.length()));
	QUuid commandUuid = QUuid::createUuid();

	dataChannel->sendRegisteredCommand(dataCommand);

	dataChannel->processResponses(0);
	//The line below is very fast if the connection isn't broken and takes up to 5 ms reconnecting if it is.
	//This shouldn't be a problem since there should be some time to spare in the state's run loop before the
	//next frame.
	dataChannel->assureConnection(5);


}


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
		else if(xmlReader->name() == "BDUP")
		{
			currBehavUnitPack_->fromXml(xmlReader);
			if(currBehavUnitPack_->getChannel() == "Position")
			{
				currBehavUnit_ = currBehavUnitPack_->takeFirstDataPoint();
				getSignalChannel("Position")->insertValue("x",currBehavUnit_->x);
				getSignalChannel("Position")->insertValue("y",currBehavUnit_->y);
			}
			//Currently we don't do anything with any signal channels besides "Position"	
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
	QString result;
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

void PictoEngine::firstPhosphorOperations(double frameTime)
{
	rewardController_->triggerRewards(!(dataCommandChannel_.isNull() || slave_));

	foreach(QSharedPointer<SignalChannel> channel, signalChannels_)
	{
		channel->updateData(frameTime);
	}
}

}; //namespace Engine
}; //namespace Picto
