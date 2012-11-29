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
	currentDataCell_ = -1;
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
	if(index == getCurrentIndexPriv())
	{
		return true;
	}

	//Check if index implies a single cell move.  If so, move.
	if(index > getCurrentIndexPriv())
	{
		PlaybackIndex secondIndex = get2NextIndex();
		if(secondIndex.isValid() && (secondIndex > index))
		{							
			if(getNextIndexPriv() <= index)
				moveToNext(true);
			//If we got here, the current index is the closest <= to the input so we're done.
			return true;
		}
	}
	else
	{
		if(getPrevIndexPriv() <= index)
		{
			moveToPrev(true);
			//If we got here, the current index is the closest <= to the input so we're done.
			return true;
		}
	}

	//Move to index.
	if(iterateOnMove_)
	{	
		PlaybackIndex nextInd = getNextIndexPriv();
		while(nextInd.isValid() && nextInd <= index)
		{
			moveToNext(nextInd >= index);
			nextInd = getNextIndexPriv();
		}
		while(getCurrentIndexPriv() > index && getPrevIndexPriv().isValid())
			moveToPrev(getPrevIndexPriv() <= index);
		if(getCurrentIndexPriv() <= index)
			return true;
	}
	else
	{
		//Search for cell with greatest index less than or equal to index
		int cellId = findIndexCell(index);
		if(cellId >= 0)
		{
			moveToCell(cellId,true);
			return true;
		}
	}
	return false;
}

QSharedPointer<IndexedData> DataState::getCurrentValue()
{
	QMutexLocker locker(mutex_.data());
	return getCurrentValuePriv();
}

QSharedPointer<IndexedData> DataState::getPrevValue(double lookBackTime)
{
	QMutexLocker locker(mutex_.data());
	waitForSufficientData(&locker,PlaybackIndex::minForTime(lookBackTime));
	return getPrevValuePriv();
}

QSharedPointer<IndexedData> DataState::getNextValue(double lookForwardTime)
{
	QMutexLocker locker(mutex_.data());
	waitForSufficientData(&locker,PlaybackIndex::maxForTime(lookForwardTime));
	return getNextValuePriv();
}

QList<QSharedPointer<IndexedData>> DataState::getValuesSince(double time)
{
	QMutexLocker locker(mutex_.data());
	return getValuesSincePriv(time);
}

QList<QSharedPointer<IndexedData>> DataState::getValuesUntil(double time)
{
	QMutexLocker locker(mutex_.data());
	waitForSufficientData(&locker,PlaybackIndex::maxForTime(time));
	return getValuesUntilPriv(time);
}

PlaybackIndex DataState::getCurrentIndex()
{
	QMutexLocker locker(mutex_.data());
	return getCurrentIndexPriv();
}

PlaybackIndex DataState::getPrevIndex(double lookBackTime)
{
	QMutexLocker locker(mutex_.data());
	waitForSufficientData(&locker,PlaybackIndex::minForTime(lookBackTime));
	return getPrevIndexPriv();
}

PlaybackIndex DataState::getNextIndex(double lookForwardTime)
{
	QMutexLocker locker(mutex_.data());
	waitForSufficientData(&locker,PlaybackIndex::maxForTime(lookForwardTime));
	return getNextIndexPriv();
}

PlaybackIndex DataState::getFirstIndex()
{
	QMutexLocker locker(mutex_.data());
	return getFirstIndexPriv();
}

PlaybackIndex DataState::getLastIndex()
{
	QMutexLocker locker(mutex_.data());
	return getLastIndexPriv();
}

void DataState::setValue(QSharedPointer<IndexedData> pbData)
{
	QMutexLocker locker(mutex_.data());
	//If the Index's pos value was not set yet, set it appropriately.
	if(!pbData->index_.isValid())
	{
		if(pbData->index_.time() == getLastIndexPriv().time())
			pbData->index_.pos() = getLastIndexPriv().pos()+1;
		else if(pbData->index_.time() == getFirstIndexPriv().time())
			pbData->index_.pos() = getFirstIndexPriv().pos()-1;
		else //If this is data for a new time, just set its pos to the center of the pos range.
			pbData->index_.pos() = 0x7FFFFFFFFFFFFFFF;
	}

	if(pbData->index_ > getLastIndexPriv())
	{
		//Add it to the end of the list
		pbDataList_.append(pbData);
	}
	else if(pbData->index_ < getFirstIndexPriv())
	{
		//Add it to the beginning of the list
		pbDataList_.prepend(pbData);
		if(currentDataCell_ >= 0)
			currentDataCell_++;
	}
	else
		return;	//We already have this data
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
	if(time <= 0 || !pbDataList_.size())
		return;
	PlaybackIndex index(PlaybackIndex::minForTime(time));
	if(index < getMinIndex())
		return;
	if(getCurrentIndexPriv().isValid() && index > getCurrentIndexPriv())
	{//If clearing would remove the current cell, clear before current cell time.
		index = PlaybackIndex::minForTime(getCurrentIndexPriv().time());
	}
	else if(index > getMaxIndex())
	{//If no current index was set yet, and index is beyond all data, just
	 //reset everything.
		reset();
		return;
	}
	minIndex_ = index;
	int clearBeforeCell = findIndexCell(index);
	if(clearBeforeCell < 0)
		return;
	if(pbDataList_[clearBeforeCell]->index_ < index)
		clearBeforeCell++;
	pbDataList_ = pbDataList_.mid(clearBeforeCell);
	currentDataCell_ = currentDataCell_-clearBeforeCell;
}

