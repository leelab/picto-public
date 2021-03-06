#include "SlaveExperimentDriver.h"
#include "../statemachine/State.h"
#include "../memleakdetect.h"


#include "../variable/NumberVariable.h"
#include "../parameter/BooleanParameter.h"

using namespace Picto;
#define FRAMEMS 16
/*! \brief Constructs a SlaveExperimentDriver.
 *	\details The updater provides State/Transition/Input data from the Session master, be it a remotely running
 *	instance of the Picto Director or simply a session file being played back in the Playback viewer.  Data provided
 *	from the updater is sent into the exp Experiment and rendered on screen.
 */
SlaveExperimentDriver::SlaveExperimentDriver(QSharedPointer<Experiment> exp, QSharedPointer<StateUpdater> updater)
	: paused_(false), initialized_(false), finalTransitionID_(0)
{
	experiment_ = exp;
	updater_ = updater;
	designConfig_ = experiment_->getDesignConfig();
	frameTimer_.restart();

	//Initialize scripting for this experiment in case this hasn't been done yet
	experiment_->initScripting(false);
	renderingEnabled_ = true;

	connect(updater_.data(), SIGNAL(startingRun(QString, QString)),
		this, SLOT(masterRunStarting(QString, QString)));
	connect(updater_.data(), SIGNAL(endingRun()),
		this, SLOT(masterRunEnding()));

	connect(updater_.data(), SIGNAL(processQueue()),
		this, SLOT(handleEvents()));

	//Version With DataId
	connect(updater_.data(), SIGNAL(propertyValueChanged(qulonglong, int, QString)),
		this, SLOT(masterPropertyValueChanged(qulonglong, int, QString)));
	connect(updater_.data(), SIGNAL(propertyInitValueChanged(qulonglong, int, QString)),
		this, SLOT(masterPropertyInitValueChanged(qulonglong, int, QString)));
	connect(updater_.data(), SIGNAL(transitionActivated(qulonglong, int, bool)),
		this, SLOT(masterTransitionActivated(qulonglong, int, bool)));

	//Version Without DataId
	connect(updater_.data(), SIGNAL(propertyValueChanged(int, QString)),
		this, SLOT(masterPropertyValueChanged(int, QString)));
	connect(updater_.data(), SIGNAL(propertyInitValueChanged(int, QString)),
		this, SLOT(masterPropertyInitValueChanged(int, QString)));
	connect(updater_.data(), SIGNAL(transitionActivated(int)),
		this, SLOT(masterTransitionActivated(int)));

	connect(updater_.data(), SIGNAL(framePresented(double)),
		this, SLOT(masterFramePresented(double)));
	connect(updater_.data(), SIGNAL(rewardSupplied(double, int, int)),
		this, SLOT(masterRewardSupplied(double, int, int)));
	connect(updater_.data(), SIGNAL(signalChanged(QString, QStringList, QVector<float>)),
		this, SLOT(masterSignalChanged(QString, QStringList, QVector<float>)));
	connect(updater_.data(), SIGNAL(disableRendering(bool)),
		this, SLOT(disableRendering(bool)));

	//Put the various data sources into the design config for access from analysis parameters
	experiment_->getDesignConfig()->setFrameReader(updater_->getFrameReader());
	experiment_->getDesignConfig()->setLfpReader(updater_->getLfpReader());
	experiment_->getDesignConfig()->setRewardReader(updater_->getRewardReader());
	experiment_->getDesignConfig()->setRunNotesReader(updater_->getRunNotesReader());
	experiment_->getDesignConfig()->setSpikeReader(updater_->getSpikeReader());
	foreach(QString signalName,updater_->getSignalReaderNames())
	{
		experiment_->getDesignConfig()->setSignalReader(signalName,updater_->getSignalReader(signalName));
	}
}

