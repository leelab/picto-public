#include "RewardState.h"
#include "PlaybackData.h"
using namespace Picto;

void RewardState::setDatabase(QSqlDatabase session)
{
	runStart_ = runEnd_ = curr_ = -1;
	session_ = session;
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(session_));
	query_->exec("SELECT COUNT(*) FROM rewards");
	if(!query_->exec() || !query_->next())
	{
		Q_ASSERT(false);
		return;
	}
	data_.resize(query_->value(0).toInt());

	query_->exec("SELECT r.time,r.dataid,r.duration,r.channel FROM rewards r "
		"ORDER BY r.dataid");
	if(!query_->exec())
	{
		Q_ASSERT(false);
		return;
	}

	double duration;
	int channel;
	int arrayIndex = 0;
	while(query_->next())
	{
		data_[arrayIndex++] = PlaybackRewardData(PlaybackIndex(query_->value(0).toDouble(),query_->value(1).toLongLong()),
			query_->value(2).toInt(),query_->value(3).toInt());
	}
	Q_ASSERT(data_.size() == arrayIndex);
}

void RewardState::startRun(double runStartTime,double runEndTime)
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

PlaybackIndex RewardState::getCurrentIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0 || curr_ >= data_.size())
		return PlaybackIndex();
	return globalToRunIndex(data_[curr_].index_);
}

PlaybackIndex RewardState::getNextIndex(double lookForwardTime)
{
	PlaybackIndex returnVal = getNextIndex();
	if(returnVal.time() > lookForwardTime)
		return PlaybackIndex();
	return returnVal;
}

void RewardState::moveToIndex(PlaybackIndex index)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(index >= getCurrentIndex());
	PlaybackIndex nextIndex = getNextIndex();
	while(nextIndex.isValid() && nextIndex <= index)
	{
		curr_++;
		emit rewardSupplied(globalToRunIndex(data_[curr_].index_).time(),data_[curr_].duration_,data_[curr_].channel_);
		nextIndex = getNextIndex();
	}
}

double RewardState::getLatestTime()
{
	if(curr_ < 0)
		return 0;
	return getCurrentIndex().time();
}
double RewardState::getNextTime()
{
	if(curr_ >= (data_.size()-1))
		return -1;
	PlaybackIndex currIndex = globalToRunIndex(data_[curr_+1].index_);
	return currIndex.time();
}
int RewardState::getLatestDuration()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0)
		return -1;
	return data_[curr_].duration_;
}
int RewardState::getNextDuration()
{
	if(curr_ >= (data_.size()-1))
		return -1;
	data_[curr_+1].duration_;
}
QVariantList RewardState::getTimesSince(double time)
{
	Q_ASSERT(runStart_ >= 0);
	double afterTime = time;
	if(afterTime >= getLatestTime())
		return QVariantList();
	PlaybackIndex beyondIndex = PlaybackIndex::maxForTime(afterTime+runStart_);
	QVector<PlaybackRewardData>::iterator iter = qUpperBound<QVector<PlaybackRewardData>::iterator,PlaybackRewardData>(data_.begin(),data_.begin()+curr_,PlaybackRewardData(beyondIndex,0,0));
	QVariantList returnVal;
	for(;(iter <= data_.begin()+curr_) && (iter < data_.end());iter++)
	{
		returnVal.append(globalToRunIndex(iter->index_).time());
	}
	return returnVal;
}

QVariantList RewardState::getTimesUntil(double time)
{
	Q_ASSERT(runStart_ >= 0);
	double upToTime = time;
	if(upToTime <= getLatestTime())
		return QVariantList();
	PlaybackIndex beforeIndex = PlaybackIndex::maxForTime(upToTime+runStart_);
	QVector<PlaybackRewardData>::iterator upToIter = qUpperBound<QVector<PlaybackRewardData>::iterator,PlaybackRewardData>(data_.begin()+curr_,data_.end(),PlaybackRewardData(beforeIndex,0,0));
	QVariantList returnVal;
	for(QVector<PlaybackRewardData>::iterator iter = data_.begin()+curr_+1;(iter < upToIter) && (iter < data_.end());iter++)
	{
		returnVal.append(globalToRunIndex(iter->index_).time());
	}
	return returnVal;
}
  
QVariantList RewardState::getDurationsSince(double time)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(curr_ >= 0);
	double afterTime = time;
	if(afterTime >= getLatestTime())
		return QVariantList();
	PlaybackIndex beyondIndex = PlaybackIndex::maxForTime(afterTime+runStart_);
	QVector<PlaybackRewardData>::iterator iter = qUpperBound<QVector<PlaybackRewardData>::iterator,PlaybackRewardData>(data_.begin(),data_.begin()+curr_,PlaybackRewardData(beyondIndex,0,0));
	QVariantList returnVal;
	for(;(iter <= data_.begin()+curr_) && (iter < data_.end());iter++)
	{
		returnVal.append(iter->duration_);
	}
	return returnVal;
}
  
QVariantList RewardState::getDurationsUntil(double time)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(curr_ >= 0);
	double upToTime = time;
	if(upToTime <= getLatestTime())
		return QVariantList();
	PlaybackIndex beforeIndex = PlaybackIndex::maxForTime(upToTime+runStart_);
	QVector<PlaybackRewardData>::iterator upToIter = qUpperBound<QVector<PlaybackRewardData>::iterator,PlaybackRewardData>(data_.begin()+curr_,data_.end(),PlaybackRewardData(beforeIndex,0,0));
	QVariantList returnVal;
	for(QVector<PlaybackRewardData>::iterator iter = data_.begin()+curr_+1;(iter < upToIter) && (iter < data_.end());iter++)
	{
		returnVal.append(iter->duration_);
	}
	return returnVal;
}

PlaybackIndex RewardState::getNextIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ >= data_.size()-1)
		return PlaybackIndex();
	return globalToRunIndex(data_[curr_+1].index_);
}

PlaybackIndex RewardState::globalToRunIndex(PlaybackIndex index)
{
	PlaybackIndex returnVal = index;
	returnVal.time() -= runStart_;
	return returnVal;
}