#include "SlaveExperimentDriver.h"
#include "../statemachine/State.h"
#include "../memleakdetect.h"

using namespace Picto;
#define FRAMEMS 16
SlaveExperimentDriver::SlaveExperimentDriver(QSharedPointer<Experiment> exp,QSharedPointer<StateUpdater> updater)
{
	experiment_ = exp;
	updater_ = updater;
	designConfig_ = experiment_->getDesignConfig();
	frameTimer_.restart();

	//Initialize scripting for this experiment in case this hasn't been done yet
	experiment_->initScripting(false);
	renderingEnabled_ = true;

	connect(updater_.data(),SIGNAL(startingRun(QString,QString)),this,SLOT(masterRunStarting(QString,QString)));
	connect(updater_.data(),SIGNAL(endingRun()),this,SLOT(masterRunEnding()));

	connect(updater_.data(),SIGNAL(propertyValueChanged(int, QString)),this,SLOT(masterPropertyValueChanged(int, QString)));
	connect(updater_.data(),SIGNAL(propertyInitValueChanged(int, QString)),this,SLOT(masterPropertyInitValueChanged(int, QString)));
	connect(updater_.data(),SIGNAL(transitionActivated(int)),this,SLOT(masterTransitionActivated(int)));
	connect(updater_.data(),SIGNAL(framePresented(double)),this,SLOT(masterFramePresented(double)));
	connect(updater_.data(),SIGNAL(rewardSupplied(double,int,int)),this,SLOT(masterRewardSupplied(double,int,int)));
	connect(updater_.data(),SIGNAL(signalChanged(QString,QStringList,QVector<float>)),this,SLOT(masterSignalChanged(QString,QStringList,QVector<float>)));
	connect(updater_.data(),SIGNAL(disableRendering(bool)),this,SLOT(disableRendering(bool)));

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

void SlaveExperimentDriver::renderFrame()
{
	if(!currElement_)
		return;
	
	if(frameTimer_.elapsed() < FRAMEMS || !renderingEnabled_)
	{
		//Even if we don't render the frame, we need to make sure that the signal
		//channels' data is getting cleared out each frame.  Otherwise they'll become
		//huge and use up all our memory.  In general, slaveRenderFrame() takes care
		//of this.
		experiment_->getEngine()->emptySignalChannels();
		return;
	}

	//Don't render the frame more than once per FRAMEMS
	currElement_->slaveRenderFrame(experiment_->getEngine());
	frameTimer_.restart();
}

void SlaveExperimentDriver::handleEvent(SlaveEvent& event)
{
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
				return;
			}
			QSharedPointer<Transition> trans = asset.staticCast<Transition>();
	
			//As soon as a transition comes in and it has a source result, 
			//we set that transition's source's latest result to the transition's 
			//source result.  Then, if the source is a state machine element, we 
			//call it's AnalysisExitScripts
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
	
			QSharedPointer<Asset> destAsset = trans->getDestinationAsset();
			QString destName = destAsset?destAsset->getName():"";
			if(!destAsset || !destAsset->inherits("Picto::StateMachineElement"))
			{
				Q_ASSERT(!destAsset || destAsset->inherits("Picto::Result"));
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
			//Set is as the current element
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
	//Reset Analysis elements to initial states on Top
	top->resetScriptableAnalysisValues();
	//Run AnalysisEntryScript on Top
	top->runAnalysisEntryScripts();
}
void SlaveExperimentDriver::masterRunEnding()
{
	if(currTask_.isEmpty())
		return;	//This could happen if the task never actually started before it was stopped.
	QSharedPointer<StateMachine> top = experiment_->getTaskByName(currTask_)->getStateMachine();
	//Run AnalysisExitScripts on Top
	top->runAnalysisExitScripts();
	designConfig_->markRunEnd();
	eventQueue_.reset();
}
void SlaveExperimentDriver::masterPropertyValueChanged(int propId, QString value)
{
	eventQueue_.addPropChange(propId,value);
}
void SlaveExperimentDriver::masterPropertyInitValueChanged(int propId, QString value)
{
	eventQueue_.addInitPropChange(propId,value);
}
void SlaveExperimentDriver::masterTransitionActivated(int transId)
{
	eventQueue_.addTransActivation(transId);
}
void SlaveExperimentDriver::masterFramePresented(double time)
{
	//Report the time of the frame following all the property and transition updates that are
	//about to occur.
	experiment_->getDesignConfig()->getFrameTimerFactory()->setNextFrameTime(time);
	//Perform all queued events that occured up to this frame since the last one
	//(We do this after setting next frame time so that when reading absolute time values during the course
	//of state machine execution, we will read the absolute time value of the frame that follows
	//the current position in the state machine.  This is necessary since the timing 
	//paradigm in Picto is that all property changes and transitions take effect at the time
	//of the frame that follows them.  Note that this matters in analysis because times
	//of behavioral events can be compared to times of separately timed signals (eye position,
	//neural data, etc).  See FrameResolutionTimer for more info.
	SlaveEvent event;
	while((event = eventQueue_.takeFirstEvent()).type != SlaveEvent::INVALID)
	{
		handleEvent(event);
	}
	//Render frame
	renderFrame();
	
	//Tell all timers that work on single frame resolution what the latest frame time is
	experiment_->getDesignConfig()->getFrameTimerFactory()->setLastFrameTime(time);

	//----------  Run the analysis frame scripts ---------------------------------------
	QSharedPointer<State> currState = currElement_.dynamicCast<State>();	
	if(currState)
	{
		currState->runAnalysisFrameScripts();
	}
}
void SlaveExperimentDriver::masterRewardSupplied(double,int duration,int channel)
{
	experiment_->getEngine()->giveReward(channel,duration,duration);
}
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
void SlaveExperimentDriver::disableRendering(bool disable)
{
	bool shouldRenderFrame = !disable && !renderingEnabled_;
	renderingEnabled_=!disable;
	if(shouldRenderFrame)
		renderFrame();
}