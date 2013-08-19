/*! * \file PictoEngine.cpp
 * \ingroup picto_engine
 * \brief
 */
#include <QApplication>
#include <QTime>
#include <QtConcurrentRun>
#include <QMutexLocker>

#include "PictoEngine.h"

#include "../controlelements/FrameResolutionTimer.h"
#include "../storage/BehavioralDataUnitPackage.h"
#include "../storage/RewardDataUnit.h"
#include "../storage/PropertyDataUnit.h"
#include "../storage/FrameDataUnit.h"
#include "../storage/FrameDataUnitPackage.h"
#include "../parameter/SignalValueParameter.h"
#include "../memleakdetect.h"


namespace Picto {
	namespace Engine {


PictoEngine::PictoEngine() :
	timingType_(PictoEngineTimingType::precise),
	propTable_(NULL),
	slave_(false),
	userIsOperator_(false),
	syncInitProperties_(true),
	currBehavUnitPack_(QSharedPointer<BehavioralDataUnitPackage>(new BehavioralDataUnitPackage())),
	currBehavUnit_(QSharedPointer<BehavioralDataUnit>(new BehavioralDataUnit())),
	lastFrameId_(-1),
	engineCommand_(StopEngine),
	taskRunStarting_(false),
	taskRunEnding_(false)
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

QList<QSharedPointer<ControlPanelInterface> > PictoEngine::getControlPanels()
{
	return controlPanelIfs_;
}

void PictoEngine::addRenderingTarget(QSharedPointer<RenderingTarget> target)
{	
	renderingTargets_.append(target);
	connect(target->getVisualTarget().data(), SIGNAL(presented(double)), this, SLOT(firstPhosphorOperations(double)));
	connect(target->getVisualTarget().data(), SIGNAL(escapePressed()), this, SIGNAL(escapePressed()));
}

void PictoEngine::addControlPanel(QSharedPointer<ControlPanelInterface> controlPanel)
{	
	controlPanelIfs_.append(controlPanel);
	//Set first time information
	controlPanel->ipAddressChanged(getIpAddress().toString());
	controlPanel->nameChanged(getName());
	for(int i=0;i<rewardDurations_.size();i++)
	{
		controlPanel->rewardDurationChanged(i,rewardDurations_[i]);
	}
	for(int i=0;i<flushDurations_.size();i++)
	{
		controlPanel->flushDurationChanged(i,flushDurations_[i]);
	}

	//Connect all signals! and set data in appropriate places
	connect(controlPanel.data(),SIGNAL(nameChangeRequest(QString)),this,SLOT(setName(QString)));
	connect(controlPanel.data(),SIGNAL(rewardDurationChangeRequest(int,int)),this,SLOT(setRewardDuration(int,int)));
	connect(controlPanel.data(),SIGNAL(flushDurationChangeRequest(int,int)),this,SLOT(setFlushDuration(int,int)));
	connect(controlPanel.data(),SIGNAL(giveRewardRequest(int)),this,SLOT(giveReward(int)));
	connect(controlPanel.data(),SIGNAL(flushRequest(int)),this,SLOT(flushRequest(int)));

}

//bool PictoEngine::hasVisibleRenderingTargets()
//{
//	foreach(QSharedPointer<RenderingTarget> target, renderingTargets_)
//	{
//		if(target->getVisualTarget()->isVisible())
//			return true;
//	}
//	return false;
//}

QSharedPointer<SignalChannel> PictoEngine::getSignalChannel(QString name)
{
	return signalChannels_.value(name.toLower(), QSharedPointer<SignalChannel>());
}

void PictoEngine::addSignalChannel(QSharedPointer<SignalChannel> channel)
{
	signalChannels_.insert(channel->getName().toLower(), channel);
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

void PictoEngine::emptySignalChannels()
{
	foreach(QSharedPointer<SignalChannel> channel, signalChannels_)
	{
		//By using getValues() instead of clearValues(), we make sure that the latest data 
		//from the data input ports is moved into the signal channels before the signal channels
		//data is erased.  Possibly this should happen in clearValues() as well... this should
		//be looked into.
		channel->getValues();
	}
}

void PictoEngine::markTaskRunStart(QString name)
{
	//If there's not data channel, this is a test run.  Treat every run as if its in its own world
	//by restarting the FrameResolutionTimer and Timer stamper timers from scratch.
	if(getDataCommandChannel().isNull())
	{
		Timestamper::reset();
		if(frameTimerFactory_)
			frameTimerFactory_->resetAllTimers();
	}

	//Set up data for task run.
	taskRunName_ = name;
	taskRunUnit_ = QSharedPointer<TaskRunDataUnit>();
	taskRunStarting_ = true;
	
	//If there's are data readers, tell them that we're starting a new run
	if(frameReader_)
	{
		frameReader_->setRunStart();
		rewardReader_->setRunStart();
	}
}

void PictoEngine::markTaskRunStop()
{
	taskRunEnding_ = true;

	//If there's are data readers, tell them that the run is over
	if(frameReader_)
	{
		frameReader_->setRunEnd();
		rewardReader_->setRunEnd();
	}
}


double PictoEngine::generateEvent(unsigned int eventCode)
{
	if(!eventCodeGenerator_.isNull())
		return eventCodeGenerator_->sendEvent(eventCode);
	return 0.0;
}


//! \brief Issues a reward and sends notfication to the server.  quantity is the number of ms to supply reward.  minRewardPeriod is the time between start of one reward and start of the next.
void PictoEngine::giveReward(int channel, int quantity, int minRewardPeriod)
{
	if(rewardController_.isNull())
		return;

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
			rewardController_->triggerRewards(!sessionId_.isNull());
			if(!sessionId_.isNull())
			{	//Only send data to the server if we're in a session
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

void PictoEngine::setOutputSignalValue(QString port, int pinId, QVariant value)
{
	if(!outSigControllers_.contains(port))
		return;
	outSigControllers_[port]->setValue(pinId,value);
}
void PictoEngine::enableOutputSignal(QString port, int pinId,bool enable)
{
	if(!outSigControllers_.contains(port))
		return;
	outSigControllers_[port]->enablePin(enable,pinId);
}


void PictoEngine::addChangedPropertyValue(Property* changedProp)
{
	if(slave_)
		return;
	if(!propPackage_)
	{
		propPackage_ = QSharedPointer<PropertyDataUnitPackage>(new PropertyDataUnitPackage());
	}
	//If the changedProp has no parent, its a UI parameter.  Set the path as such.
	propPackage_->addData(changedProp->getAssetId(),false,changedProp->valToUserString());
}
void PictoEngine::addChangedPropertyInitValue(Property* changedProp)
{
	if(slave_)
		return;
	if(!propPackage_)
	{
		propPackage_ = QSharedPointer<PropertyDataUnitPackage>(new PropertyDataUnitPackage());
	}
	//If the changedProp has no parent, its a UI parameter.  Set the path as such.
	propPackage_->addData(changedProp->getAssetId(),true,changedProp->initValToUserString());
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

	//If this is an experimental run or this is a test run (not playback)
	if(frameTimerFactory_)
	{
		//Tell all timers that work on single frame resolution what the latest frame time is
		frameTimerFactory_->setLastFrameTime(frameTime);
	}
	
	//If there's an active frameReader_ (ie. We're using the TestViewer with Analysis)
	if(frameReader_)
	{
		//Set the latest frame time to the frameReader_
		frameReader_->setLatestFrameTime(frameTime);
		//Set the latest reward data to the rewardReader_
		QList<QSharedPointer<RewardDataUnit>> rewards = getDeliveredRewards();
		foreach(QSharedPointer<RewardDataUnit> reward,rewards)
		{
			rewardReader_->setLatestRewardData(reward->getTime().toDouble(),reward->getDuration());
		}
	}

	if(dataChannel.isNull())
		return;

	QSharedPointer<PropertyDataUnitPackage> propPack = getChangedPropertyPackage();
	QSharedPointer<StateDataUnitPackage> statePack = getStateDataPackage();
	
	if(taskRunStarting_)
		taskRunUnit_->endFrame_ = 0;
	else if(taskRunEnding_)
		taskRunUnit_->endFrame_ = getLastFrameId();

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
	if((taskRunStarting_ || taskRunEnding_) && taskRunUnit_)
	{
		taskRunUnit_->toXml(xmlWriter);
		taskRunStarting_ = false;
		taskRunEnding_ = false;
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

void PictoEngine::setLastFrame(qulonglong frameId)
{
	lastFrameId_ = frameId;
	if(!taskRunUnit_)
	{
		taskRunUnit_ = QSharedPointer<TaskRunDataUnit>(new TaskRunDataUnit(frameId,taskRunName_,""));
	}
}

bool PictoEngine::updateCurrentStateFromServer()
{
	if(!stateUpdater_)
		return false;
	return stateUpdater_->updateState();
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

bool PictoEngine::setFrontPanelCommandChannel(QSharedPointer<CommandChannel> commandChannel)
{
	if(commandChannel.isNull())
		return false;

	fpCommandChannel_ = commandChannel;
	if(commandChannel->getChannelStatus() == CommandChannel::disconnected)
		return false;
	else
		return true;
}
QSharedPointer<CommandChannel> PictoEngine::getFrontPanelCommandChannel()
{
	return fpCommandChannel_;
}

bool PictoEngine::setFrontPanelEventChannel(QSharedPointer<CommandChannel> commandChannel)
{
	if(commandChannel.isNull())
		return false;

	fpEventChannel_ = commandChannel;
	if(commandChannel->getChannelStatus() == CommandChannel::disconnected)
		return false;
	else
		return true;
}
QSharedPointer<CommandChannel> PictoEngine::getFrontPanelEventChannel()
{
	return fpEventChannel_;
}

void PictoEngine::setPropertyTable(QSharedPointer<PropertyTable> propTable)
{
	if(propTable_ == propTable)
		return;
	if(propTable_)
	{
		disconnect(propTable_.data(),SIGNAL(propertyValueChanged(Property*)),this,SLOT(addChangedPropertyValue(Property*)));
		disconnect(propTable_.data(),SIGNAL(propertyInitValueChanged(Property*)),this,SLOT(addChangedPropertyInitValue(Property*)));
	}
	propTable_ = propTable;	
	if(propTable_.isNull())
		return;
	connect(propTable_.data(),SIGNAL(propertyValueChanged(Property*)),this,SLOT(addChangedPropertyValue(Property*)));
	connect(propTable_.data(),SIGNAL(propertyInitValueChanged(Property*)),this,SLOT(addChangedPropertyInitValue(Property*)));
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

//void PictoEngine::setDesignConfig(QSharedPointer<DesignConfig> designConfig)
//{
//	if(designConfig_ == designConfig)
//		return;
//	designConfig_ = designConfig;
//	if(propTable_)
//	{
//		disconnect(propTable_.data(),SIGNAL(propertyValueChanged(Property*)),this,SLOT(addChangedPropertyValue(Property*)));
//		disconnect(propTable_.data(),SIGNAL(propertyInitValueChanged(Property*)),this,SLOT(addChangedPropertyInitValue(Property*)));
//	}
//	propTable_.clear();
//	if(designConfig_.isNull())
//		return;
//	propTable_ = QSharedPointer<PropertyTable>(new PropertyTable(getDesignConfig()));
//	connect(propTable_.data(),SIGNAL(propertyValueChanged(Property*)),this,SLOT(addChangedPropertyValue(Property*)));
//	connect(propTable_.data(),SIGNAL(propertyInitValueChanged(Property*)),this,SLOT(addChangedPropertyInitValue(Property*)));
//}

void PictoEngine::stop()
{ 
	engineCommand_ = StopEngine; 
	stopAllSignalChannels();
}

void PictoEngine::setName(QString name) 
{
	name_ = name;
	foreach(QSharedPointer<ControlPanelInterface> cp,controlPanelIfs_)
	{
		cp->nameChanged(name);
	}
	emit nameChanged(name);
}

void PictoEngine::setRewardDuration(int controller, int duration)
{
	while(rewardDurations_.size() <= controller)
		rewardDurations_.append(-1);
	rewardDurations_[controller] = duration;
	foreach(QSharedPointer<ControlPanelInterface> cp,controlPanelIfs_)
	{
		cp->rewardDurationChanged(controller,duration);
	}
	emit rewardDurationChanged(controller,duration);
}

void PictoEngine::setFlushDuration(int controller, int duration)
{
	while(flushDurations_.size() <= controller)
		flushDurations_.append(-1);
	flushDurations_[controller] = duration;
	foreach(QSharedPointer<ControlPanelInterface> cp,controlPanelIfs_)
	{
		cp->flushDurationChanged(controller,duration);
	}
	emit flushDurationChanged(controller,duration);
}

void PictoEngine::giveReward(int channel)
{
	if(rewardDurations_.size() <= channel)
		return;
	giveReward(channel,rewardDurations_[channel],0);
}

void PictoEngine::flushRequest(int channel)
{
	if(flushDurations_.size() <= channel)
		return;
	if(rewardController_.isNull())
		return;
	rewardController_->flush(channel,flushDurations_[channel]);
	if(rewardController_->isFlushing(channel))
	{
		rewardController_->abortFlush(channel);
	}
	else
	{
		rewardController_->flush(channel,flushDurations_[channel]);

		QString status;
		int engCmd = getEngineCommand();
		if(engCmd != PlayEngine)
		{	//The experiment is not currently running.  
			//We should trigger flush/rewards now rather than waiting for the system to trigger them.
			//All rewards must be reported to the server
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
				rewardController_->triggerRewards(false);
				if(dataCommandChannel_->getSessionId() != QUuid())
				{	//Only send data to the server if we're in a session
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
				QCoreApplication::processEvents();
			}
		}		
	}
}

QHostAddress PictoEngine::getIpAddress()
{
	if(!ipAddress_.isNull())
		return ipAddress_;
	
	//Use the first IPv4 address that isn't localhost and is on an interface that is up and running
	//This will probably be a valid ip address, but there could still be issues...
	QList<QNetworkInterface> networkInterfaces = QNetworkInterface::allInterfaces();
	foreach(QNetworkInterface inter, networkInterfaces)
	{
		QList<QNetworkAddressEntry> hostAddresses = inter.addressEntries();
		if( !(inter.flags() & (QNetworkInterface::IsUp | QNetworkInterface::IsRunning)) )
			continue;
		foreach(QNetworkAddressEntry addrEntry, hostAddresses)
		{
			QHostAddress addr = addrEntry.ip();
			if(addr.protocol() == QAbstractSocket::IPv4Protocol 
				&& addr != QHostAddress::LocalHost)
			{
				ipAddress_ = addr;
				return ipAddress_;
			}
		}
	}
	return QHostAddress();
}

int PictoEngine::getEngineCommand()
{
	return engineCommand_;
}

void PictoEngine::firstPhosphorOperations(double frameTime)
{
	foreach(QSharedPointer<OutputSignalController> sigCont,outSigControllers_)
	{
		sigCont->updateValues();
	}

	//Below, we enable the reward controller's reward list if there is a reward reader, but if not, then we only enable it if
	//this is the director.  This is so that in the case of the director rewards can get sent to the server and in the case
	//of Testing analysis code (rewardReader).  Triggered rewards can get read by the analysis code.
	if(!rewardController_.isNull())
		rewardController_->triggerRewards(rewardReader_ || !(dataCommandChannel_.isNull() || sessionId_.isNull() || slave_));

	foreach(QSharedPointer<SignalChannel> channel, signalChannels_)
	{
		channel->updateData(frameTime);
	}
	
	foreach(QSharedPointer<ControlPanelInterface> cp, controlPanelIfs_)
	{
		cp->doIncomingCommands();
	}

	foreach(QSharedPointer<SignalChannel> channel, signalChannels_)
	{
		foreach(QString subChan,channel->getSubchannels())
		{
			SignalValueParameter::setLatestValue(channel->getName(),subChan,channel->peekValue(subChan));
		}
	}
}


}; //namespace Engine
}; //namespace Picto
