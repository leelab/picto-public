#ifndef _DATASTATE_H_
#define _DATASTATE_H_
#include <QSharedPointer>
#include <QPair>

#include "IndexedData.h"

namespace Picto {
/*! \brief Component of Picto Playback system that stores IndexedData values.
 *	This class stores indexed playback data and offers several functions for
 *  querying that data.
 */
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
	void setValue(QSharedPointer<IndexedData> pbData);
	//Sets the current index of this DataState to the largest value available lower than
	//or equal to the input index.  If not enough data is available to reach that position, 
	//requestMoreData will be called.  If this call fails to provide sufficient data.  False
	//will be returned.
	bool setCurrentIndex(PlaybackIndex index);
	//Functions below clear all IndexedData before/after the input time for memory savings.
	void clearDataBefore(double time);
	void clearDataAfter(double time);
	//Resets this Data State back to its initial condition
	virtual void reset();

	//GET FUNCTIONS-----------------------------------------------------------------
	//Gets the current IndexedData item.
	QSharedPointer<IndexedData> getCurrentValue();
	//Gets the previous IndexedData item.
	//@param lookBackTime The maximum amount of time to look back for the
	//	previous value. Using an input value less than zero will cause the
	//	system to request at least one prior value if it is not yet available
	//	no matter how far back it appears.
	QSharedPointer<IndexedData> getPrevValue(double lookBackTime);
	//Gets the next IndexedData item
	//@param lookForwardTime The time to look forward to for the
	//	next value.  Note that this function will request data if the look forward
	//	time cannot be met with current data and this may cause some delay in cases
	//	where data is not yet available (such as live playback).  Using an input
	//	value less than zero is equivalent to inputing a value of infinity.  The function
	//	will look forward as far as it takes until a new value is available (which
	//	may cause some significant delay).
	QSharedPointer<IndexedData> getNextValue(double lookForwardTime);

	//Returns a list of IndexedData with times >= the input time and appearing before the current value.  
	QList<QSharedPointer<IndexedData>> getValuesSince(double time);
	//Returns a list of IndexedData appearing after the current value and <= the input time.  
	QList<QSharedPointer<IndexedData>> getValuesUntil(double time);

	//Gets the current index of this DataState
	PlaybackIndex getCurrentIndex();

	//Gets the previous index available in this DataState before the current one
	//@param lookBackTime The maximum amount of time to look back for the
	//	previous value.  Using an input value less than zero will cause the
	//	system to request at least one prior value if it is not yet available
	//	no matter how far back it appears.
	PlaybackIndex getPrevIndex(double lookBackTime);

	//Gets the next index available in this DataState after the current one
	//@param lookForwardTime The time to look forward to for the
	//	next value.  Note that this function will request data if the look forward
	//	time cannot be met with current data and this may cause some delay in cases
	//	where data is not yet available (such as live playback).  Using an input
	//	value less than zero is equivalent to inputing a value of infinity.  The function
	//	will look forward as far as it takes until a new value is available (which
	//	may cause some significant delay).
	PlaybackIndex getNextIndex(double lookForwardTime);

	//Returns the index of the first data unit tracked by this DataState
	PlaybackIndex getFirstIndex();

	//Returns the index of the last data unit tracked by this DataState
	PlaybackIndex getLastIndex();

protected:
	//@param reverse indicates if the value was reached by
	//moving the current index backward (ie. back in time)
	//@param last indicates that this is the last iteration
	//from the current setCurrentIndex() call.
	virtual void triggerValueChange(bool reverse,bool last) = 0;
	//Tells child classes that we need more data in order to
	//carry out a request.
	//@param currLast indicates the last index that we currently have available
	//@param to indicates the index to which we are requesting data
	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to) = 0;
	//Tells child classes that we need at least one more data entry after/before
	//the currLast entry.
	//carry out a request.
	//@param currLast indicates the last/first index that we currently have available
	//@param backward indicates the direction of the data request, before or after.
	virtual void requestNextData(PlaybackIndex currLast,bool backward) = 0;

private:
	//Sets the maximum/minimum PlaybackIndeces for the window in which this DataState can
	//provide valid data.  This is important because it lets the DataState know if a lack 
	//of available data is a result data not having been loaded yet or to a simple lack of 
	//data points.
	//Note: The value is automatically detected as a max or min and set accordingly.
	void setBoundValues(PlaybackIndex index);
	//Data may not be available after getLastIndex() but there may just not have been 
	//any physical data in that region.  getMaxIndex() gets index after which this
	//DataState's data is no longer sufficient.
	PlaybackIndex getMaxIndex();
	PlaybackIndex getMinIndex();

	//Moves current cell to the next one.
	//@param last Indicates that this is the last iteration
	//to move to the current index
	bool moveToNext(bool last);
	//Moves current cell to the previous one.
	//@param last Indicates that this is the last iteration
	//to move to the current index
	bool moveToPrev(bool last);
	//Moves to cell at input cellId without iterating through intervening
	//cells (regardless of iterateOnMove_).
	//@param last Indicates that this is the last iteration
	//to move to the current index
	bool moveToCell(int cellId,bool last);
	//Returns the index of the cell two cells after the current one.
	PlaybackIndex get2NextIndex(double lookForwardTime);
	//Returns the cell number of the cell with the highest index less than or equal to
	//the input.
	int findIndexCell(PlaybackIndex index);
	//Recursively searches for the highest indexed cell with index <= input index
	//within the input window
	int binaryIndexSearch(PlaybackIndex index,int minCell, int maxCell);
	//Assures that this object has all data available between the input Indeces
	void assureMinDataWindow(double lowTime,double highTime);
	
	//Data Fields
	PlaybackIndex maxIndex_;
	PlaybackIndex minIndex_;
	int currentDataCell_;
	bool iterateOnMove_;
	//List of IndexedData items ordered by their index.
	QList<QSharedPointer<IndexedData>> pbDataList_;
};


}; //namespace Picto
#endif