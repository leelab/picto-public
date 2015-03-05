/*! * \file PictoEngine.cpp
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
	propTable_(NULL),
	slave_(false),
	userIsOperator_(false),
	lastFrameId_(-1),
	engineCommand_(StopEngine),
	taskRunStarting_(false),
	taskRunEnding_(false)
{
	bExclusiveMode_ = true;
	setSessionId(QUuid());
}

/*! \brief Returns a list of RenderingTargets to which the running Experiment is rendered.*/
QList<QSharedPointer<RenderingTarget> > PictoEngine::getRenderingTargets()
{
	return renderingTargets_;
}

/*! \brief Returns a list of ControlPanelInterfaces with which this Engine is interfacing.
 *	\details Currently, the Pictobox Front Panel interface FPInterface, is the only one used.
 */
QList<QSharedPointer<ControlPanelInterface> > PictoEngine::getControlPanels()
{
	return controlPanelIfs_;
}

/*! \brief Adds a RenderingTarget to which Experiments should be rendered.*/
void PictoEngine::addRenderingTarget(QSharedPointer<RenderingTarget> target)
{	
	renderingTargets_.append(target);
	connect(target->getVisualTarget().data(), SIGNAL(presented(double)), this, SLOT(firstPhosphorOperations(double)));
	connect(target->getVisualTarget().data(), SIGNAL(escapePressed()), this, SIGNAL(escapePressed()));
}

/*! \brief Adds a ControlPanelInterfaces with which this Engine should interface.
 *	\details Currently, the Pictobox Front Panel interface FPInterface, is the only one that we are using.
 */
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

/*! \brief Returns the SignalChannel that was added to this PictoEngine with the input name.
*/
QSharedPointer<SignalChannel> PictoEngine::getSignalChannel(QString name)
{
	return signalChannels_.value(name.toLower(), QSharedPointer<SignalChannel>());
}

/*! \brief Adds a SignalChannel to this PictoEngine so that it will be accessible to experimental elements.
 *	\details For example, when the State gets the current eye position, it gets it from the "position" signal stored int the
 *	engine.
*/
void PictoEngine::addSignalChannel(QSharedPointer<SignalChannel> channel)
{
	signalChannels_.insert(channel->getName().toLower(), channel);
}

/*! \brief Starts every signal channel in the engine
 *	\details This is done when starting a Task.
 *	\sa Experiment::runTask()
 */
void PictoEngine::startAllSignalChannels()
{
	foreach(QSharedPointer<SignalChannel> channel, signalChannels_)
	{
		channel->start();
	}
}

/*! \brief Stops every signal channel in the engine
 *	\details This is done when a Task ends.
 *	\sa Experiment::runTask()
 */
void PictoEngine::stopAllSignalChannels()
{
	foreach(QSharedPointer<SignalChannel> channel, signalChannels_)
	{
		channel->stop();
	}
}

/*! \brief Empties all of the latest data out of all SignalChannels that have been added to this PictoEngine.*/
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

/*! \brief Marks when a task with the input name starts.  Initializes various objects and data fields for the new run.
*/
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
		lfpReader_->setRunStart();
		rewardReader_->setRunStart();
		spikeReader_->setRunStart();
		foreach(QSharedPointer<LiveSignalReader> signalReader,signalReaders_)
		{
			signalReader->setRunStart();
		}
	}
}

/*! \brief Marks when a task with the input name stops.  Deinitializes various objects and data fields for the end of the run.
*/
void PictoEngine::markTaskRunStop()
{
	taskRunEnding_ = true;

	//If there's are data readers, tell them that the run is over
	if(frameReader_)
	{
		frameReader_->setRunEnd();
		lfpReader_->setRunEnd();
		rewardReader_->setRunEnd();
		spikeReader_->setRunEnd();
		foreach(QSharedPointer<LiveSignalReader> signalReader,signalReaders_)
		{
			signalReader->setRunEnd();
		}
	}
}

