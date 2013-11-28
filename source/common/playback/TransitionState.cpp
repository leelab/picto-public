#include "TransitionState.h"
#include "PlaybackData.h"
using namespace Picto;

void TransitionState::setDatabase(QSqlDatabase session)
{
	runStart_ = runEnd_ = curr_ = firstLocationInRun_ = -1;
	session_ = session;
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(session_));
	query_->exec("SELECT COUNT(*) FROM transitions");
	if(!query_->exec() || !query_->next())
	{
		Q_ASSERT(false);
		return;
	}
	data_.clear();
	data_.reserve(query_->value(0).toInt());
	for(int i=0;i<query_->value(0).toInt();i++)
	{
		data_.append(PlaybackTransData());
	}

	
	query_->exec("SELECT f.time,t.dataid,t.transid FROM transitions t, frames f "
		"WHERE f.dataid=t.frameid ORDER BY t.dataid");
	if(!query_->exec())
	{
		Q_ASSERT(false);
		return;
	}
	int transId;
	PlaybackIndex index;
	int arrayIndex = 0;
	while(query_->next())
	{
		transId = query_->value(2).toInt();
		if(obsoleteAssetIds_.contains(transId))
			continue;
		data_[arrayIndex++] = PlaybackTransData(PlaybackIndex(query_->value(0).toDouble(),query_->value(1).toLongLong()),
			transId);
	}
	if(arrayIndex < data_.size())
		data_.erase(data_.begin()+arrayIndex,data_.end());
}

void TransitionState::startRun(double runStartTime,double runEndTime)
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
	firstLocationInRun_ = curr_+1;
}

PlaybackIndex TransitionState::getCurrentIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0)
		return PlaybackIndex();
	return globalToRunIndex(data_[curr_].index_);
}

PlaybackIndex TransitionState::getNextIndex(double lookForwardTime)
{
	PlaybackIndex returnVal = getNextIndex();
	if(returnVal.time() > lookForwardTime)
		return PlaybackIndex();
	return returnVal;
}

void TransitionState::moveToIndex(PlaybackIndex index)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(index >= getCurrentIndex());
	PlaybackIndex nextIndex = getNextIndex();
	while(nextIndex.isValid() && nextIndex <= index)
	{
		curr_++;
		emit transitionActivated(data_[curr_].transId_);
		nextIndex = getNextIndex();
	}
}

/*! \brief Returns the Asset Id of the first Transition in the current run.
 */
int TransitionState::getFirstTransIdInRun()
{
	if(firstLocationInRun_ < 0 || firstLocationInRun_ >= data_.size())
		return 0;
	return data_[firstLocationInRun_].transId_;
}

PlaybackIndex TransitionState::getNextIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ >= data_.size()-1)
		return PlaybackIndex();
	return globalToRunIndex(data_[curr_+1].index_);
}

PlaybackIndex TransitionState::globalToRunIndex(PlaybackIndex index)
{
	PlaybackIndex returnVal = index;
	returnVal.time() -= runStart_;
	return returnVal;
}