/*! \brief Renders the current frame by calling State::slaveRenderFrame().
 *	\details Note that in slave mode, the Picto Experiment is working with the Qt Event loop which is why we are calling
 *	slaveRenderFrame from an outside class.  In master experiment execution, the experiment is one giant loop of code
 *	that doesn't make use of the Qt Event loop.  In that case frame rendering is handled as part of running a
 *	State/PausePoint.
 */
void SlaveExperimentDriver::renderFrame()
{
	if(!currElement_)
		return;
	
	if(frameTimer_.elapsed() < FRAMEMS || !renderingEnabled_)
	{
		//Even if we don't render the frame, we need to make sure that the signal channels' data is getting cleared out
		//	each frame.  Otherwise they'll become huge and use up all our memory.  In general, slaveRenderFrame() takes
		//	care of this.
		experiment_->getEngine()->emptySignalChannels();
		return;
	}

	//Don't render the frame more than once per FRAMEMS
	currElement_->slaveRenderFrame(experiment_->getEngine());
	frameTimer_.restart();
}

/*! \brief Handles the appropriate response to each input SlaveEvent.
 *	\details When a PROP_VAL_CHANGE or INIT_PROP_VAL_CHANGE comes in, we change the value of our local Property.
 *	When a TRANS_ACTIVATED comes in, we move control to the appropriate StateMachineElement and call whatever
 *	Analysis scripts need to be called in the process.
 *	\note Rather than handling each different type of event as it comes in from the StateUpdater, we
 *	queue them up at handle each frame all at once.  This gives us the flexibility to know all data about the frame
 *	before it is actually presented to the display.  It should be noted, however, that we are not taking advantage of
 *	this capability to any significant extent at this point.
*/
void SlaveExperimentDriver::handleEvent(SlaveEvent& event)
{
	//These conditions should only fulfilled when joining an experiment mid-session
	if (!initialized_ && !(event.id < 0 && event.type == SlaveEvent::TRANS_ACTIVATED) && event.dataId > finalTransitionID_)
	{
		QSharedPointer<Asset> asset = designConfig_->getAsset(event.id);
		while (asset && !asset->inherits("Picto::Task"))
		{
			asset = asset->getParentAsset();
		}

		if (asset)
		{
			remoteRunStarting(asset->getName());
		}
	}

	switch(event.type)
	{
	case SlaveEvent::PROP_VAL_CHANGE:
		{
			QSharedPointer<Asset> asset = designConfig_->getAsset(event.id);
			Q_ASSERT(asset && asset->inherits("Picto::Property"));
			asset.staticCast<Property>()->valFromUserString(event.value);
		}
		break;
	case SlaveEvent::INIT_PROP_VAL_CHANGE:
		{
			QSharedPointer<Asset> asset = designConfig_->getAsset(event.id);
			Q_ASSERT(asset && asset->inherits("Picto::Property"));
			asset.staticCast<Property>()->initValFromUserString(event.value);
		}
		break;
	case SlaveEvent::TRANS_ACTIVATED:
		{
			QSharedPointer<Asset> asset = designConfig_->getAsset(event.id);
			if(!asset || !asset->inherits("Picto::Transition"))
			{
				Q_ASSERT(!asset);
				if (!event.value.isEmpty())
				{
					finalTransitionID_ = event.dataId;
					remoteRunEnding();
				}
				return;
			}

			QSharedPointer<Transition> trans = asset.staticCast<Transition>();

			//As soon as a transition comes in and it has a source result, we set that transition's source's latest
			//	result to the transition's source result.  Then, if the source is a state machine element, we call its
			//	AnalysisExitScripts
			QSharedPointer<Asset> sourceResult = trans->getSourceResultAsset();
			QSharedPointer<Asset> sourceAsset = trans->getSourceAsset();
			if(sourceResult)
			{
				sourceAsset.staticCast<ResultContainer>()->setLatestResult(sourceResult->getName());
			}
			if(sourceAsset && sourceAsset->inherits("Picto::StateMachineElement"))
			{
				sourceAsset.staticCast<StateMachineElement>()->runAnalysisExitScripts();
			}

			QString newResult = trans->getDestination();
			if(newResult == "EngineAbort")
			{
				return;
			}
	
			//Plots	- right after analyses exit script		
			if (sourceAsset && sourceAsset->inherits("Picto::StateMachineElement"))
			{
				QSharedPointer<StateMachineElement> srcElement = sourceAsset.staticCast<StateMachineElement>();
				int srcEltID = sourceAsset ? sourceAsset->getAssetId() : 0;
				if (srcElement && srcElement->getAlignParam())
					emit(alignPlot(srcEltID));
				if (srcElement && (srcElement->getSelEventParam() || (srcElement->getMarkerParam())))
					emit(eventAdded(srcEltID));
			}
			if (sourceAsset && sourceAsset->inherits("Picto::ScriptableContainer"))
			{		
				QSharedPointer<ScriptableContainer> srcElement = sourceAsset.staticCast<ScriptableContainer>();
				QStringList childTags = srcElement->getDefinedChildTags();
				foreach(QString childTag, childTags)
				{
					QList<QSharedPointer<Asset>> tagChildren = srcElement->getGeneratedChildren(childTag);
					foreach(QSharedPointer<Asset> tagChild, tagChildren)
					{
						if (tagChild->inherits("Picto::Scriptable"))
						{
							QSharedPointer<Scriptable> childElt = tagChild.staticCast<Scriptable>();
							int srcEltChildID = tagChild ? tagChild->getAssetId() : 0;
							if (childElt && childElt->getScriptParam())
							{
								int scTrialVal = -1;
								if (tagChild && tagChild.objectCast<NumberVariable>())
								{
									scTrialVal = tagChild.objectCast<NumberVariable>()->getValue();
								}
								else if (tagChild && tagChild.objectCast<BooleanParameter>())
								{
									scTrialVal = tagChild.objectCast<BooleanParameter>()->getValue();
								}
								emit(scriptContAdded(srcEltChildID));
							}
						}
					}
				}
			}
			//end Plots

			QSharedPointer<Asset> destAsset = trans->getDestinationAsset();
			QString destName = destAsset?destAsset->getName():"";
			if(!destAsset || !destAsset->inherits("Picto::StateMachineElement"))
			{
				Q_ASSERT(!destAsset || destAsset->inherits("Picto::Result"));
				if (event.id < 0 && !event.value.isEmpty())
				{
					QString currTask = "";
					if (trans->getParentAsset())
						currTask = trans->getParentAsset()->getName();
					remoteRunStarting(currTask);
				}
				return;
			}
			//If we got here, we transitioned to a StateMachineElement.  
			//Tell the world if the task has changed
			QString currTask = destAsset->getPath().split("::").first();
			if(currTask != currTask_)
			{
				currTask_ = currTask;
				emit taskChanged(currTask_);
			}
			//Set it as the current element
			//Reset its values to initial conditions
			//run its AnalysisEntryScripts
			//start it up
			currElement_ = destAsset.staticCast<StateMachineElement>();
			currElement_->resetScriptableValues();
			currElement_->resetScriptableAnalysisValues();
			currElement_->runAnalysisEntryScripts();
			currElement_->slaveRun(experiment_->getEngine());
		}
		break;
	}
}