/*! \brief Sends an alignment code over the EventCodeGenerator set in setEventCodeGenerator and returns the time in seconds
 *	before the function returned that the event code was triggered.  
 *	\details The idea here is that in general, most EventCodeGenerators are going to need to hold a signal
 *	high for some number of microseconds before returning.  In order to get an
 *	accurage timestamp, we will record the time when this function returns minus
 *	the value that it returns.  We can't simply use the time at which the function
 *	is called because setup times may vary and are typically going to be harder to
 *	estimate.
 *	\sa setEventCodeGenerator()
 */
double PictoEngine::generateEvent(unsigned int eventCode)
{
	if(!eventCodeGenerator_.isNull())
		return eventCodeGenerator_->sendEvent(eventCode);
	return 0.0;
}


/*! \brief Issues a reward and sends notfication of such to the server.  
 *	\details quantity is the number of ms to supply reward.  
 *	minRewardPeriod is the time between start of one reward and start of the next.
 */
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
/*! \brief Returns a list of all rewards supplied since the last time this function was called.*/
QList<QSharedPointer<RewardDataUnit>> PictoEngine::getDeliveredRewards()
{
	if(rewardController_.isNull())
		return QList<QSharedPointer<RewardDataUnit>>();
	return rewardController_->getDeliveredRewards();
}

/*! \brief Sets the output signal value on pinid of port to value.
 *	\sa setOutputSignalController()
 */
void PictoEngine::setOutputSignalValue(QString port, int pinId, QVariant value)
{
	if(!outSigControllers_.contains(port))
		return;
	outSigControllers_[port]->setValue(pinId,value);
}
/*! \brief Enables/disables the signal on pinId of port.
 *	\details If disabled, the pin voltage goes to zero.  Otherwise, it goes to whatever value
 *	was last set on setOutputSignalValue()
 */
void PictoEngine::enableOutputSignal(QString port, int pinId,bool enable)
{
	if(!outSigControllers_.contains(port))
		return;
	outSigControllers_[port]->enablePin(enable,pinId);
}

/*! \brief Informs this engine of a Property whose value changed so that it can be included in the PropertyDataUnitPackage returned from getChangedPropertyPackage().
 */
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
/*! \brief Informs this engine of a Property whose init value changed so that it can be included in the PropertyDataUnitPackage returned from getChangedPropertyPackage().
 */
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

/*! \brief Retrieves the latest package of changed properties.
 *	\details When this function is called, the stored PropertyDataUnitPackage pointer is cleared such that each PropertyDataUnitPackage can
 *	only be retrieved once and includes data regarding Properties whose values changed since the last time the function was called.
 */
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

/*! \brief Adds the input Transition to a stored. StateDataUnitPackage.  This package is transfered to the Server as part of reportNewFrame() 
 *	to record the control flow history of the running experiment.
 */
void PictoEngine::addStateTransitionForServer(QSharedPointer<Transition> stateTrans)
{
	if(!stateDataPackage_)
		stateDataPackage_ = QSharedPointer<StateDataUnitPackage>(new StateDataUnitPackage());
	stateDataPackage_->addTransition(stateTrans);
}

/*! \brief Retrieves the latest StateDataUnitPackage of control flow history.
 *	\details When this function is called, the stored StateDataUnitPackage pointer is cleared such that each StateDataUnitPackage can
 *	only be retrieved once and includes data regarding transitions since the last time the function was called.
 */
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

/*! \brief Retrieves the latest BehavioralDataUnitPackages for each SignalChannel.
 *	\details When this function is called, stored signal data is cleared such that each BehavioralDataUnitPackage list can
 *	only be retrieved once and includes data regarding signal values since the last time the function was called.
 */

QList<QSharedPointer<BehavioralDataUnitPackage>> PictoEngine::getBehavioralDataPackages()
{
	//Note that the call to getDataPackage() clears out any existing values,
	//so it should only be made once per frame on each signalChannel
	QList<QSharedPointer<BehavioralDataUnitPackage>> returnList;
	QSharedPointer<BehavioralDataUnitPackage> pack;
	QSharedPointer<DoubletSignalChannel> chan = getSignalChannel("Position").dynamicCast<DoubletSignalChannel>();
	if (chan)
	{
		pack = chan->getDataPackage();
		if (pack)
		{
			pack->setActionFrame(getLastFrameId());
			returnList.append(pack);
		}
	}

	chan = getSignalChannel("Diameter").dynamicCast<DoubletSignalChannel>();
	if (chan)
	{
		pack = chan->getDataPackage();
		if (pack)
		{
			pack->setActionFrame(getLastFrameId());
			returnList.append(pack);
		}
	}
	return returnList;
}

