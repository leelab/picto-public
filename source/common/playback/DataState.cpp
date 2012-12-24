#include "DataState.h"
using namespace Picto;



AutoDataState::AutoDataState()
{
	runStartTime_ = runEndTime_ = lastQueryEndTime_ = -1;
	dataIndex_ = -2;
}
AutoDataState::~AutoDataState()
{}

void AutoDataState::setDatabase(QSqlDatabase session)
{
	session_ = session;
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(session_));
	reset();
}

void AutoDataState::startRun(double runStartTime,double runEndTime)
{
	Q_ASSERT(runStartTime != runStartTime_);
	runStartTime_ = runStartTime;
	runEndTime_ = runEndTime;
	reset();
}

PlaybackIndex AutoDataState::getCurrentIndex()
{
	if(dataIndex_ < -1)
	{
		//Query Database and put dataIndex_ at -1!
		querySessionForData(query_,lastQueryEndTime_,runEndTime_<0?1000000000:runEndTime_);
		lastQueryEndTime_ = runEndTime_<0?-1:runEndTime_;
		if(!query_->next() || query_->at() == QSql::AfterLastRow)
			return PlaybackIndex();
		currQueryData_ = convertQueryToDataVector(query_,runStartTime_);
		if(!currQueryData_.size())
			return PlaybackIndex();
		if(query_->next() && query_->at() != QSql::AfterLastRow)
			nextQueryData_ = convertQueryToDataVector(query_,runStartTime_);
		dataIndex_ = -1;
	}
	if(dataIndex_ < 0)
		return PlaybackIndex();
	return currQueryData_[dataIndex_]->index_;
}

PlaybackIndex AutoDataState::getNextIndex(double lookForwardTime)
{
	if(dataIndex_ < -1)
	{
		querySessionForData(query_,lastQueryEndTime_,runEndTime_<0?1000000000:runEndTime_);
		lastQueryEndTime_ = runEndTime_<0?-1:runEndTime_;
		if(!query_->next() || query_->at() == QSql::AfterLastRow)
			return PlaybackIndex();
		currQueryData_ = convertQueryToDataVector(query_,runStartTime_);
		if(!currQueryData_.size())
			return PlaybackIndex();
		if(query_->next() && query_->at() != QSql::AfterLastRow)
			nextQueryData_ = convertQueryToDataVector(query_,runStartTime_);
		dataIndex_ = -1;
	}
	if(dataIndex_ < currQueryData_.size()-1)
	{
		if(currQueryData_[dataIndex_+1]->index_.time() <= lookForwardTime)
			return currQueryData_[dataIndex_+1]->index_;
		return PlaybackIndex();
	}
	while(!nextQueryData_.size())
	{
		if(!query_->next() || query_->at() == QSql::AfterLastRow)
		{
			//Query Database starting after currentIndex!
			if(lastQueryEndTime_ < 0)
				lastQueryEndTime_ = getCurrentIndex().time()+runStartTime_;
			if(lastQueryEndTime_ >= runEndTime_)
				return PlaybackIndex();
			querySessionForData(query_,lastQueryEndTime_,runEndTime_<0?1000000000:runEndTime_);
			lastQueryEndTime_ = runEndTime_<0?-1:runEndTime_;
			if(!query_->next() || query_->at() == QSql::AfterLastRow)
			{
				query_->previous();
				return PlaybackIndex();
			}
		}
		nextQueryData_ = convertQueryToDataVector(query_,runStartTime_);
	}
	if(nextQueryData_[0]->index_.time() <= lookForwardTime)
		return nextQueryData_[0]->index_;
	return PlaybackIndex();
}

void AutoDataState::moveToIndex(PlaybackIndex index)
{
	PlaybackIndex curr = getCurrentIndex();
	if(index == curr)
		return;
	Q_ASSERT(index >= curr);
	PlaybackIndex next = getNextIndex(index.time());
	while(next.isValid() && next <= index)
	{
		if(dataIndex_ < currQueryData_.size()-1)
		{
			dataIndex_++;
		}
		else
		{
			currQueryData_ = nextQueryData_;
			nextQueryData_.clear();
			dataIndex_ = 0;
		}
		curr = getCurrentIndex();
		next = getNextIndex(index.time());
		triggerDataChange(getCurrentValue());
	}
}