/*! \brief Called when a new Run starts.  Handles various initializations that need to occur at the beginning of a new run.
 *	\details As part of this function, the taskChanged() signal is emitted.
 */
void SlaveExperimentDriver::masterRunStarting(QString taskName,QString runName)
{
	experiment_->getDesignConfig()->getFrameTimerFactory()->resetAllTimers();
	eventQueue_.reset();
	currElement_.clear();
	if(taskName != currTask_)
	{
		currTask_ = taskName;
		emit taskChanged(currTask_);
	}
	//Tell the design config about the new run
	designConfig_->markRunStart(runName);

	QSharedPointer<StateMachine> top = experiment_->getTaskByName(currTask_)->getStateMachine();

	experiment_->getTaskByName(currTask_)->getTaskConfig()->requestUpdate();
	//Reset Analysis elements to initial states on Top
	top->resetScriptableAnalysisValues();
	//Run AnalysisEntryScript on Top
	top->runAnalysisEntryScripts();
	paused_ = false;
	initialized_ = true;
}

/*! \brief Called when the current Run ends.  Handles various deinitializations that need to occur at the end of a run.
 */
void SlaveExperimentDriver::masterRunEnding()
{
	if(currTask_.isEmpty())
		return;	//This could happen if the task never actually started before it was stopped.
	QSharedPointer<StateMachine> top = experiment_->getTaskByName(currTask_)->getStateMachine();
	//Run AnalysisExitScripts on Top
	top->runAnalysisExitScripts();
	designConfig_->markRunEnd();
	eventQueue_.reset();
	paused_ = false;
	initialized_ = false;
}

