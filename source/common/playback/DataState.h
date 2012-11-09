#ifndef _DATASTATE_H_
#define _DATASTATE_H_
#include <QSharedPointer>

#include "IndexedData.h"

namespace Picto {
/*! \brief Component of Picto Playback system that stores IndexedData values.
 *	This class stores indexed playback data and offers several functions for
 *  querying that data.  It can be configured to store data within an index
 *	window or simply store data available from the current index forward.
 */
template <class I>// <type of data index>
class DataState
{
public:
	//@param moveByIterating If set true, this DataState will always move to the index
	//		set by setCurrentIndex by iterating through each index along the way and
	//		triggering triggerValueChange() for each one.
	DataState(bool moveByIterating);
	virtual ~DataState();
	//SET FUNCTIONS-----------------------------------------------------------------
	//IndexedData values must be set either in increasing index order after latest
	//or in decreaseing index order before the first
	void setValue(QSharedPointer<IndexedData<I>> pbData);
	//Sets the current index for getting IndexedData values.
	//Returns false if the input index cannot be set (ie. data has been cleared at that index)
	bool setCurrentIndex(I index);
	//Sets the maximum/minimum time/index of data available.  This is important because
	//it lets the DataState know if a lack of available data is a result data not having 
	//been loaded yet or to a simple lack of data points.
	//Note: The value is automatically detected as a max or min and set accordingly.
	void setBoundValues(double time,I index);
	//Functions below clear all IndexedData before/after the input index for memory savings.
	void clearDataBefore(double time);
	void clearDataAfter(double time);
	//Resets this Data State back to its initial condition
	virtual void reset();

	//GET FUNCTIONS-----------------------------------------------------------------
	//Gets the current IndexedData item.
	QSharedPointer<IndexedData<I>> getCurrentValue();
	//Gets the previous IndexedData item
	QSharedPointer<IndexedData<I>> getPrevValue();
	//Gets the next IndexedData item
	QSharedPointer<IndexedData<I>> getNextValue();

	//Returns a list of IndexedData with indeces > the input index and <= the current index.  
	QList<QSharedPointer<IndexedData<I>>> getValuesSince(I index);
	//Returns a list of IndexedData with indeces > the current index and <= the input index.  
	QList<QSharedPointer<IndexedData<I>>> getValuesUntil(I index);

	//Gets the current index of this DataState
	I getCurrentIndex();

	//Gets the previous index available in this DataState before the current one
	I getPrevIndex();

	//Gets the next index available in this DataState after the current one
	I getNextIndex();

	//Returns the index before which data is not available
	I getFirstIndex();

	//Returns the index after which data is not available
	I getLastIndex();

protected:
	//@param reverse indicates if the value was reached by
	//moving the current index backward (ie. back in time)
	//@param last indicates that this is the last iteration
	//from the current setCurrentIndex() call.
	//Note: If more than one values have the same index, 
	//this will be called one after another for all of these
	//values with reverse and last the same throughout for a
	//single setCurrentIndex call.
	virtual void triggerValueChange(bool reverse,bool last) = 0;
	//Tells child classes that we need more data in order to
	//carry out a request.
	//@param index indicates the index for which we were unable
	//to get data.
	virtual void requestMoreData(I index) = 0;
	//Tells child classes that we need more data in order to
	//carry out a request.
	//@param time indicates the time for which we were unable
	//to get data.
	virtual void requestMoreDataByTime(double time) = 0;

private:
	//Data may not be available after getLastIndex() but there may just have been physical not be data
	//in that region.  getPostMaxIndex() gets the maximum index that we can use for which we
	//can be sure our existing data is sufficient.
	I getPostMaxIndex();
	I getPostMaxTime();
	I getPreMinIndex();
	I getPreMinTime();

