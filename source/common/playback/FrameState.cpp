#include "FrameState.h"
#include "PlaybackData.h"
using namespace Picto;

void FrameState::setDatabase(QSqlDatabase session)
{
	runStart_ = runEnd_ = curr_ = -1;
	session_ = session;
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(session_));
	query_->exec("SELECT COUNT(*) FROM frames");
	if(!query_->exec() || !query_->next())
	{
		Q_ASSERT(false);
		return;
	}
	data_.clear();
	data_.reserve(query_->value(0).toInt());
	for(int i=0;i<query_->value(0).toInt();i++)
		data_.push_back(PlaybackIndex());

	//Currently, we don't select properties with no parent (ie. Runtime parameters).
	query_->exec("SELECT f.time,f.dataid FROM frames f "
		"ORDER BY f.dataid");
	if(!query_->exec())
	{
		Q_ASSERT(false);
		return;
	}

	int arrayIndex = 0;
	while(query_->next())
	{
		data_[arrayIndex++] = PlaybackIndex(query_->value(0).toDouble(),query_->value(1).toLongLong());
	}
	Q_ASSERT(data_.size() == arrayIndex);
}

void FrameState::startRun(double runStartTime,double runEndTime)
{
	Q_ASSERT(session_.isOpen());
	runStart_ = runStartTime;
	runEnd_ = runEndTime;
	//Move curr_ to first property in run
	curr_ = -1;
	PlaybackIndex nextIndex = getNextIndex();
	while(nextIndex.isValid() && nextIndex.time() < 0)
	{
		curr_++;
		nextIndex = getNextIndex();
	}
}

PlaybackIndex FrameState::getCurrentIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0)
		return PlaybackIndex();
	return globalToRunIndex(data_[curr_]);
}

PlaybackIndex FrameState::getNextIndex(double lookForwardTime)
{
	PlaybackIndex returnVal = getNextIndex();
	if(returnVal.time() > lookForwardTime)
		return PlaybackIndex();
	return returnVal;
}

void FrameState::moveToIndex(PlaybackIndex index)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(index >= getCurrentIndex());
	PlaybackIndex nextIndex = getNextIndex();
	while(nextIndex.isValid() && nextIndex <= index)
	{
		curr_++;
		emit framePresented(globalToRunIndex(data_[curr_]).time());
		nextIndex = getNextIndex();
	}
}

double FrameState::getPrevTime()
{
	if(curr_ < 1)
		return 0;
	PlaybackIndex prevIndex = globalToRunIndex(data_[curr_-1]);
	if(prevIndex.time() < 0)
		return 0;
	return prevIndex.time();
}
double FrameState::getLatestTime()
{
	if(curr_ < 0)
		return 0;
	return getCurrentIndex().time();
}
double FrameState::getNextTime()
{
	if(curr_ >= (data_.size()-1))
		return -1;
	PlaybackIndex currIndex = globalToRunIndex(data_[curr_+1]);
	return currIndex.time();
}
QVariantList FrameState::getTimesSince(double time)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(curr_ >= 0);
	double afterTime = time;
	if(afterTime >= getLatestTime())
		return QVariantList();
	PlaybackIndex beyondIndex = PlaybackIndex::maxForTime(afterTime+runStart_);
	QList<PlaybackIndex>::iterator iter = qUpperBound<QList<PlaybackIndex>::iterator,PlaybackIndex>(data_.begin(),curr_>=0?data_.begin()+curr_:data_.begin(),beyondIndex);
	QVariantList returnVal;
	for(;(iter <= data_.begin()+curr_) && (iter < data_.end());iter++)
	{
		returnVal.append(globalToRunIndex(*iter).time());
	}
	Q_ASSERT(iter != data_.end());
	return returnVal;
}

QVariantList FrameState::getTimesUntil(double time)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(curr_ >= 0);
	double upToTime = time;
	if(!data_.size() || upToTime <= getLatestTime())
		return QVariantList();
	PlaybackIndex beforeIndex = PlaybackIndex::maxForTime(upToTime+runStart_);
	QList<PlaybackIndex>::iterator upToIter = qUpperBound<QList<PlaybackIndex>::iterator,PlaybackIndex>(curr_>=0?data_.begin()+curr_:data_.begin(),data_.end(),beforeIndex);
	QVariantList returnVal;
	for(QList<PlaybackIndex>::iterator iter = data_.begin()+curr_+1;(iter < upToIter) && (iter < data_.end());iter++)
	{
		returnVal.append(globalToRunIndex(*iter).time());
	}
	return returnVal;
}

PlaybackIndex FrameState::getNextIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ >= data_.size()-1)
		return PlaybackIndex();
	return globalToRunIndex(data_[curr_+1]);
}

PlaybackIndex FrameState::globalToRunIndex(PlaybackIndex index)
{
	PlaybackIndex returnVal = index;
	returnVal.time() -= runStart_;
	return returnVal;
}