//! Called when starting a Remote run.  Runs only a subset of the initialization that masterRunStarting runs.
void SlaveExperimentDriver::remoteRunStarting(QString taskName)
{
	currElement_.clear();
	if (taskName != currTask_)
	{
		currTask_ = taskName;
		emit taskChanged(currTask_);
	}
	//Tell the design config about the new run
	QDateTime dateTime = QDateTime::currentDateTime();
	designConfig_->markRunStart(taskName + "_" + dateTime.toString("yyyy_MM_dd__hh_mm_ss"));

	experiment_->getDesignConfig()->getFrameReader().objectCast<LiveFrameReader>()->setRunStart();

	QSharedPointer<StateMachine> top = experiment_->getTaskByName(currTask_)->getStateMachine();
	Q_ASSERT(top);
	//Reset Analysis elements to initial states on Top
	top->resetScriptableAnalysisValues();
	//Run AnalysisEntryScript on Top
	top->runAnalysisEntryScripts();
	paused_ = false;
	initialized_ = true;
}

//! Called when ending a Remote run.  Runs only a subset of the initialization that masterRunEnding runs.
void SlaveExperimentDriver::remoteRunEnding()
{
	//This could happen if the task never actually started before it was stopped.
	if (currTask_.isEmpty())
		return;
	QSharedPointer<StateMachine> top = experiment_->getTaskByName(currTask_)->getStateMachine();
	//Run AnalysisExitScripts on Top
	top->runAnalysisExitScripts();
	designConfig_->markRunEnd();

	experiment_->getDesignConfig()->getFrameReader().objectCast<LiveFrameReader>()->setRunEnd();

	paused_ = false;
	initialized_ = false;
}

/*! \brief Called when a Property value changes in the master.  Adds a corresponding event to the event queue.
 */
void SlaveExperimentDriver::masterPropertyValueChanged(qulonglong dataId, int propId, QString value)
{
	eventQueue_.addPropChange(dataId, propId, value);
}

/*! \brief Called when a Property initValue changes in the master.  Adds a corresponding event to the event queue.
 */
void SlaveExperimentDriver::masterPropertyInitValueChanged(qulonglong dataId, int propId, QString value)
{
	eventQueue_.addInitPropChange(dataId, propId,value);
}

/*! \brief Called when executions traverses a Transition in the master.  Adds a corresponding event to the event queue.
 */
void SlaveExperimentDriver::masterTransitionActivated(qulonglong dataId, int transId, bool remoteRunSignal)
{
	eventQueue_.addTransActivation(dataId, transId, remoteRunSignal);
}

/*! \brief Called when a Property value changes in the master.  Adds a corresponding event to the event queue.
 */
void SlaveExperimentDriver::masterPropertyValueChanged(int propId, QString value)
{
	eventQueue_.addPropChange(0, propId, value);
}

/*! \brief Called when a Property initValue changes in the master.  Adds a corresponding event to the event queue.
 */
void SlaveExperimentDriver::masterPropertyInitValueChanged(int propId, QString value)
{
	eventQueue_.addInitPropChange(0, propId,value);
}

