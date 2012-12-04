#ifndef _DATASTATELIST_H_
#define _DATASTATELIST_H_
#include <QSharedPointer>
#include "IndexedData.h"

namespace Picto {

/*! \brief Holds Data State's Data.
 */
template <template <typename> class T>
class DataStateList
{
	typedef typename T<QSharedPointer<IndexedData>> ListType;
	typedef typename ListType::iterator CellIter;
public:
	DataStateList();
	virtual ~DataStateList();

	void clear();
	int size();
	void append(QSharedPointer<IndexedData> data);
	void clearBefore(double time);

	QSharedPointer<IndexedData> currValue();
	QSharedPointer<IndexedData> nextValue();
	QSharedPointer<IndexedData> next2Value();
	QSharedPointer<IndexedData> firstValue();
	QSharedPointer<IndexedData> lastValue();

	PlaybackIndex currIndex();
	PlaybackIndex nextIndex();
	PlaybackIndex next2Index();
	PlaybackIndex firstIndex();
	PlaybackIndex lastIndex();

	PlaybackIndex moveToCell(PlaybackIndex index);
	PlaybackIndex moveToNext();

	QList<QSharedPointer<IndexedData>> getValuesSince(double time);
	QList<QSharedPointer<IndexedData>> getValuesUntil(double time);

private:

	//These functions should be specialized for each container type used for this template.-----
	//All other functions should function for standard QT data types.
	//Returns a list of Indexed Data pointers starting at the begin iterator and ending
	//at the end iterator inclusive.
	QList<QSharedPointer<IndexedData>> getList(CellIter begin,CellIter end);
	//Returns the list index of the cell with the highest playback index less than or equal to
	//the input.  If no such index is found, returns the pbDataList_ end iterator
	CellIter findIndexCell(PlaybackIndex index);
	//------------------------------------------------------------------------------------------

	typename ListType::iterator currentDataCell_;
	ListType pbDataList_;
	bool initialized_;
};





//Template function definitions must be in header file-------------------------------------------
template <template <typename> class T>
DataStateList<T>::DataStateList()
{
	clear();
	initialized_ = false;
}

template <template <typename> class T>
DataStateList<T>::~DataStateList()
{

}

template <template <typename> class T>
void DataStateList<T>::clear()
{
	pbDataList_.clear();
	currentDataCell_ = pbDataList_.end();
	initialized_ = false;
}

template <template <typename> class T>
int DataStateList<T>::size()
{
	return pbDataList_.size();
}

template <template <typename> class T>
void DataStateList<T>::append(QSharedPointer<IndexedData> data)
{
	pbDataList_.append(data);
}

template <template <typename> class T>
void DataStateList<T>::clearBefore(double time)
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

template <template <typename> class T>
QSharedPointer<IndexedData> DataStateList<T>::currValue()
{
	if(!initialized_)
		return QSharedPointer<IndexedData>();
	return (*currentDataCell_);
}

template <template <typename> class T>
QSharedPointer<IndexedData> DataStateList<T>::nextValue()
{
	if(!initialized_)
		return firstValue();
	if(currentDataCell_+1 == pbDataList_.end())
		return QSharedPointer<IndexedData>();
	return *(currentDataCell_+1);
}

template <template <typename> class T>
QSharedPointer<IndexedData> DataStateList<T>::next2Value()
{
	if(!initialized_)
	{
		if(size() > 1)
			return *(pbDataList_.begin()+1);
		return QSharedPointer<IndexedData>();
	}
	if(	currentDataCell_+1 == pbDataList_.end()
		|| currentDataCell_+2 == pbDataList_.end())
		return QSharedPointer<IndexedData>();
	return *(currentDataCell_+2);
}

template <template <typename> class T>
QSharedPointer<IndexedData> DataStateList<T>::firstValue()
{
	if(!pbDataList_.size())
		return QSharedPointer<IndexedData>();
	return *pbDataList_.begin();
}

template <template <typename> class T>
QSharedPointer<IndexedData> DataStateList<T>::lastValue()
{
	if(!pbDataList_.size())
		return QSharedPointer<IndexedData>();
	return *(pbDataList_.end()-1);
}

template <template <typename> class T>
PlaybackIndex DataStateList<T>::currIndex()
{
	QSharedPointer<IndexedData> val = currValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

template <template <typename> class T>
PlaybackIndex DataStateList<T>::nextIndex()
{
	QSharedPointer<IndexedData> val = nextValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

template <template <typename> class T>
PlaybackIndex DataStateList<T>::next2Index()
{
	QSharedPointer<IndexedData> val = next2Value();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

template <template <typename> class T>
PlaybackIndex DataStateList<T>::firstIndex()
{
	QSharedPointer<IndexedData> val = firstValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

template <template <typename> class T>
PlaybackIndex DataStateList<T>::lastIndex()
{
	QSharedPointer<IndexedData> val = lastValue();
	if(!val)
		return PlaybackIndex();
	return val->index_;
}

template <template <typename> class T>
PlaybackIndex DataStateList<T>::moveToCell(PlaybackIndex index)
{
	CellIter cellId = findIndexCell(index);
	if(cellId != pbDataList_.end())
	{
		currentDataCell_ = cellId;
		initialized_ = true;
	}
	return currIndex();
}

template <template <typename> class T>
PlaybackIndex DataStateList<T>::moveToNext()
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

template <template <typename> class T>
QList<QSharedPointer<IndexedData>> DataStateList<T>::getValuesSince(double time)
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

template <template <typename> class T>
QList<QSharedPointer<IndexedData>> DataStateList<T>::getValuesUntil(double time)
{
	QList<QSharedPointer<IndexedData>> emptyVal;
	if(!currIndex().isValid() || currIndex().time() >= time || !nextIndex().isValid())
		return emptyVal;
	CellIter cellId = findIndexCell(PlaybackIndex::maxForTime(time));
	if(cellId == pbDataList_.end())
		return emptyVal;
	return getList(currentDataCell_+1,cellId);
}

//Dummy function.  This needs to be specialized.
template <template <typename> class T>
QList<QSharedPointer<IndexedData>> DataStateList<T>::getList(CellIter begin,CellIter end)
{
	return QList<QSharedPointer<IndexedData>>();
}

//Dummy function.  This needs to be specialized.
template <template <typename> class T>
typename DataStateList<T>::CellIter DataStateList<T>::findIndexCell(PlaybackIndex index)
{
	return pbDataList_.end();
}

}; //namespace Picto
#endif