/*! \brief Retrieves the latest BehavioralDataUnitPackages for each SignalChannel.
*	\details When this function is called, stored signal data is cleared such that each BehavioralDataUnitPackage list can
*	only be retrieved once and includes data regarding signal values since the last time the function was called.
*/

QList<QSharedPointer<InputDataUnitPackage>> PictoEngine::getInputDataPackages()
{
	//Note that the call to getDataPackage() clears out any existing values,
	//so it should only be made once per frame on each signalChannel
	QList<QSharedPointer<InputDataUnitPackage>> returnList;
	QSharedPointer<InputDataUnitPackage> pack;
	QSharedPointer<GenericInput> chan = getSignalChannel("GenericInputs").dynamicCast<GenericInput>();
	if (chan)
	{
		pack = chan->getDataPackage();
		if (pack)
		{
			pack->setActionFrame(getLastFrameId());
			returnList.append(pack);
		}
	}
	return returnList;
}

/*! \brief Sends the latest behavioral data to the server aligned with this frame time
 *
 *	We update the server with all important data whenever a new frame is reported.  This includes 
 *	the time at which the most recent frame was drawn, as well as all the input
 *	coordinates from the subject and all changed properties and transition history.  This data is important
 *	so we send it as a registered command, which means that we will get confirmation
 *	when it's written to disk, and store it for resending if a confirmation isn't received in a set window.  
 *	It also means we don't have to wait around for a response before continuing experimental execution.
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
	QList<QSharedPointer<InputDataUnitPackage>> inputPackList = getInputDataPackages();

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
		//Set the latest frame time to the lfpReader_
		lfpReader_->createVirtualLfpData(frameTime);
		//Set the latest reward data to the rewardReader_
		QList<QSharedPointer<RewardDataUnit>> rewards = getDeliveredRewards();
		foreach(QSharedPointer<RewardDataUnit> reward,rewards)
		{
			rewardReader_->setLatestRewardData(reward->getTime().toDouble(),reward->getDuration());
		}
		//Add a spike to the spikeReader
		spikeReader_->createVirtualSpike(frameTime);

		//Set the latest signal data to the signal readers
		QStringList subChannels;
		QVector<float> chanData;
		//Loop through channels
		foreach(QSharedPointer<Picto::SignalChannel> chan,signalChannels_)
		{
			//Loop through this channel's subchannels and build a list of float data
			subChannels = chan->getSubchannels();
			chanData.clear();
			foreach(QString subChan,subChannels)
			{
				chanData.append(chan->peekValue(subChan));
			}
			//Add data to the appropriate signal reader
			Q_ASSERT(signalReaders_.contains(chan->getName().toLower()));
			QSharedPointer<LiveSignalReader> signalReader = signalReaders_.value(chan->getName().toLower());
			//Write the input signal the correct number of times according to the signal reader's set
			//sample period
			double samplePeriod = signalReader->getSamplePeriod();
			double nextSignalTime = signalReader->getLatestTime() + samplePeriod;
			while(nextSignalTime <= frameTime)
			{
				signalReader->setLatestSignalData(nextSignalTime,chanData);
				nextSignalTime += samplePeriod;
			}
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

	foreach(QSharedPointer<InputDataUnitPackage> inputPack, inputPackList)
	{
		if (inputPack && inputPack->length())
			inputPack->toXml(xmlWriter);
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

/*! \brief Sets the id of the latest frame to be rendered.*/
void PictoEngine::setLastFrame(qulonglong frameId)
{
	lastFrameId_ = frameId;
	//If no taskRunUnit_ was defined yet, do it now since we now have the id of the frame at which the run starts.
	if(!taskRunUnit_)
	{
		taskRunUnit_ = QSharedPointer<TaskRunDataUnit>(new TaskRunDataUnit(frameId,taskRunName_,""));
	}
}