void DataState::clearDataAfter(double time)
{
	QMutexLocker locker(mutex_.data());
	PlaybackIndex index(PlaybackIndex::maxForTime(time));
	if(index > getMaxIndex() || !pbDataList_.size())
		return;

	if(getCurrentIndexPriv().isValid() && index < getCurrentIndexPriv())
	{//If clearing would remove the current cell, clear after current cell time.
		index = PlaybackIndex::maxForTime(getCurrentIndexPriv().time());
	}
	else if(index < getMinIndex())
	{//If no current index was set yet, and index is below all data, just
	 //reset everything.
		reset();
		return;
	}
	maxIndex_ = index;
	int clearAfterCell = findIndexCell(index);
	pbDataList_ = pbDataList_.mid(0,clearAfterCell+1);
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
	if(currentDataCell_ >= pbDataList_.size()-1)
		return false;
	moveToCell(currentDataCell_+1,last);
	return true;
}

bool DataState::moveToPrev(bool last)
{
	if(currentDataCell_ < 0)
		return false;
	moveToCell(currentDataCell_-1,last);
	return true;
}

bool DataState::moveToCell(int cellId,bool last)
{
	if(cellId < -1 || cellId >= pbDataList_.size())
		return false;
	if(cellId == currentDataCell_)
		return true;
	bool reverse = cellId < currentDataCell_;
	currentDataCell_ = cellId;
	mutex_->unlock();	//Since sub classes may call functions that lock the mutex, release it for
						//this call.
	triggerValueChange(reverse,last);
	mutex_->lock();
	return true;
}

PlaybackIndex DataState::get2NextIndex()
{
	if(currentDataCell_ >= pbDataList_.size()-2)
	{
		return PlaybackIndex();
	}
	return pbDataList_[currentDataCell_+2]->index_;
}

int DataState::findIndexCell(PlaybackIndex index)
{
	if(!index.isValid())
		return -1;
	if(pbDataList_.isEmpty())
		return -1;
	return binaryIndexSearch(index,0,pbDataList_.size()-1);
}

int DataState::binaryIndexSearch(PlaybackIndex index,int minCell, int maxCell)
{
	//Stop condition
	if(maxCell == minCell)
	{
		PlaybackIndex sIndex = pbDataList_[minCell]->index_;
		if(sIndex <= index)
		{
			return minCell;
		}
		return -1;
	}
	//Reduce search space
	int midCell = 1+(maxCell+minCell)/2;
	if(pbDataList_[midCell]->index_ <= index)
		return binaryIndexSearch(index,midCell,maxCell);
	return binaryIndexSearch(index,minCell,midCell-1);
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

QSharedPointer<IndexedData> DataState::getCurrentValuePriv()
{
	if(currentDataCell_ < 0)
		return QSharedPointer<IndexedData>();
	return pbDataList_[currentDataCell_];
}

QSharedPointer<IndexedData> DataState::getPrevValuePriv()
{
	if(currentDataCell_ <= 0)
	{
		return QSharedPointer<IndexedData>();
	}
	return pbDataList_[currentDataCell_-1];
}

QSharedPointer<IndexedData> DataState::getNextValuePriv()
{
	if(currentDataCell_ == pbDataList_.size()-1)
	{
		return QSharedPointer<IndexedData>();
	}
	return pbDataList_[currentDataCell_+1];
}

QList<QSharedPointer<IndexedData>> DataState::getValuesSincePriv(double time)
{
	PlaybackIndex minIndex(PlaybackIndex::minForTime(time));
	if(minIndex >= getCurrentIndexPriv())
		return QList<QSharedPointer<IndexedData>>();
	int dataFromCell = findIndexCell(minIndex);
	if(dataFromCell < 0)
		dataFromCell = 0;
	if(pbDataList_[dataFromCell]->index_ < minIndex)
		dataFromCell++;
	return pbDataList_.mid(dataFromCell,currentDataCell_-dataFromCell+1);
}

QList<QSharedPointer<IndexedData>> DataState::getValuesUntilPriv(double time)
{
	PlaybackIndex index(PlaybackIndex::maxForTime(time));
	if(index <= getCurrentIndexPriv())
		return QList<QSharedPointer<IndexedData>>();
	int dataToCell = findIndexCell(index);
	return pbDataList_.mid(currentDataCell_+1,dataToCell-currentDataCell_);
}

PlaybackIndex DataState::getCurrentIndexPriv()
{
	if(currentDataCell_ >= 0)
		return pbDataList_[currentDataCell_]->index_;
	return PlaybackIndex();
}

PlaybackIndex DataState::getPrevIndexPriv()
{
	if(currentDataCell_ <= 0)
	{	
		return PlaybackIndex();	//No more data.
	}
	return pbDataList_[currentDataCell_-1]->index_;
}

PlaybackIndex DataState::getNextIndexPriv()
{
	if(currentDataCell_ >= pbDataList_.size()-1)
	{
		return PlaybackIndex();	//No more data.
	}
	return pbDataList_[currentDataCell_+1]->index_;
}

PlaybackIndex DataState::getFirstIndexPriv()
{
	if(pbDataList_.size())
		return pbDataList_.first()->index_;
	return PlaybackIndex();
}

PlaybackIndex DataState::getLastIndexPriv()
{
	if(pbDataList_.size())
		return pbDataList_.last()->index_;
	return PlaybackIndex();
}