#include <QMutexLocker>
#include <QThread>
#include "LinkedDataStateList.h"
using namespace Picto;			

//Template function definitions must be in header file-------------------------------------------
LinkedDataStateList::LinkedDataStateList()
{
	clear();
	initialized_ = false;
}

LinkedDataStateList::~LinkedDataStateList()
{

}

void LinkedDataStateList::clear()
{
	pbDataList_.clear();
	currentDataCell_ = pbDataList_.end();
	initialized_ = false;
}

int LinkedDataStateList::size()
{
	return pbDataList_.size();
}

void LinkedDataStateList::append(QSharedPointer<IndexedData> data)
{
	pbDataList_.append(data);
}

void LinkedDataStateList::clearBefore(double time)
{
	if(!size())
		return;
	PlaybackIndex index = PlaybackIndex::minForTime(time);
	if(index <= firstIndex())
		return;
	PlaybackIndex currId = currIndex();
	CellIter loc;
	if(currId < index)
	{//If clearing would remove the current cell, clear before current cell time.
		index = currId;
		loc = currentDataCell_;
	}
	else
	{
		loc = findIndexCell(index);
		if(loc == pbDataList_.end())
			return;//The index was less than the first value
		while(loc != pbDataList_.begin() && (*(loc-1))->index_.time() == time)
			loc--;
	}
	if(loc == pbDataList_.begin())
		return;
	pbDataList_.erase(pbDataList_.begin(),loc);
}

QSharedPointer<IndexedData> LinkedDataStateList::currValue()
{
	if(!initialized_)
		return QSharedPointer<IndexedData>();
	return (*currentDataCell_);
}

QSharedPointer<IndexedData> LinkedDataStateList::nextValue()
{
	if(!initialized_)
		return firstValue();
	if(currentDataCell_+1 == pbDataList_.end())
		return QSharedPointer<IndexedData>();
	return *(currentDataCell_+1);
}

QSharedPointer<IndexedData> LinkedDataStateList::next2Value()
{
	if(!initialized_ && size() > 1)
		return *(pbDataList_.begin()+1);
	if(	currentDataCell_+1 == pbDataList_.end()
		|| currentDataCell_+2 == pbDataList_.end())
		return QSharedPointer<IndexedData>();
	return *(currentDataCell_+2);
}

QSharedPointer<IndexedData> LinkedDataStateList::firstValue()
{
	if(!pbDataList_.size())
		return QSharedPointer<IndexedData>();
	return *pbDataList_.begin();
}

QSharedPointer<IndexedData> LinkedDataStateList::lastValue()
{
	if(!pbDataList_.size())
		return QSharedPointer<IndexedData>();
	return *(pbDataList_.end()-1);
}

PlaybackIndex LinkedDataStateList::currIndex()
{
	QSharedPointer<IndexedData> val = currValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

PlaybackIndex LinkedDataStateList::nextIndex()
{
	QSharedPointer<IndexedData> val = nextValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

PlaybackIndex LinkedDataStateList::next2Index()
{
	QSharedPointer<IndexedData> val = next2Value();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

PlaybackIndex LinkedDataStateList::firstIndex()
{
	QSharedPointer<IndexedData> val = firstValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

PlaybackIndex LinkedDataStateList::lastIndex()
{
	QSharedPointer<IndexedData> val = lastValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

PlaybackIndex LinkedDataStateList::moveToCell(PlaybackIndex index)
{
	CellIter cellId = findIndexCell(index);
	if(cellId != pbDataList_.end())
	{
		currentDataCell_ = cellId;
		initialized_ = true;
	}
	return currIndex();
}

PlaybackIndex LinkedDataStateList::moveToNext()
{
	if(!initialized_)
	{
		currentDataCell_ = pbDataList_.begin();
		initialized_ = true;
	}
	else if(currentDataCell_+1 != pbDataList_.end())
		currentDataCell_++;
	return currIndex();		
}

QList<QSharedPointer<IndexedData>> LinkedDataStateList::getValuesSince(double time)
{
	QList<QSharedPointer<IndexedData>> emptyVal;
	if(!currIndex().isValid() || currIndex().time() <= time)
		return emptyVal;
	CellIter cellId = findIndexCell(PlaybackIndex::minForTime(time));
	if(cellId == pbDataList_.end())
	{
		if(firstIndex().time() > time)
			cellId = pbDataList_.begin();
		else
			return emptyVal;
	}
	return getList(cellId,currentDataCell_);
}

QList<QSharedPointer<IndexedData>> LinkedDataStateList::getValuesUntil(double time)
{
	QList<QSharedPointer<IndexedData>> emptyVal;
	if(!currIndex().isValid() || currIndex().time() >= time || !nextIndex().isValid())
		return emptyVal;
	CellIter cellId = findIndexCell(PlaybackIndex::maxForTime(time));
	if(cellId == pbDataList_.end())
		return emptyVal;
	return getList(currentDataCell_+1,cellId);
}

QList<QSharedPointer<IndexedData>> LinkedDataStateList::getList(CellIter begin,CellIter end)
{
	QList<QSharedPointer<IndexedData>> returnVal;
	for(CellIter iter = begin;iter!=end;iter++)
	{
		returnVal.append(*iter);
	}
	return returnVal;
}

CellIter LinkedDataStateList::findIndexCell(PlaybackIndex index)
{
	if(!index.isValid())
		return pbDataList_.end();
	if(!size())
		return pbDataList_.end();
	CellIter finder = currentDataCell_;
	if(finder == pbDataList_.end())
		finder = pbDataList_.begin();
	int inc = 1;
	if((*finder)->index_ > index)
		inc = -1;
	while	(	
				(inc < 0 || finder+1 != pbDataList_.end())
				&& (inc > 0 || finder != pbDataList_.begin()) 
				&& !(	
						(	(*finder)->index_ <= index	) 
						&&	((*(finder+1))->index_ > index	)
					)
			)
	{
		finder = finder+inc;
	}
	if(finder == pbDataList_.begin() && (*finder)->index_ > index)
	{
		finder = pbDataList_.end();
	}
	return finder;
}