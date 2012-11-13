#include "SessionPlayer.h"
using namespace Picto;

SessionPlayer::SessionPlayer(QSharedPointer<SessionState> sessState)
:
sessionState_(sessState)
{
	sessionStateReset();
	connect(sessionState_.data(),SIGNAL(wasReset()),this,SLOT(sessionStateReset()));
}

SessionPlayer::~SessionPlayer()
{
}

bool SessionPlayer::stepForward()
{
	return step(false);
}

bool SessionPlayer::stepToNextFrame()
{
	PlaybackIndex nextFrameId = sessionState_->getFrameState()->getNextIndex(-1);
	if(!nextFrameId.isValid())
		return false;
	while(lastIndex_ < nextFrameId)
		stepForward();
	return true;
}

bool SessionPlayer::stepBack()
{
	return step(true);
}

bool SessionPlayer::stepToPrevFrame()
{
	PlaybackIndex prevFrameId = sessionState_->getFrameState()->getPrevIndex(-1);
	if(!prevFrameId.isValid())
		return false;
	while(lastIndex_ > prevFrameId)
		stepBack();
	return true;
}

bool SessionPlayer::step(bool backward)
{
	PlaybackIndex prevFrameId = sessionState_->getFrameState()->getPrevIndex(-1);
	PlaybackIndex currFrameId = sessionState_->getFrameState()->getCurrentIndex();
	PlaybackIndex nextFrameId = sessionState_->getFrameState()->getNextIndex(-1);
	QSharedPointer<DataState> stateToTrigger = getNextTriggerState(backward);
	if(stateToTrigger.isNull())
		return false;
	if	(	(!backward && (stateToTrigger->getNextIndex(nextFrameId.time()) == nextFrameId))
			|| (backward && (lastIndex_ == currFrameId))
		)
	{	//When moving forward, if the next stateToTrigger is a frame,  or
		//when moving backward if the latest triggered state is a frame, step to
		//all time based data states first (ie. reward, signals, spike, lfp).
		PlaybackIndex frameIndexForTime = (backward)?prevFrameId:nextFrameId;
		if(frameIndexForTime.isValid())
		{
			PlaybackIndex timeIndex = PlaybackIndex::maxForTime(frameIndexForTime.time());
			QList<QSharedPointer<DataState>> timeIndexedStates = sessionState_->getStatesIndexedByTime();
			foreach(QSharedPointer<DataState> dataState,timeIndexedStates)
			{
				dataState->setCurrentIndex(timeIndex);
			}
		}
	}
	stateToTrigger->setCurrentIndex((backward)?stateToTrigger->getPrevIndex(prevFrameId.time()):stateToTrigger->getNextIndex(nextFrameId.time()));
	lastIndex_ = stateToTrigger->getCurrentIndex();
	return true;
}

bool SessionPlayer::stepToTime(double time)
{
	if(time == lastIndex_.time())
		return true;
	while(time > lastIndex_.time())
	{
		if(!stepToNextFrame())
			return false;
	}
	while(time < lastIndex_.time())
	{
		if(!stepToPrevFrame())
			return false;
	}
	return true;
}

QSharedPointer<DataState> SessionPlayer::getNextTriggerState(bool backward)
{
	//When moving forward, look for the next property, transition or frame
	//with lowest index.  When moving backward, look for the current property,
	//transition or frame with highest index.
	QList<QSharedPointer<DataState>> idIndexedStates = sessionState_->getStatesIndexedById();
	int nBuffer;
	PlaybackIndex indexBuffer;
	PlaybackIndex iterationIndex;
	double boundaryTime = backward?
							sessionState_->getFrameState()->getPrevIndex(-1).time()
							:sessionState_->getFrameState()->getNextIndex(-1).time();

	//Intialize the nBuffer and indexBuffer with the first
	//valid entries available
	int i=0;
	do
	{
		nBuffer = i;
		if(backward)
			indexBuffer = idIndexedStates[i++]->getCurrentIndex();
		else
			indexBuffer = idIndexedStates[i++]->getNextIndex(boundaryTime);
	} while(!indexBuffer.isValid() && i < idIndexedStates.size());
	
	//Get the state with the highest/lowest index for backward/forward case
	for(;i<idIndexedStates.size();i++)
	{
		if(backward)
		{
			iterationIndex = idIndexedStates[i]->getCurrentIndex();
		}
		else
		{
			iterationIndex = idIndexedStates[i]->getNextIndex(boundaryTime);
		}
		if	(	iterationIndex.isValid()
				&&	(
						(!backward && iterationIndex < indexBuffer)
						|| (backward && iterationIndex > indexBuffer)
					)
			)
		{
			indexBuffer = iterationIndex;
			nBuffer = i;
		}
		
	}
	if(!indexBuffer.isValid())
		return QSharedPointer<DataState>();
	return idIndexedStates[nBuffer];
}

void SessionPlayer::sessionStateReset()
{
	lastIndex_ = PlaybackIndex();
}