/*! \brief Calls updateState of the stateUpdater_ object; however, nothing seems to be using this function so it should probably be removed.
*/
bool PictoEngine::updateCurrentStateFromServer()
{
	if(!stateUpdater_)
		return false;
	return stateUpdater_->updateState();
}

/*! Sets the CommandChannel used to send data to the Picto Server.  Returns true if the channel's status is connected, false otherwise.*/
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
/*! Returns the CommandChannel used to send data to the Picto Server.*/
QSharedPointer<CommandChannel> PictoEngine::getDataCommandChannel()
{
	return dataCommandChannel_;
}

/*! \brief Sets a PropertyTable to this Picto Engine for the purpose of tracking changed values and initvalues and sending them to
 *	the Picto Server.
 */
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

/*! \brief Uses the PropertyTable set in setPropertyTable() to send the current values of all Properties in the experiment to the
 *	Picto Server.
 */
void PictoEngine::sendAllPropertyValuesToServer()
{
	Q_ASSERT(propTable_);
	propTable_->reportChangeInAllProperties();
}

/*! \brief Sets the session ID of the current session to this experiment.  If Command Channels have been
 *	added to this engine, the sessionId will be copied to them as well.
 */
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

/*! \brief Changes the engine command to StopEngine so that a execution will end.
 *	\details Internally, calls stopAllSignalChannels()
 */
void PictoEngine::stop()
{ 
	engineCommand_ = StopEngine; 
	stopAllSignalChannels();
}

/*! \brief Sets the name of the system on which this PictoEngine's experiment is running.
 *	\details For example, in a real experiment, this would be the name of the PictoBox.
 *	\note The name returned here is not the computer's network id.  It is a simple string, stored inside
 *	Picto used to reference this instance of the Picto application.
 */
void PictoEngine::setName(QString name) 
{
	name_ = name;
	foreach(QSharedPointer<ControlPanelInterface> cp,controlPanelIfs_)
	{
		cp->nameChanged(name);
	}
	emit nameChanged(name);
}

/*! \brief Changes the reward duration for the input controller to the input duration (in ms).
 *	\details Also takes care of informing the ControlPanelInterface objects of the change and
 *	emits rewardDurationChanged().
 */
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

/*! \brief Changes the flush duration for the input controller to the input duration (in seconds).
 *	\details Also takes care of informing the ControlPanelInterface objects of the change and
 *	emits flushDurationChanged().
 */
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

/*! \brief Supplies a single reward on the input channel with the latest set reward duration.
 *	\sa giveReward(int,int,int)
 */
void PictoEngine::giveReward(int channel)
{
	if(rewardDurations_.size() <= channel)
		return;
	giveReward(channel,rewardDurations_[channel],0);
}

/*! \brief Starts flushing on the input channel or stops a flush that is in progress on that channel.
 *	\details Flush duration is the latest set duration.  Flush will continue for that duration, or until this
 *	function is called again.  Whichever comes first.
 *	\sa setFlushDuration()
 */
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

/*! \brief Returns this machines current IP Address.
 *	\note The method used for detecting the IP Address is that we use the first IPv4 address that isn't localhost 
 *	and is on an interface that is up and running.  This has worked correctly so far, but there could possibly be some issues 
 *	with strange networking setups.
 */
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

/*! \brief Returns the current engine command.
 *	\details Possiblities are: NoCommand, PlayEngine, StopEngine, PauseEngine
 */
int PictoEngine::getEngineCommand()
{
	return engineCommand_;
}

/*! \brief Performs various operations that need to occur once per frame as soon as possible after the first phosphor is rendered to the display.
 *	\details This is called when the VisualTarget detects that the first phosphor was rendered.  It first updates OutputSignalControllers to 
 *	implement any changes to output voltage signals, it then starts/stops any rewards that need to be updated, next it updates all data on input 
 *	signal channels (ie. reads in latest data from buffers, aligns input hardware timing with Picto timing), afterward ControlPanelInterfaces 
 *	respond to incoming commands from ControlPanels and SignalValueParameter objects are updated with the latest values of their referenced
 *	SignalChannels.
 */
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
