#include "SessionPlayer.h"
#include "../../common/storage/NeuralDataUnit.h"

using namespace Picto;

/*! \brief Constructs a new SessionPlayer.
*	\details sessState is the SessionState that will be used to playback all of the Session data.  sessLoader
*	is the FileSessionLoader that will load the Session file into the SessionState object before playback.
*/
SessionPlayer::SessionPlayer(QSharedPointer<SessionState> sessState, QSharedPointer<FileSessionLoader> sessLoader)
	:
	sessionState_(sessState),
	sessionLoader_(sessLoader),
	processing_(false),
	reachedEnd_(false),
	loading_(false)
{
	lastIndex_ = PlaybackIndex::minForTime(0);
	nextFrame_ = PlaybackIndex();

	connect(sessionState_.data(), SIGNAL(spikeEvent(double, int, int, QVector<float>)), this, SLOT(spikeEvent(double, int, int, QVector<float>)));
}

SessionPlayer::~SessionPlayer()
{
}

/*! \brief Restarts the current run from the beginning.
*/
void SessionPlayer::restart()
{
	markLoading(true);
	sessionState_->restartRun();
	lastIndex_ = PlaybackIndex::minForTime(0);
	nextFrame_ = PlaybackIndex();
	reachedEnd_ = false;
	markLoading(false);
}

/*! \brief Steps the Session time up to the input time.  SessionState signals indicating that Session events occured
*	will be triggered along the way.
*	\details This function calls stepToNextFrame() over and over again until the input time is no longer higher than
*	the time of the next frame.
*	If the current Run has not yet been started, some initialization will occur before starting stepToNextFrame() in
*	the loop.
*
*	If the input time is lower than the current time in the Run, the Run will be
*	restarted and then stepToNextFrame() will be called until the input time as usual.  This is due to the fact that
*	Analyses depend on a complete pass through the Session Run from the beginning.  If we "rewound", we would have to
*	undo Analysis script effects while we rewind to the input time and then restart Analysis at that point. That
*	is not supported so we need to just clear the entire Analysis and start over from scratch.
*	In cases where we end up restarting the run, the outside world will know what happened
*	because the startingRun() signal will be emitted, just as it always is when playback of a new run starts.
*
*	If we reach the end of the Run before arriving at the input time, the reachedEnd() will be emitted.
*/
bool SessionPlayer::stepToTime(double time)
{
	//qDebug(QString("Player: Step To Time called with input: %1").arg(time).toLatin1());
	if (time < 0)
		return false;
	if (processing_) return false;
	if (time < lastIndex_.time())
	{
		//If time goes down, restart from the beginning of the run and step back to it.
		restart();
		markLoading(true);
		return false;
	}
	if (time == lastIndex_.time())
	{
		markLoading(false);
		return true;
	}
	if (reachedEnd_)
	{
		markLoading(false);
		return true;
	}
	//Check if this is the beginning of a new run playback
	if (lastIndex_ == PlaybackIndex::minForTime(0))
	{
		//Get the first transition in this run
		int firstTransId = sessionState_->getTransitionState()->getFirstTransIdInRun();
		if (firstTransId == 0)
			return true;
		Q_ASSERT(sessionLoader_->getDesignRoot() && sessionLoader_->getDesignRoot()->getExperiment());
		QSharedPointer<Asset> trans = sessionLoader_->getDesignRoot()->getExperiment()->getDesignConfig()->getAsset(firstTransId);
		if (trans.isNull())
			return true;
		QString taskName = trans->getPath().split("::").first();
		QString runName = sessionLoader_->currRunName();
		//Tell the world that we're staring a new run
		emit startingRun(taskName, runName);
	}
	//if(!sessionLoader_->setCurrentTime(time))
	//{
	//	qDebug(QString("Player: Failed to set time to Loader").toLatin1());
	//	return false;
	//}
	//if(!sessionLoader_->dataIsReady(time))
	//{
	//	markLoading(true);
	//	qDebug(QString("Player: Loader not ready for time:%1").arg(time).toLatin1());
	//	return false;
	//}

	//Step to the input time or the end of the run. Whichever comes first.
	do
	{
		if (!stepToNextFrame(time))
		{
			if (time >= sessionLoader_->currRunDuration())
			{
				reachedEnd_ = true;
				emit reachedEnd();
			}
			break;

		}
	} while (time > nextFrame_.time());
	//sessionLoader_->setProcessedTime(getTime());
	//qDebug(QString("Player: Step To Time reached time: %1").arg(getTime()).toLatin1());
	markLoading(false);
	return true;
}