QSharedPointer<IndexedData> AutoDataState::getCurrentValue()
{
	if(dataIndex_ < -1)
	{
		//Query Database and put dataIndex_ at -1!
		querySessionForData(query_,lastQueryEndTime_,runEndTime_<0?1000000000:runEndTime_);
		lastQueryEndTime_ = runEndTime_<0?-1:runEndTime_;
		if(!query_->next() || query_->at() == QSql::AfterLastRow)
			return QSharedPointer<IndexedData>();
		currQueryData_ = convertQueryToDataVector(query_,runStartTime_);
		if(!currQueryData_.size())
			return QSharedPointer<IndexedData>();
		if(query_->next() && query_->at() != QSql::AfterLastRow)
			nextQueryData_ = convertQueryToDataVector(query_,runStartTime_);
		dataIndex_ = -1;
	}
	if(dataIndex_ < 0)
		return QSharedPointer<IndexedData>();
	return currQueryData_[dataIndex_];
}

void AutoDataState::reset()
{
	query_->finish();
	dataIndex_ = -2;
	lastQueryEndTime_ = runStartTime_-0.0000001;
}







//#include <QMutexLocker>
//#include <QThread>
//#include "DataState.h"
//using namespace Picto;			
//
////Template function definitions must be in header file-------------------------------------------
//DataState::AutoDataState(bool moveByIterating) :
//iterateOnMove_(moveByIterating),
//bufferTime_(3600)
//{
//	mutex_ = QSharedPointer<QMutex>(new QMutex(QMutex::NonRecursive));
//	reset();
//}
//
//DataState::~DataState()
//{
//
//}
//
//void AutoDataState::reset()
//{
//	QMutexLocker locker(mutex_.data());
//	pbDataList_.clear();
//	maxIndex_ = PlaybackIndex();
//	minIndex_ = PlaybackIndex();
//	finishedLoading_ = false;
//	waitForData_ = true;
//}
//
//void AutoDataState::setBoundTimes(double minTime,double maxTime)
//{
//	QMutexLocker locker(mutex_.data());
//	PlaybackIndex newMax = PlaybackIndex::maxForTime(maxTime);
//	PlaybackIndex newMin = PlaybackIndex::minForTime(minTime);
//	setBoundIndex(newMax);
//	setBoundIndex(newMin);
//}
//
//void AutoDataState::setFinishedLoading()
//{
//	finishedLoading_ = true;
//}
//
//bool AutoDataState::setCurrentIndex(PlaybackIndex index)
//{
//	QMutexLocker locker(mutex_.data());
//	waitForSufficientData(&locker,index);
//	if(index == pbDataList_.currIndex())
//	{
//		return true;
//	}
//
//	//Check if index implies a single cell move.  If so, move.
//	if(index > pbDataList_.currIndex())
//	{
//		PlaybackIndex secondIndex =  pbDataList_.next2Index();
//		if(secondIndex.isValid() && (secondIndex > index))
//		{							
//			if(pbDataList_.nextIndex() <= index)
//				moveToNext(true);
//			//If we got here, the current index is the closest <= to the input so we're done.
//			return true;
//		}
//	}
//	else if(pbDataList_.currIndex().isValid())
//	{
//		Q_ASSERT(false);
//	}
//
//	//Move to index.
//	if(iterateOnMove_)
//	{	
//		PlaybackIndex nextInd = pbDataList_.nextIndex();
//		while(nextInd.isValid() && nextInd <= index)
//		{
//			moveToNext(nextInd >= index);
//			nextInd = pbDataList_.nextIndex();
//		}
//		if(pbDataList_.currIndex() <= index)
//			return true;
//	}
//	else
//	{
//		//go to cell with greatest index less than or equal to index
//		PlaybackIndex currIndex = pbDataList_.currIndex();
//		if(!(currIndex == pbDataList_.moveToCell(index)))
//		{
//			triggerChange(true);
//			return true;
//		}
//	}
//	return false;
//}
//
//QSharedPointer<IndexedData> AutoDataState::getCurrentValue()
//{
//	QMutexLocker locker(mutex_.data());
//	return pbDataList_.currValue();
//}
//
//QSharedPointer<IndexedData> AutoDataState::getNextValue(double lookForwardTime)
//{
//	QMutexLocker locker(mutex_.data());
//	waitForSufficientData(&locker,PlaybackIndex::maxForTime(lookForwardTime));
//	return pbDataList_.nextValue();
//}
//
//QList<QSharedPointer<IndexedData>> AutoDataState::getValuesSince(double time)
//{
//	QMutexLocker locker(mutex_.data());
//	return pbDataList_.getValuesSince(time);
//}
//
//QList<QSharedPointer<IndexedData>> AutoDataState::getValuesUntil(double time)
//{
//	QMutexLocker locker(mutex_.data());
//	waitForSufficientData(&locker,PlaybackIndex::maxForTime(time));
//	return pbDataList_.getValuesSince(time);
//}
//
//PlaybackIndex AutoDataState::getCurrentIndex()
//{
//	QMutexLocker locker(mutex_.data());
//	return pbDataList_.currIndex();
//}
//
//PlaybackIndex AutoDataState::getNextIndex(double lookForwardTime)
//{
//	QMutexLocker locker(mutex_.data());
//	waitForSufficientData(&locker,PlaybackIndex::maxForTime(lookForwardTime));
//	return pbDataList_.nextIndex();
//}
//
//PlaybackIndex AutoDataState::getFirstIndex()
//{
//	QMutexLocker locker(mutex_.data());
//	return pbDataList_.firstIndex();
//}
//
//PlaybackIndex AutoDataState::getLastIndex()
//{
//	QMutexLocker locker(mutex_.data());
//	return pbDataList_.lastIndex();
//}
//
//void AutoDataState::setValue(QSharedPointer<IndexedData> pbData)
//{
//	QMutexLocker locker(mutex_.data());
//	//If the Index's pos value was not set yet, set it appropriately.
//	if(!pbData->index_.isValid())
//	{
//		if(pbData->index_.time() == pbDataList_.lastIndex().time())
//			pbData->index_.pos() = pbDataList_.lastIndex().pos()+1;
//		else if(pbData->index_.time() == pbDataList_.firstIndex().time())
//			pbData->index_.pos() = pbDataList_.firstIndex().pos()-1;
//		else //If this is data for a new time, just set its pos to the center of the pos range.
//			pbData->index_.pos() = 0x7FFFFFFFFFFFFFFF;
//	}
//
//	Q_ASSERT(pbData->index_ > pbDataList_.lastIndex());
//	//Add it to the end of the list
//	pbDataList_.append(pbData);
//	setBoundIndex(pbData->index_);
//}
//
//void AutoDataState::shouldWaitForData(bool wait)
//{
//	QMutexLocker locker(mutex_.data());
//	waitForData_ = wait;
//}
//
////Functions below clear all IndexedData before/after the input index for memory savings.
//void AutoDataState::clearDataBefore(double time)
//{
//	QMutexLocker locker(mutex_.data());
//	pbDataList_.clearBefore(time);
//	minIndex_ = pbDataList_.firstIndex();
//}
//
//void AutoDataState::setBoundIndex(PlaybackIndex index)
//{
//	if(!getMinIndex().isValid())
//	{//Bound values were not yet initialized.
//		maxIndex_ = index;
//		minIndex_ = index;
//		return;
//	}
//	if(index > getMaxIndex())
//	{
//		maxIndex_ = index;
//		return;
//	}
//	if(index < getMinIndex())
//	{
//		minIndex_ = index;
//	}
//}
//
//PlaybackIndex AutoDataState::getMaxIndex()
//{
//	return maxIndex_;
//}
//
//PlaybackIndex AutoDataState::getMinIndex()
//{
//	return minIndex_;	
//}
//
//bool AutoDataState::moveToNext(bool last)
//{
//	if(!pbDataList_.nextValue())
//		return false;
//	pbDataList_.moveToNext();
//	triggerChange(last);
//	return true;
//}
//
//bool AutoDataState::triggerChange(bool last)
//{
//	mutex_->unlock();	//Since sub classes may call functions that lock the mutex, release it for
//						//this call.
//	triggerValueChange(false,last);
//	mutex_->lock();
//	return true;
//}
//
//PlaybackIndex AutoDataState::get2NextIndex()
//{
//	if(!pbDataList_.next2Value())
//	{
//		return PlaybackIndex();
//	}
//	return pbDataList_.next2Value()->index_;
//}
//
//bool AutoDataState::inDataWindow(PlaybackIndex index)
//{
//	if((index >= minIndex_) && ((index <= maxIndex_) || finishedLoading_))
//		return true;
//	return false;
//}
//	
//void AutoDataState::waitForSufficientData(QMutexLocker* mutexLocker,PlaybackIndex dataIndex)
//{
//	while(waitForData_ && !inDataWindow(dataIndex))
//	{
//		mutexLocker->unlock();
//		QThread::yieldCurrentThread();
//		mutexLocker->relock();
//	}											
//}