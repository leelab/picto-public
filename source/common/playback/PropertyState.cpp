#include "PropertyState.h"
#include "PlaybackData.h"
using namespace Picto;

void PropertyState::setDatabase(QSqlDatabase session)
{
	runStart_ = runEnd_ = curr_ = -1;
	session_ = session;
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(session_));
	query_->exec("SELECT COUNT(*) FROM properties");
	if(!query_->exec() || !query_->next())
	{
		Q_ASSERT(false);
		return;
	}
	data_.resize(query_->value(0).toInt());

	//Currently, we don't select properties with no parent (ie. Runtime parameters).
	query_->exec("SELECT f.time,p.dataid,p.assetid,p.value FROM properties p, propertylookup pl,frames f "
		"WHERE p.assetid=pl.assetid AND pl.parent <> 0 AND f.dataid=p.frameid ORDER BY p.dataid");
	if(!query_->exec())
	{
		Q_ASSERT(false);
		return;
	}
	int assetId;
	PlaybackIndex index;
	QString val;
	int arrayIndex = 0;
	while(query_->next())
	{
		assetId = query_->value(2).toInt();
		if(obsoleteAssetIds_.contains(assetId))
			continue;
		val = query_->value(3).toString();
		data_[arrayIndex++] = PropertyData(PlaybackIndex(query_->value(0).toDouble(),query_->value(1).toLongLong()),
			assetId,query_->value(3).toString());
	}
	if(arrayIndex < data_.size())
		data_.resize(arrayIndex);
}

void PropertyState::startRun(double runStartTime,double runEndTime)
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

PlaybackIndex PropertyState::getCurrentIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0)
		return PlaybackIndex();
	return globalToRunIndex(data_[curr_].index_);
}

PlaybackIndex PropertyState::getNextIndex(double lookForwardTime)
{
	PlaybackIndex returnVal = getNextIndex();
	if(returnVal.time() > lookForwardTime)
		return PlaybackIndex();
	return returnVal;
}

void PropertyState::moveToIndex(PlaybackIndex index)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(index >= getCurrentIndex());
	PlaybackIndex nextIndex = getNextIndex();
	while(nextIndex.isValid() && nextIndex <= index)
	{
		curr_++;
		emit propertyChanged(data_[curr_].assetid_,data_[curr_].val_);
		nextIndex = getNextIndex();
	}
}

PlaybackIndex PropertyState::getNextIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ >= data_.size()-1)
		return PlaybackIndex();
	return globalToRunIndex(data_[curr_+1].index_);
}

PlaybackIndex PropertyState::globalToRunIndex(PlaybackIndex index)
{
	PlaybackIndex returnVal = index;
	returnVal.time() -= runStart_;
	return returnVal;
}