/*! \brief Returns the current playback time.
*	\details All Session events that occured up to this time have been emitted through SessionState signals.
*/
double SessionPlayer::getTime()
{
	return lastIndex_.time();
}

/*! \brief This function calls step() and doesn't appear to add any other functionality apart from reentrancy.  We should probably
*	consider getting rid of it.
*/
bool SessionPlayer::stepForward(double lookForward)
{
	if (processing_) return false;
	return step(lookForward);
}

/*! \brief Steps playback forward to the next Frame presentation so long as it occurs before the input lookForward
*	time.
*	\details The lookForward time is in runtime (ie. 0 is the beginning of the run) in units of seconds.
*/
bool SessionPlayer::stepToNextFrame(double lookForward)
{
	if (processing_) return false;
	nextFrame_ = sessionState_->getFrameState()->getNextIndex(lookForward);
	if (!nextFrame_.isValid() || nextFrame_.time() > lookForward)
		return false;
	while (lastIndex_ < nextFrame_)
		stepForward(lookForward);
	return true;
}

/*! \brief Steps playback forward to the next Session event so long as that event occurs before the input lookForward
*	time.
*	\details The lookForward time is in Run time (ie. 0 is the beginning of the run) in units of seconds.
*	\note Since some Picto events have no DataIds and are indexed only by their timestamps, the ordering of this
*	step() function could have been set up in multiple ways.  We have set it up so that all DataId indexed events marked
*	with the upcoming frame presentation time are traversed first.  Then, before the actual frame presentation is
*	traversed, we traverse all timestamp indexed Session events (spikes, lfp, signal channels, rewards) up to the time of
*	the upcoming frame presentation.  We currently just move through all of the time based events one after the other.
*	This means, for example that if we had a Position signal with times .003, .005, .007, etc and a spike at time .004
*	we would still see all of the Position signal data first and only then see the spike event, or alternatively, we might
*	see the spike event first and only then all of the Position signal data.  Strictly speaking, we should probably
*	be presenting these in their time order too; however it is currently irrelevant because the only place where anything
*	can actually happen with this data is in Analysis scripts which are part of the StateMachine traversal.  Analysis Scripts are
*	triggered by Transition traversals and frame presentations, so whenever an Analysis script is called, all of the Timestamp
*	indexed events for the latest frame will have been triggered anyway.  It should also be pointed out that even though
*	timestamp indexed events might sometimes have occured just after one frame was presented, all DataId indexed events will be
*	traversed before those timestamp based events are traversed.  We thought long and hard about this, and it has to do with the
*	fact that the Experiment system itself only has access to position data from the previous frame regardless of when position
*	data for the upcoming frame is read.  If you want to change this, think about it for a long long time.
*/
bool SessionPlayer::step(double lookForward)
{
	processing_ = true;
	PlaybackIndex currFrameId = sessionState_->getFrameState()->getCurrentIndex();
	PlaybackIndex nextFrameId = sessionState_->getFrameState()->getNextIndex(lookForward);
	QSharedPointer<DataState> stateToTrigger = getNextTriggerState(lookForward);
	if (stateToTrigger.isNull())
	{
		processing_ = false;
		return false;
	}
	if (stateToTrigger->getNextIndex(nextFrameId.time()) == nextFrameId)
	{	//If the next stateToTrigger is a frame, step to all time based data states first (ie. reward, signals, spike, lfp).
		PlaybackIndex frameIndexForTime = nextFrameId;
		if (frameIndexForTime.isValid())
		{
			PlaybackIndex prevSpikeInd = sessionState_->getSpikeState()->getCurrentIndex();

			PlaybackIndex timeIndex = PlaybackIndex::maxForTime(frameIndexForTime.time());
			QList<QSharedPointer<DataState>> timeIndexedStates = sessionState_->getStatesIndexedByTime();
			foreach(QSharedPointer<DataState> dataState, timeIndexedStates)
			{
				dataState->moveToIndex(timeIndex);
			}
		}
	}
	stateToTrigger->moveToIndex(stateToTrigger->getNextIndex(nextFrameId.time()));
	lastIndex_ = stateToTrigger->getCurrentIndex();
	processing_ = false;
	return true;
}

