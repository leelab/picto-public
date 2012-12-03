#include <QMutexLocker>
#include <QThread>
#include "VectorDataStateList.h"
using namespace Picto;			

//Template function definitions must be in header file-------------------------------------------
VectorDataStateList::VectorDataStateList()
{
	clear();
}

VectorDataStateList::~VectorDataStateList()
{

}

void VectorDataStateList::clear()
{
	pbDataList_.clear();
	currentDataCell_ = -1;
}

int VectorDataStateList::size()
{
	return pbDataList_.size();
}

void VectorDataStateList::append(QSharedPointer<IndexedData> data)
{
	pbDataList_.append(data);
}

void VectorDataStateList::clearBefore(double time)
{
	if(!size())
		return;
	PlaybackIndex index = PlaybackIndex::minForTime(time);
	if(index <= firstIndex())
		return;
	PlaybackIndex currId = currIndex();
	int loc;
	if(currId < index)
	{//If clearing would remove the current cell, clear before current cell time.
		index = currId;
		loc = currentDataCell_;
	}
	else
	{
		loc = findIndexCell(index);
		while(loc > 0 && pbDataList_[loc-1]->index_.time() == time)
			loc--;
	}
	if(loc <= 0)
		return;
	pbDataList_ = pbDataList_.mid(loc);
	currentDataCell_ = currentDataCell_-loc;
}

QSharedPointer<IndexedData> VectorDataStateList::currValue()
{
	if(currentDataCell_ < 0 || currentDataCell_ >= pbDataList_.size())
		return QSharedPointer<IndexedData>();
	return pbDataList_[currentDataCell_];
}

QSharedPointer<IndexedData> VectorDataStateList::nextValue()
{
	if(currentDataCell_ >= pbDataList_.size()-1)
		return QSharedPointer<IndexedData>();
	return pbDataList_[currentDataCell_+1];
}

QSharedPointer<IndexedData> VectorDataStateList::next2Value()
{
	if(currentDataCell_ >= pbDataList_.size()-2)
		return QSharedPointer<IndexedData>();
	return pbDataList_[currentDataCell_+2];
}

QSharedPointer<IndexedData> VectorDataStateList::firstValue()
{
	if(!pbDataList_.size())
		return QSharedPointer<IndexedData>();
	return pbDataList_[0];
}

QSharedPointer<IndexedData> VectorDataStateList::lastValue()
{
	if(!pbDataList_.size())
		return QSharedPointer<IndexedData>();
	return pbDataList_[pbDataList_.size()-1];
}

PlaybackIndex VectorDataStateList::currIndex()
{
	QSharedPointer<IndexedData> val = currValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

PlaybackIndex VectorDataStateList::nextIndex()
{
	QSharedPointer<IndexedData> val = nextValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

PlaybackIndex VectorDataStateList::next2Index()
{
	QSharedPointer<IndexedData> val = next2Value();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

PlaybackIndex VectorDataStateList::firstIndex()
{
	QSharedPointer<IndexedData> val = firstValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

PlaybackIndex VectorDataStateList::lastIndex()
{
	QSharedPointer<IndexedData> val = lastValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

PlaybackIndex VectorDataStateList::moveToCell(PlaybackIndex index)
{
	int cellId = findIndexCell(index);
	if(cellId >= 0)
		moveToCell(cellId);
	return currIndex();
}

PlaybackIndex VectorDataStateList::moveToNext()
{
	if(currentDataCell_ < pbDataList_.size())
		currentDataCell_++;
	return currIndex();		
}

QList<QSharedPointer<IndexedData>> VectorDataStateList::getValuesSince(double time)
{
	int index = findIndexCell(PlaybackIndex::minForTime(time));
	if(index < 0)
		index = 0;
	if(index >= currentDataCell_)
		return QList<QSharedPointer<IndexedData>>();
	return pbDataList_.mid(index,currentDataCell_-index+1);
}

QList<QSharedPointer<IndexedData>> VectorDataStateList::getValuesUntil(double time)
{
	int index = findIndexCell(PlaybackIndex::maxForTime(time));
	if(index <= currentDataCell_)
		return QList<QSharedPointer<IndexedData>>();
	return pbDataList_.mid(currentDataCell_+1,index-currentDataCell_);
}

int VectorDataStateList::currCell()
{
	return currentDataCell_;
}

int VectorDataStateList::moveToCell(int cell)
{
	if(cell >= 0 && cell < pbDataList_.size())
		currentDataCell_ = cell;
	return currentDataCell_;		
}

int VectorDataStateList::findIndexCell(PlaybackIndex index)
{
	if(!index.isValid())
		return -1;
	if(pbDataList_.isEmpty())
		return -1;
	return indexSearch(index,0,pbDataList_.size()-1);
}

int VectorDataStateList::indexSearch(PlaybackIndex index,int minCell, int maxCell)
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
		return indexSearch(index,midCell,maxCell);
	return indexSearch(index,minCell,midCell-1);
}