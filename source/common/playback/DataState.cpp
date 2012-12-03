#include <QMutexLocker>
#include <QThread>
#include "DataState.h"
using namespace Picto;			

//Template function definitions must be in header file-------------------------------------------
DataState::DataState(bool moveByIterating) :
iterateOnMove_(moveByIterating),
bufferTime_(3600)
{
	mutex_ = QSharedPointer<QMutex>(new QMutex(QMutex::NonRecursive));
	reset();
}

DataState::~DataState()
{

}

void DataState::reset()
{
	QMutexLocker locker(mutex_.data());
	pbDataList_.clear();
	maxIndex_ = PlaybackIndex();
	minIndex_ = PlaybackIndex();
	finishedLoading_ = false;
	waitForData_ = true;
}

void DataState::setBoundTimes(double minTime,double maxTime)
{
	QMutexLocker locker(mutex_.data());
	PlaybackIndex newMax = PlaybackIndex::maxForTime(maxTime);
	PlaybackIndex newMin = PlaybackIndex::minForTime(minTime);
	setBoundIndex(newMax);
	setBoundIndex(newMin);
}

void DataState::setFinishedLoading()
{
	finishedLoading_ = true;
}

bool DataState::setCurrentIndex(PlaybackIndex index)
{
	QMutexLocker locker(mutex_.data());
	waitForSufficientData(&locker,index);
	if(index == pbDataList_.currIndex())
	{
		return true;
	}

	//Check if index implies a single cell move.  If so, move.
	if(index > pbDataList_.currIndex())
	{
		PlaybackIndex secondIndex =  pbDataList_.next2Index();
		if(secondIndex.isValid() && (secondIndex > index))
		{							
			if(pbDataList_.nextIndex() <= index)
				moveToNext(true);
			//If we got here, the current index is the closest <= to the input so we're done.
			return true;
		}
	}
	else if(pbDataList_.currIndex().isValid())
	{
		Q_ASSERT(false);
	}

	//Move to index.
	if(iterateOnMove_)
	{	
		PlaybackIndex nextInd = pbDataList_.nextIndex();
		while(nextInd.isValid() && nextInd <= index)
		{
			moveToNext(nextInd >= index);
			nextInd = pbDataList_.nextIndex();
		}
		if(pbDataList_.currIndex() <= index)
			return true;
	}
	else
	{
		//go to cell with greatest index less than or equal to index
		PlaybackIndex currIndex = pbDataList_.currIndex();
		if(!(currIndex == pbDataList_.moveToCell(index)))
		{
			triggerChange(true);
			return true;
		}
	}
	return false;
}

QSharedPointer<IndexedData> DataState::getCurrentValue()
{
	QMutexLocker locker(mutex_.data());
	return pbDataList_.currValue();
}

QSharedPointer<IndexedData> DataState::getNextValue(double lookForwardTime)
{
	QMutexLocker locker(mutex_.data());
	waitForSufficientData(&locker,PlaybackIndex::maxForTime(lookForwardTime));
	return pbDataList_.nextValue();
}

QList<QSharedPointer<IndexedData>> DataState::getValuesSince(double time)
{
	QMutexLocker locker(mutex_.data());
	return pbDataList_.getValuesSince(time);
}

QList<QSharedPointer<IndexedData>> DataState::getValuesUntil(double time)
{
	QMutexLocker locker(mutex_.data());
	waitForSufficientData(&locker,PlaybackIndex::maxForTime(time));
	return pbDataList_.getValuesSince(time);
}

PlaybackIndex DataState::getCurrentIndex()
{
	QMutexLocker locker(mutex_.data());
	return pbDataList_.currIndex();
}

PlaybackIndex DataState::getNextIndex(double lookForwardTime)
{
	QMutexLocker locker(mutex_.data());
	waitForSufficientData(&locker,PlaybackIndex::maxForTime(lookForwardTime));
	return pbDataList_.nextIndex();
}

PlaybackIndex DataState::getFirstIndex()
{
	QMutexLocker locker(mutex_.data());
	return pbDataList_.firstIndex();
}

PlaybackIndex DataState::getLastIndex()
{
	QMutexLocker locker(mutex_.data());
	return pbDataList_.lastIndex();
}

void DataState::setValue(QSharedPointer<IndexedData> pbData)
{
	QMutexLocker locker(mutex_.data());
	//If the Index's pos value was not set yet, set it appropriately.
	if(!pbData->index_.isValid())
	{
		if(pbData->index_.time() == pbDataList_.lastIndex().time())
			pbData->index_.pos() = pbDataList_.lastIndex().pos()+1;
		else if(pbData->index_.time() == pbDataList_.firstIndex().time())
			pbData->index_.pos() = pbDataList_.firstIndex().pos()-1;
		else //If this is data for a new time, just set its pos to the center of the pos range.
			pbData->index_.pos() = 0x7FFFFFFFFFFFFFFF;
	}

	Q_ASSERT(pbData->index_ > pbDataList_.lastIndex());
	//Add it to the end of the list
	pbDataList_.append(pbData);
	setBoundIndex(pbData->index_);
}

void DataState::shouldWaitForData(bool wait)
{
	QMutexLocker locker(mutex_.data());
	waitForData_ = wait;
}

//Functions below clear all IndexedData before/after the input index for memory savings.
void DataState::clearDataBefore(double time)
{
	QMutexLocker locker(mutex_.data());
	pbDataList_.clearBefore(time);
	minIndex_ = pbDataList_.firstIndex();
}

void DataState::setBoundIndex(PlaybackIndex index)
{
	if(!getMinIndex().isValid())
	{//Bound values were not yet initialized.
		maxIndex_ = index;
		minIndex_ = index;
		return;
	}
	if(index > getMaxIndex())
	{
		maxIndex_ = index;
		return;
	}
	if(index < getMinIndex())
	{
		minIndex_ = index;
	}
}

PlaybackIndex DataState::getMaxIndex()
{
	return maxIndex_;
}

PlaybackIndex DataState::getMinIndex()
{
	return minIndex_;	
}

bool DataState::moveToNext(bool last)
{
	if(!pbDataList_.nextValue())
		return false;
	pbDataList_.moveToNext();
	triggerChange(last);
	return true;
}

bool DataState::triggerChange(bool last)
{
	mutex_->unlock();	//Since sub classes may call functions that lock the mutex, release it for
						//this call.
	triggerValueChange(false,last);
	mutex_->lock();
	return true;
}

PlaybackIndex DataState::get2NextIndex()
{
	if(!pbDataList_.next2Value())
	{
		return PlaybackIndex();
	}
	return pbDataList_.next2Value()->index_;
}

bool DataState::inDataWindow(PlaybackIndex index)
{
	if((index >= minIndex_) && ((index <= maxIndex_) || finishedLoading_))
		return true;
	return false;
}
	
void DataState::waitForSufficientData(QMutexLocker* mutexLocker,PlaybackIndex dataIndex)
{
	while(waitForData_ && !inDataWindow(dataIndex))
	{
		mutexLocker->unlock();
		QThread::yieldCurrentThread();
		mutexLocker->relock();
	}											
}