/*! \brief Use this function to mark when loading starts and stops.  Internally, it takes care of only emitting
*	the loading() signal when loading first starts and when it first ends.
*/
void SessionPlayer::markLoading(bool load)
{
	if (loading_ != load)
	{
		loading_ = load;
		emit loading(loading_);
	}
}

/*! \brief Returns the SessionState's DataState which is DataId indexed and has the next lowest
*	DataId so long as it occurs before the input lookForward time.
*	\details This is used in order to step through the session in the proper order.  We always choose
*	the next lowest DataId so that can be sure, for example, that any Property value changes that occured before
*	a particular Transition was traversed in the actual experiment will always be played back before that
*	Transition and not afterward.
*
*	The lookForward time is in Run time (ie. 0 is the beginning of the run) in units of seconds.
*	If no appropriate DataState can be found before the lookForward time, an empty shared DataState
*	pointer is returned.
*/
QSharedPointer<DataState> SessionPlayer::getNextTriggerState(double lookForward)
{
	//Look for the next property, transition or frame
	//with lowest index. 
	QList<QSharedPointer<DataState>> idIndexedStates = sessionState_->getStatesIndexedById();
	int nBuffer;
	PlaybackIndex indexBuffer;
	PlaybackIndex iterationIndex;
	double boundaryTime = sessionState_->getFrameState()->getNextIndex(lookForward).time();

	//Intialize the nBuffer and indexBuffer with the first
	//valid entries available
	int i = 0;
	do
	{
		nBuffer = i;
		indexBuffer = idIndexedStates[i++]->getNextIndex(boundaryTime);
	} while (!indexBuffer.isValid() && i < idIndexedStates.size());

	//Get the state with the lowest index
	for (; i<idIndexedStates.size(); i++)
	{
		iterationIndex = idIndexedStates[i]->getNextIndex(boundaryTime);
		if (iterationIndex.isValid()
			&& (iterationIndex < indexBuffer)
			)
		{
			indexBuffer = iterationIndex;
			nBuffer = i;
		}

	}
	if (!indexBuffer.isValid())
		return QSharedPointer<DataState>();
	return idIndexedStates[nBuffer];
}
void SessionPlayer::spikeEvent(double time, int channel, int unit, QVector<float> waveform)
{
	// Tell the Neural Data Listeners	
	//alert NeuralDataListener
	//Get the first transition in this run
	int firstTransId = sessionState_->getTransitionState()->getFirstTransIdInRun();
	if (firstTransId != 0)
	{
		QSharedPointer<Experiment> pExperiment = sessionLoader_->getDesignRoot()->getExperiment().objectCast<Experiment>();
		if (pExperiment)
		{
			QSharedPointer<Asset> trans = pExperiment->getDesignConfig()->getAsset(firstTransId);
			if (trans)
			{
				QString taskName = trans->getPath().split("::").first();
				if (pExperiment->getTaskByName(taskName))
				{
					QSharedPointer<Picto::NeuralDataUnit> neuralData = QSharedPointer<Picto::NeuralDataUnit>(new Picto::NeuralDataUnit());
					neuralData->setTimestamp(time);
					neuralData->setChannel(channel);
					neuralData->setUnit(unit);
					neuralData->setFittedtime(time);

					pExperiment->getTaskByName(taskName)->getTaskConfig()->notifyNeuralDataListeners(*neuralData);
				}
			}
		}
	}
	
	//END NeuralDataListeners
}