	//Moves current cell to the next one.
	//@param last Indicates that this is the last iteration
	//to move to the current index
	bool moveToNext(bool last);
	//Moves current cell to the last one.
	//@param last Indicates that this is the last iteration
	//to move to the current index
	bool moveToPrev(bool last);
	//Moves to cell at input cellId without iterating through intervening
	//cells (regardless of iterateOnMove_).
	//@param last Indicates that this is the last iteration
	//to move to the current index
	//Note: Error checking is not done on cellId for optimization
	bool moveToCell(int cellId,bool last);
	//Returns the index of the cell two cells after the current one.
	I get2NextIndex();
	//Returns the cell number of the first cell with the input index.
	int findIndexCell(qulonglong index);
	//Recursively searches for the highest indexed cell with index <= input index.
	int binaryIndexSearch(qulonglong index,int minCell, int maxCell);
	double maxTime_;
	I maxIndex_;
	double minTime_;
	I minIndex_;
	int currentDataCell_;
	bool iterateOnMove_;
	//List of IndexedData items ordered by their index.
	QList<QSharedPointer<IndexedData<I>>> pbDataList_;
};

//Template function definitions must be in header file-------------------------------------------
template <class I> DataState<I>::DataState(bool moveByIterating) :
iterateOnMove_(moveByIterating)
{
	reset();
}

template <class I> DataState<I>::~DataState()
{

}

template <class I> void DataState<I>::setValue(QSharedPointer<IndexedData<I>> pbData)
{
	Q_ASSERT((pbData->index_ >= getLastIndex()) || (pbData->index_ <= getFirstIndex()));
	if(pbData->index_ >= getLastIndex())
		pbDataList_.append(pbData);
	else
		pbDataList_.prepend(pbData);
	setBoundValues(pbData->time_,pbData->index_);
}

template <class I> bool DataState<I>::setCurrentIndex(I index)
{
	if(index == getCurrentIndex())
	{
		return true;
	}

	//Check if index implies a single cell move.  If so, move.
	if(index > getCurrentIndex())
	{
		I secondIndex = get2NextIndex();
		if(secondIndex && (secondIndex > index))
		{							
			if(getNextIndex() <= index)
				moveToNext(true);
			//If we got here, the current index is the closest <= to the input so we're done.
			return true;
		}
	}
	else
	{
		if(getPrevIndex() <= index)
		{
			moveToPrev(true);
			//If we got here, the current index is the closest <= to the input so we're done.
			return true;
		}
	}

	//Move to index.
	if(iterateOnMove_)
	{	
		I nextInd = getNextIndex();
		while(nextInd <= index)
		{
			moveToNext(nextInd >= index);
			nextInd = getNextIndex();
		}
		while(getCurrentIndex() > index)
			moveToPrev(getPrevIndex() <= index);
	}
	else
	{
		//Search for cell with greatest index less than or equal to index
		int cellId = findIndexCell(index);
		moveToCell(cellId,true);
	}
	return false;
}

template <class I> void DataState<I>::setBoundValues(double time,I index)
{
	if(maxTime_ < 0)
	{//Bound values were not yet initialized.
		maxTime_ = time;
		minTime_ = time;
		maxIndex_ = index;
		minIndex_ = index;
		return;
	}
	if((time >= getPostMaxTime()) || (index >= getPostMaxIndex()))
	{
		maxTime_ = time;
		maxIndex_ = index;
		return;
	}
	if((time <= getPreMinTime()) || (index <= getPreMinIndex()))
	{
		minTime_ = time;
		minIndex_ = index;
	}
}

//Functions below clear all IndexedData before/after the input index for memory savings.
template <class I> void DataState<I>::clearDataBefore(double time)
{
	if(index <= getPreMinTime())
		return;
	if(index > getCurrentIndex())
		return;	//Don't allow clearing if it would remove the current cell.
	if(index > getLastIndex())
	{
		reset();
	}
	else
	{
		int clearBeforeCell = findIndexCell(index);
		if(pbDataList_[clearBeforeCell].index_ < index)
			clearBeforeCell++;
		pbDataList_ = pbDataList_.mid(clearBeforeCell);
		currentDataCell_ = currentDataCell_-clearBeforeCell;
		minTime_ = pbDataList_.first()->time_;
		minIndex_ = pbDataList_.first()->index_;
	}	
}

template <class I> void DataState<I>::clearDataAfter(double time)
{
	if(index >= getPostMaxTime())
		return;
	if(index < getCurrentIndex())
		return;	//Don't allow clearing if it would remove the current cell.
	if(index < getFirstIndex())
	{
		reset();
	}
	else
	{
		int clearAfterCell = findIndexCell(index,0,pbDataList_.size()-1);
		pbDataList_ = pbDataList_.mid(0,clearThisCellAndAfter+1);
		maxTime_ = pbDataList_.last()->time_;
		maxIndex_ = pbDataList_.last()->index_;
	}
}

template <class I> void DataState<I>::reset()
{
	pbDataList_.clear();
	currentDataCell_ = -1;
	maxTime_ = -1;
	maxIndex_ = -1;
	minTime_ = -1;
	minIndex_ = -1;
}

template <class I> QSharedPointer<IndexedData<I>> DataState<I>::getCurrentValue()
{
	if(currentDataCell_ < 0)
		return QSharedPointer<IndexedData<I>>();
	return pbDataList_[currentDataCell_];
}

template <class I> QSharedPointer<IndexedData<I>> DataState<I>::getPrevValue()
{
	if(currentDataCell_ <= 0)
		return QSharedPointer<IndexedData<I>>();
	return pbDataList_[currentDataCell_-1];
}

template <class I> QSharedPointer<IndexedData<I>> DataState<I>::getNextValue()
{
	if(currentDataCell_ == pbDataList_.size()-1)
		return QSharedPointer<IndexedData<I>>();
	return pbDataList_[currentDataCell_+1];
}

template <class I> QList<QSharedPointer<IndexedData<I>>> DataState<I>::getValuesSince(I index)
{
	if(index >= getLastIndex())
		return QList<QSharedPointer<IndexedData<I>>>;
	int dataFromCell = findIndexCell(index,0,pbDataList_.size()-1)+1;
	return pbDataList_.mid(dataFromCell);
}

template <class I> QList<QSharedPointer<IndexedData<I>>> DataState<I>::getValuesUntil(I index)
{
	if(index < getFirstIndex())
		return QList<QSharedPointer<IndexedData<I>>>;
	int dataToCell = findIndexCell(index,0,pbDataList_.size()-1);
	return pbDataList_.mid(0,dataToCell+1);
}

template <class I> I DataState<I>::getCurrentIndex()
{
	if(currentDataCell_ >= 0)
		return pbDataList_[currentDataCell_]->index_;
	return 0;
}

template <class I> I DataState<I>::getPrevIndex()
{
	if(currentDataCell_ <= 0)
	{
		requestMoreData(getPreMinIndex());	//Try to get more data
		if(currentDataCell_ <= 0)
			return 0;	//No more data available.
	}
	return pbDataList_[currentDataCell_-1]->index_;
}

template <class I> I DataState<I>::getNextIndex()
{
	if(currentDataCell_ >= pbDataList_.size()-1)
	{
		requestMoreData(getPostMaxIndex());	//Try to get more data
		if(currentDataCell_ >= pbDataList_.size()-1)
			return 0;	//No more data available.
	}
	return pbDataList_[currentDataCell_+1]->index_;
}

template <class I> I DataState<I>::getFirstIndex()
{
	if(pbDataList_.size())
		return pbDataList_.first()->index_;
	return 0;
}

template <class I> I DataState<I>::getLastIndex()
{
	if(pbDataList_.size())
		return pbDataList_.last()->index_;
	return 0;
}
template <class I> I DataState<I>::getPostMaxIndex(){return 0;};

//Multiple template function specializations should be defined in cpp file.
template <> char DataState<char>::getPostMaxIndex();
template <> double DataState<double>::getPostMaxIndex();

template <class I> I DataState<I>::getPostMaxTime()
{
	return maxTime_;
}

template <class I> I DataState<I>::getPreMinIndex(){return 0;};

//Multiple template function specializations should be defined in cpp file.
template <> qulonglong DataState<qulonglong>::getPreMinIndex();
template <> double DataState<double>::getPreMinIndex();

template <class I> I DataState<I>::getPreMinTime()
{
	return minTime_;
}

template <class I> bool DataState<I>::moveToNext(bool last)
{
	if(currentDataCell_ >= pbDataList_.size()-1)
		return false;
	moveToCell(currentDataCell_+1,last);
}

template <class I> bool DataState<I>::moveToPrev(bool last)
{
	if(currentDataCell_ <= 0)
		return false;
	moveToCell(currentDataCell_-1,last);
}

template <class I> bool DataState<I>::moveToCell(int cellId,bool last)
{
	if(cellId < 0 || cellId >= pbDataList_.size())
		return false;
	int lastCellId = currentDataCell_;
	currentDataCell_ = cellId;
	bool reverse = lastCellId > currentDataCell_;
	triggerValueChange(reverse,last);
	//In case there are more than one values with the same index,
	//move the currentDataCell_ to the last one while triggering
	//changes all the way through.
	while(getCurrentIndex() == getNextIndex())
	{
		currentDataCell_++;
		triggerValueChange(reverse,last);
	}
	return true;
}

template <class I> I DataState<I>::get2NextIndex()
{
	if(currentDataCell_ >= pbDataList_.size()-2)
		return 0;
	return pbDataList_[currentDataCell_+2]->index_;
}

template <class I> int DataState<I>::findIndexCell(qulonglong index)
{
	if(index >= getPostMaxIndex())
	{
		requestMoreData(index);
		if(index >= getPostMaxIndex())
			return -1;
	}
	else if(index <= getPreMinIndex())
	{
		requestMoreData(index);
		if(index <= getPreMinIndex())
			return -1;
	}else if(pbDataList_.isEmpty())
		return -1;
	return binaryIndexSearch(index,0,pbDataList_.size()-1);
}

template <class I> int DataState<I>::binaryIndexSearch(qulonglong index,int minCell, int maxCell)
{
	//Stop condition
	if(maxCell == minCell)
	{
		I sIndex = pbDataList_[minCell]->index_;
		if(sIndex <= index)
		{
			//In case there is more than one value with the same index, move backward until reaching
			//the first.
			while((minCell > 0) && (pbDataList_[minCell-1]->index_ == sIndex))
				minCell--;
			return minCell;
		}
	}
	//Reduce search space
	int midCell = (maxCell+minCell)/2;
	if(pbDataList_[midCell]->index_ <= index)
		return binaryIndexSearch(index,midCell,maxCell);
	return binaryIndexSearch(index,minCell,midCell);
}

}; //namespace Picto
#endif