#include "SessionPlayer.h"
using namespace Picto;

SessionPlayer::SessionPlayer(QSharedPointer<SessionState> sessState)
:
sessionState_(sessState),
started_(false),
lastTime_(-1),
onFrame_(false)
{
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
	qulonglong nextFrameId = sessionState_->getFrameState()->getNextIndex();
	if(!nextFrameId)
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
	qulonglong prevFrameId = sessionState_->getFrameState()->getPrevIndex();
	if(!prevFrameId)
		return false;
	while(lastIndex_ > prevFrameId)
		stepBack();
	return true;
}

bool SessionPlayer::step(bool backward)
{
	qulonglong nextIndex;
	bool moveToFrame = false;
	double nextTime = sessionState_->getFrameReader()->getLatestTime();
	if(!lastIndex_)
	{
		if(backward)
			return false;

		//Moving forward, nextIndex is the first index available
		nextIndex = sessionState_->getTransitionState()->getFirstIndex();
		if(!nextIndex)
			return false;
	}
	else
	{
		//Figure out next step
		qulonglong nextIndex;
		qulonglong nextFrameIndex;
		if(backward)
		{//Get last transition or frame, whatever came last
			nextIndex = sessionState_->getTransitionState()->getPrevIndex();
			nextFrameIndex = sessionState_->getFrameState()->getPrevIndex();
			if(nextFrameIndex > nextIndex)
			{
				moveToFrame = true;
				nextIndex = nextFrameIndex;
			}
			if(onFrame_)
				nextTime = sessionState_->getFrameReader()->getPrevTime();
		}
		else
		{//Get next transition or frame, whatever comes first
			nextIndex = sessionState_->getTransitionState()->getNextIndex();
			nextFrameIndex = sessionState_->getFrameState()->getNextIndex();
			if(nextFrameIndex < nextIndex)
			{
				moveToFrame = true;
				nextIndex = nextFrameIndex;
				nextTime = sessionState_->getFrameReader()->getNextTime();
			}
		}
		if(!nextIndex)	//We reached the end/beginning of run for forward/backward
			return false;
	}
	if(backward)
	{	//When moving backward, just jump to the desired index for all DataStates
		//since order is irrelevant.  Just make sure to do the final state (frame or transition)
		//last so that all other levels will be correct first.
		QSharedPointer<DataState<qulonglong>> finalState;
		foreach(QSharedPointer<DataState<qulonglong>> dataState,sessionState_->getStatesIndexedById())
		{
			if(dataState->getPrevIndex() == nextIndex)
			{
				finalState = dataState;	//Will be the frame or transition state used to calculte nextIndex.
				continue;
			}
			dataState->setCurrentIndex(nextIndex);
		}
		if(onFrame_)
		{
			foreach(QSharedPointer<DataState<double>> dataState,sessionState_->getStatesIndexedByTime())
			{
				dataState->setCurrentIndex(nextTime);
			}
		}
		if(finalState)
			finalState->setCurrentIndex(nextIndex);
	}
	else
	{
		//Step through all id indexed data states in order of id index up to the nextIndex
		//without actually stepping that far.
		stepToIndex<qulonglong>(sessionState_->getStatesIndexedById(),nextIndex,false);

		//If the time is changing, step through all time indexed data states in time order
		//up to and including currTime (signal channel's allow values with time equal to
		//the first phosphor time to a frame).  Then step to the frame.
		if(moveToFrame)
		{
			stepToIndex<double>(sessionState_->getStatesIndexedByTime(),nextTime,true);
			sessionState_->getFrameState()->setCurrentIndex(nextTime);
		}
		else
		{	//Since time isn't changing, no need to update time indexed data states.
			//just update the transition state.
			sessionState_->getTransitionState()->setCurrentIndex(nextIndex);
		}
	}


	lastIndex_ = nextIndex;
	lastTime_ = nextTime;
	onFrame_ = moveToFrame;
	return true;
}

bool SessionPlayer::stepToTime(double time)
{
	if(time == lastTime_)
		return true;
	while(time > lastTime_)
	{
		if(!stepToNextFrame())
			return false;
	}
	while(time < lastTime_)
	{
		if(!stepToPrevFrame())
			return false;
	}
	return true;
}

void SessionPlayer::sessionStateReset()
{
	lastIndex_ = 0;
	lastTime_ = 0;
}