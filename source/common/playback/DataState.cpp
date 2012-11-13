#include "DataState.h"
using namespace Picto;

//Template function definitions must be in header file-------------------------------------------
DataState::DataState(bool moveByIterating) :
iterateOnMove_(moveByIterating)
{
	reset();
}

DataState::~DataState()
{

}

void DataState::setValue(QSharedPointer<IndexedData> pbData)
{
	//If the Index's pos value was not set yet, set it appropriately.
	if(!pbData->index_.isValid())
	{
		if(pbData->index_.time() == getLastIndex().time())
			pbData->index_.pos() = getLastIndex().pos()+1;
		else if(pbData->index_.time() == getFirstIndex().time())
			pbData->index_.pos() = getFirstIndex().pos()-1;
		else //If this is data for a new time, just set its pos to the center of the pos range.
			pbData->index_.pos() = 0x7FFFFFFFFFFFFFFF;
	}

	if(pbData->index_ > getLastIndex())
	{
		//Add it to the end of the list
		pbDataList_.append(pbData);
	}
	else if(pbData->index_ < getFirstIndex())
	{

		//Add it to the beginning of the list
		pbDataList_.prepend(pbData);
	}
	else
		return;	//We already have this data
	setBoundValues(pbData->index_);
}

bool DataState::setCurrentIndex(PlaybackIndex index)
{
	if(index == getCurrentIndex())
	{
		return true;
	}

	//Check if index implies a single cell move.  If so, move.
	if(index > getCurrentIndex())
	{
		PlaybackIndex secondIndex = get2NextIndex(index.time());
		if(secondIndex.isValid() && (secondIndex > index))
		{							
			if(getNextIndex(index.time()) <= index)
				moveToNext(true);
			//If we got here, the current index is the closest <= to the input so we're done.
			return true;
		}
	}
	else
	{
		if(getPrevIndex(index.time()) <= index)
		{
			moveToPrev(true);
			//If we got here, the current index is the closest <= to the input so we're done.
			return true;
		}
	}

	//Move to index.
	if(iterateOnMove_)
	{	
		PlaybackIndex nextInd = getNextIndex(index.time());
		while(nextInd.isValid() && nextInd <= index)
		{
			moveToNext(nextInd >= index);
			nextInd = getNextIndex(index.time());
		}
		while(getCurrentIndex() > index && getPrevIndex(index.time()).isValid())
			moveToPrev(getPrevIndex(index.time()) <= index);
		if(getCurrentIndex() <= index)
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

//Functions below clear all IndexedData before/after the input index for memory savings.
void DataState::clearDataBefore(double time)
{
	PlaybackIndex index(PlaybackIndex::minForTime(time));
	if(index < getMinIndex())
		return;
	if(getCurrentIndex().isValid() && index > getCurrentIndex())
	{//If clearing would remove the current cell, clear before current cell time.
		index = PlaybackIndex::minForTime(getCurrentIndex().time());
	}
	else if(index > getMaxIndex())
	{//If no current index was set yet, and index is beyond all data, just
	 //reset everything.
		reset();
		return;
	}
	minIndex_ = index;
	int clearBeforeCell = findIndexCell(index);
	if(pbDataList_[clearBeforeCell]->index_ < index)
		clearBeforeCell++;
	pbDataList_ = pbDataList_.mid(clearBeforeCell);
	currentDataCell_ = currentDataCell_-clearBeforeCell;
}

void DataState::clearDataAfter(double time)
{
	PlaybackIndex index(PlaybackIndex::maxForTime(time));
	if(index > getMaxIndex())
		return;

	if(getCurrentIndex().isValid() && index < getCurrentIndex())
	{//If clearing would remove the current cell, clear after current cell time.
		index = PlaybackIndex::maxForTime(getCurrentIndex().time());
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

void DataState::reset()
{
	pbDataList_.clear();
	currentDataCell_ = -1;
	maxIndex_ = PlaybackIndex();
	minIndex_ = PlaybackIndex();
}

QSharedPointer<IndexedData> DataState::getCurrentValue()
{
	if(currentDataCell_ < 0)
		return QSharedPointer<IndexedData>();
	return pbDataList_[currentDataCell_];
}

QSharedPointer<IndexedData> DataState::getPrevValue(double lookBackTime)
{
	if(currentDataCell_ <= 0)
	{
		//Call getPrevIndex to force valid data up to lookBackTime
		PlaybackIndex prevInd = getPrevIndex(lookBackTime);
		if(prevInd.isValid())
			return getPrevValue(lookBackTime);	//We now have enough data.  Recurse.
		return QSharedPointer<IndexedData>();
	}
	return pbDataList_[currentDataCell_-1];
}

QSharedPointer<IndexedData> DataState::getNextValue(double lookForwardTime)
{
	if(currentDataCell_ == pbDataList_.size()-1)
	{
		//Call getNextIndex to force valid data up to lookForwardTime
		PlaybackIndex nextInd = getNextIndex(lookForwardTime);
		if(nextInd.isValid())
			return getNextValue(lookForwardTime);	//We now have enough data.  Recurse.
		return QSharedPointer<IndexedData>();
	}
	return pbDataList_[currentDataCell_+1];
}

QList<QSharedPointer<IndexedData>> DataState::getValuesSince(double time)
{
	PlaybackIndex minIndex(PlaybackIndex::minForTime(time));
	if(minIndex >= getCurrentIndex())
		return QList<QSharedPointer<IndexedData>>();
	int dataFromCell = findIndexCell(minIndex);
	if(dataFromCell < 0)
		dataFromCell = 0;
	if(pbDataList_[dataFromCell]->index_ < minIndex)
		dataFromCell++;
	return pbDataList_.mid(dataFromCell,currentDataCell_-dataFromCell+1);
}

QList<QSharedPointer<IndexedData>> DataState::getValuesUntil(double time)
{
	PlaybackIndex maxIndex(PlaybackIndex::maxForTime(time));
	if(maxIndex <= getCurrentIndex())
		return QList<QSharedPointer<IndexedData>>();
	int dataToCell = findIndexCell(maxIndex);
	return pbDataList_.mid(currentDataCell_+1,dataToCell-currentDataCell_);
}

PlaybackIndex DataState::getCurrentIndex()
{
	if(currentDataCell_ >= 0)
		return pbDataList_[currentDataCell_]->index_;
	return PlaybackIndex();
}

PlaybackIndex DataState::getPrevIndex(double lookBackTime)
{
	if(currentDataCell_ <= 0)
	{	
		if(currentDataCell_ < 0)
			return PlaybackIndex();	//Before what?
		if(lookBackTime < 0)
		{	//Get previous value no matter how far it is.
			requestNextData(getMinIndex(),true);
			if(currentDataCell_ <= 0)
				return PlaybackIndex();	//No more data available.
			return getPrevIndex(lookBackTime);
		}
		//Get previous values up to lookBackTime
		if(getMinIndex().time() <= lookBackTime)
			return PlaybackIndex();	//No more data.
		//Assure that our lookup window is big enough
		assureMinDataWindow(lookBackTime,getMaxIndex().time());
		//We now have enough data to make a decision, recurse
		return getPrevIndex(lookBackTime);
	}
	return pbDataList_[currentDataCell_-1]->index_;
}

PlaybackIndex DataState::getNextIndex(double lookForwardTime)
{
	if(currentDataCell_ >= pbDataList_.size()-1)
	{
		if(lookForwardTime < 0)
		{	//Get next value no matter how far it is.
			requestNextData(getMaxIndex(),false);
			if(currentDataCell_ >= pbDataList_.size()-1)
				return PlaybackIndex();	//No more data.
			return getNextIndex(lookForwardTime);
		}
		if(lookForwardTime <= getMaxIndex().time())
			return PlaybackIndex();	//No more data.

		//Assure that our lookup window is big enough
		assureMinDataWindow(getMinIndex().time(),lookForwardTime);
		//We now have enough data to make a decision, recurse
		return getNextIndex(lookForwardTime);
	}
	return pbDataList_[currentDataCell_+1]->index_;
}

PlaybackIndex DataState::getFirstIndex()
{
	if(pbDataList_.size())
		return pbDataList_.first()->index_;
	return PlaybackIndex();
}

PlaybackIndex DataState::getLastIndex()
{
	if(pbDataList_.size())
		return pbDataList_.last()->index_;
	return PlaybackIndex();
}

void DataState::setBoundValues(PlaybackIndex index)
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
	bool reverse = cellId < currentDataCell_;
	currentDataCell_ = cellId;
	triggerValueChange(reverse,last);
	return true;
}

PlaybackIndex DataState::get2NextIndex(double lookForwardTime)
{
	if(currentDataCell_ >= pbDataList_.size()-2)
	{
		if(lookForwardTime < 0)
		{	//Get next value no matter how far it is.
			requestNextData(getMaxIndex(),false);
			if(currentDataCell_ >= pbDataList_.size()-2)
				return PlaybackIndex();	//No more data.
			return get2NextIndex(lookForwardTime);
		}
		if(lookForwardTime <= getMaxIndex().time())
			return PlaybackIndex();	//No more data.

		//Assure that our lookup window is big enough
		assureMinDataWindow(getMinIndex().time(),lookForwardTime);
		//We now have enough data to make a decision, recurse
		return get2NextIndex(lookForwardTime);
	}
	return pbDataList_[currentDataCell_+2]->index_;
}

int DataState::findIndexCell(PlaybackIndex index)
{
	if(!index.isValid())
		return -1;
	if(index > getMaxIndex())
	{
		requestMoreData(getMaxIndex(),index);
		if(index > getMaxIndex())
			return -1;
	}
	else if(index < getMinIndex())
	{
		requestMoreData(getMinIndex(),index);
		if(index < getMinIndex())
			return -1;
	}else if(pbDataList_.isEmpty())
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

void DataState::assureMinDataWindow(double lowTime,double highTime)
{
	PlaybackIndex highIndex = PlaybackIndex::maxForTime(highTime);
	PlaybackIndex lowIndex = PlaybackIndex::minForTime(lowTime);
	if((highTime >= 0) && (highIndex > getMaxIndex()))
	{
		requestMoreData(getMaxIndex(),highIndex);
		setBoundValues(highIndex);
	}
	if((lowTime >= 0) && (lowIndex < getMinIndex()))
	{
		requestMoreData(getMinIndex(),lowIndex);
		setBoundValues(lowIndex);
	}
}