/*! \brief Called when executions traverses a Transition in the master.  Adds a corresponding event to the event queue.
 */
void SlaveExperimentDriver::masterTransitionActivated(int transId)
{
	eventQueue_.addTransActivation(0, transId, false);
}

/*! \brief Called when a frame is presented in the master.  Goes through event queue handling events one by one.  Updates
 *	FrameResolutionTimer objects, renders the frame and calls the current AnalysisFrameScript.
 */
void SlaveExperimentDriver::masterFramePresented(double time)
{
	//Report the time of the frame following all the property and transition updates that are about to occur.
	experiment_->getDesignConfig()->getFrameTimerFactory()->setNextFrameTime(time);

	if (experiment_->getDesignConfig()->getFrameReader().objectCast<LiveFrameReader>())
	{
		experiment_->getDesignConfig()->getFrameReader().objectCast<LiveFrameReader>()->setLatestFrameTime(time);
	}

	//Perform all queued events that occured up to this frame since the last one 
	//	(We do this after setting next frame time so that when reading absolute time values during the course of state
	//	machine execution, we will read the absolute time value of the frame that follows the current position in the
	//	state machine.  This is necessary since the timing paradigm in Picto is that all property changes and transitions
	//	take effect at the time of the frame that follows them.  Note that this matters in analysis because times of
	//	behavioral events can be compared to times of separately timed signals (eye position, neural data, etc).
	//	See FrameResolutionTimer for more info.
	handleEvents();
	//Render frame
	renderFrame();

	//Tell all timers that work on single frame resolution what the latest frame time is
	experiment_->getDesignConfig()->getFrameTimerFactory()->setLastFrameTime(time);

	//----------  Run the analysis frame scripts ---------------------------------------
	QSharedPointer<State> currState = currElement_.dynamicCast<State>();	
	if (currState && !paused_)
	{
		currState->runAnalysisFrameScripts();
	}
}

/*! \brief Called whenever queued events should be processed.  First it unlocks and sorts the new additions, then
 *	it processes them.
 */
void SlaveExperimentDriver::handleEvents()
{
	//Move events to main queue and sort.
	eventQueue_.prepareEvents();

	SlaveEvent event;
	while ((event = eventQueue_.takeFirstEvent()).type != SlaveEvent::INVALID)
	{
		handleEvent(event);
	}
}

/*! \brief Called when a reward is supplied in the master.  Adds a corresponding reward to the PictoEngine to be supplied
 *	when the next frame is rendered.
 */
void SlaveExperimentDriver::masterRewardSupplied(double,int duration,int channel)
{
	experiment_->getEngine()->giveReward(channel,duration,duration);
}
/*! \brief Called when a list of ne signal values comes in from the master.  Adds the values to the SignalChannel.
 *	\sa SignalChannel::insertValue()
 */
void SlaveExperimentDriver::masterSignalChanged(QString name,QStringList subChanNames,QVector<float> vals)
{
	int numSubChans = subChanNames.size();
	QSharedPointer<SignalChannel> sigChan = experiment_->getEngine()->getSignalChannel(name);
	if(!sigChan)
		return;
	for(int i=0;i<vals.size();i++)
	{
		sigChan->insertValue(subChanNames[i%numSubChans],vals[i]);
	}
}
/*! \brief Disables/Enables rendering of frames to the RenderingTarget.
 *	\details This is useful for cases when we are fast forwarding or skipping around in session playback and don't need to
 *	actually see every frame on screen.  Even though the frames aren't rendered, however, all other logic operates
 *	exactly the same whether rendering is enabled or disabled.
 */
void SlaveExperimentDriver::disableRendering(bool disable)
{
	bool shouldRenderFrame = !disable && !renderingEnabled_;
	renderingEnabled_=!disable;
	if(shouldRenderFrame)
		renderFrame();
}

//! Pauses the execution of the Analysis Frame scripts - called by the owning viewer
void SlaveExperimentDriver::pause(bool paused)
{
	paused_ = paused;
}