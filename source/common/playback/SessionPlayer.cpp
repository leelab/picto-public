#include "SessionPlayer.h"
using namespace Picto;

SessionPlayer::SessionPlayer(QSharedPointer<SessionState> sessState,QSharedPointer<SessionLoader> sessLoader)
:
sessionState_(sessState),
sessionLoader_(sessLoader),
processing_(false),
reachedEnd_(false)
{
	lastIndex_ = PlaybackIndex::minForTime(0);
}

SessionPlayer::~SessionPlayer()
{
}

void SessionPlayer::restart()
{
	sessionLoader_->restart();
	lastIndex_ = PlaybackIndex::minForTime(0);
	reachedEnd_ = false;
}

bool SessionPlayer::stepToTime(double time)
{
	qDebug(QString("Player: Step To Time called with input: %1").arg(time).toAscii());
	if(time < 0)
		return false;
	if(processing_) return false;
	if(time < lastIndex_.time())
	{
		//If time goes down, restart from the beginning of the run and step back to it.
		restart();
	}
	if(time == lastIndex_.time())
		return true;
	if(reachedEnd_)
		return true;
	if(!sessionLoader_->setCurrentTime(time))
	{
		qDebug(QString("Player: Failed to set time to Loader").toAscii());
		return false;
	}
	if(time > sessionLoader_->getMaxBehavTime() && time < sessionLoader_->runDuration())
	{
		qDebug(QString("Player: Time was too high, loader hasn't caught up yet").toAscii());
		return false;
	}

	//Step to the input time or the end of the run. Whichever comes first.
	while(time > lastIndex_.time())
	{
		if(!stepToNextFrame(time))
		{
			if(time < sessionLoader_->runDuration())
			{
				qDebug(QString("Player: Could not step to next frame").toAscii());
				return false;
			}
			reachedEnd_ = true;
			emit reachedEnd();
			break;
		}
	}
	sessionLoader_->setProcessedTime(getTime());
	qDebug(QString("Player: Step To Time reached time: %1").arg(getTime()).toAscii());
	return true;
}


double SessionPlayer::getTime()
{
	return lastIndex_.time();
}

bool SessionPlayer::isProcessing()
{
	return processing_;
}

bool SessionPlayer::stepForward(double lookForward)
{
	if(processing_) return false;
	return step(lookForward);
}

bool SessionPlayer::stepToNextFrame(double lookForward)
{
	if(processing_) return false;
	PlaybackIndex nextFrameId = sessionState_->getFrameState()->getNextIndex(lookForward);
	if(!nextFrameId.isValid())
		return false;
	while(lastIndex_ < nextFrameId)
		stepForward(lookForward);
	return true;
}

bool SessionPlayer::step(double lookForward)
{
	processing_ = true;
	PlaybackIndex currFrameId = sessionState_->getFrameState()->getCurrentIndex();
	PlaybackIndex nextFrameId = sessionState_->getFrameState()->getNextIndex(lookForward);
	QSharedPointer<DataState> stateToTrigger = getNextTriggerState(lookForward);
	if(stateToTrigger.isNull())
	{
		processing_ = false;
		return false;
	}
	if	(stateToTrigger->getNextIndex(nextFrameId.time()) == nextFrameId)
	{	//If the next stateToTrigger is a frame, step to all time based data states first (ie. reward, signals, spike, lfp).
		PlaybackIndex frameIndexForTime = nextFrameId;
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
	stateToTrigger->setCurrentIndex(stateToTrigger->getNextIndex(nextFrameId.time()));
	lastIndex_ = stateToTrigger->getCurrentIndex();
	processing_ = false;
	return true;
}

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
	int i=0;
	do
	{
		nBuffer = i;
		indexBuffer = idIndexedStates[i++]->getNextIndex(boundaryTime);
	} while(!indexBuffer.isValid() && i < idIndexedStates.size());
	
	//Get the state with the lowest index
	for(;i<idIndexedStates.size();i++)
	{
		iterationIndex = idIndexedStates[i]->getNextIndex(boundaryTime);
		if	(	iterationIndex.isValid()
				&&	(iterationIndex < indexBuffer)
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