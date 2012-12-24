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
	data_.resize(query_->value(0).toInt());

	//Currently, we don't select properties with no parent (ie. Runtime parameters).
	query_->exec("SELECT f.time,f.dataid FROM frames f "
		"ORDER BY f.dataid");
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