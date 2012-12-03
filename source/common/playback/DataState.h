#ifndef _DATASTATE_H_
#define _DATASTATE_H_
#include <QSharedPointer>
#include <QPair>
#include <QMutex>
#include <QMutexLocker>

#include "IndexedData.h"
#include "LinkedDataStateList.h"
#include "VectorDataStateList.h"

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
	//CONFIGURE FUNCTIONS-----------------------------------------------------------
	//Resets this Data State back to its initial condition
	virtual void reset();

	//SET STORED DATA FUNCTIONS-----------------------------------------------------------------
	//Sets the minimum and maximum times for which data is valid in this DataState.
	//If, for example, there is only one spike in a session, the DataState needs to
	//know that the one spike consitutes all data from time n to time m.  That is what
	//this is for.
	virtual void setBoundTimes(double minTime,double maxTime);
	virtual void setFinishedLoading();

	//Functions below clear all IndexedData before the input time for memory savings.
	virtual void clearDataBefore(double time);

	//SET ACTIVE DATA FUNCTIONS-----------------------------------------------------------------
	//Sets the current index of this DataState to the largest value available lower than
	//or equal to the input index.  If not enough data is available to reach that position, 
	//requestMoreData will be called.  If this call fails to provide sufficient data.  False
	//will be returned.
	bool setCurrentIndex(PlaybackIndex index);

	//GET FUNCTIONS-----------------------------------------------------------------
	//Gets the current IndexedData item.
	QSharedPointer<IndexedData> getCurrentValue();
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
	//IndexedData values must be set either in increasing index order after latest
	//or in decreaseing index order before the first
	void setValue(QSharedPointer<IndexedData> pbData);
	//Configures the DataState such that when functions are called requesting data
	//or changing the active data state it waits until enough data has been added
	//to procede.  If this is not enabled, the DataState will function as if it has
	//enough data to procede.  (ie. SetCurrentIndex(infinity) will set the current
	//index to the highest available location in the data list.
	//Default is enabled.
	void shouldWaitForData(bool wait);

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
	void setBoundIndex(PlaybackIndex index);
	//Data may not be available after getLastIndex() but there may just not have been 
	//any physical data in that region.  getMaxIndex() gets index after which this
	//DataState's data is no longer sufficient.
	PlaybackIndex getMaxIndex();
	PlaybackIndex getMinIndex();

	//Moves current cell to the next one.
	//@param last Indicates that this is the last iteration
	//to move to the current index
	bool moveToNext(bool last);
	//Informs child class that value change occured.
	bool triggerChange(bool last);
	//Returns the index of the cell two cells after the current one.
	PlaybackIndex get2NextIndex();
	//Returns true if the input index is within the data limits of this DataState
	bool inDataWindow(PlaybackIndex index);
	//Yields this thread until sufficient data is available to fulfil requests.
	//Note: Assumes mutex is locked when called.
	void waitForSufficientData(QMutexLocker* mutexLocker,PlaybackIndex dataIndex);

	//Separating functions from the public interface into public and private versions
	//allows us to enact mutex locking when the function is called from outside of the
	//object
	QSharedPointer<IndexedData> getCurrentValuePriv();
	QSharedPointer<IndexedData> getPrevValuePriv();
	QSharedPointer<IndexedData> getNextValuePriv();
	QList<QSharedPointer<IndexedData>> getValuesSincePriv(double time);
	QList<QSharedPointer<IndexedData>> getValuesUntilPriv(double time);
	PlaybackIndex getCurrentIndexPriv();
	PlaybackIndex getPrevIndexPriv();
	PlaybackIndex getNextIndexPriv();
	PlaybackIndex getFirstIndexPriv();
	PlaybackIndex getLastIndexPriv();
	
	//Data Fields
	bool finishedLoading_;
	PlaybackIndex maxIndex_;
	PlaybackIndex minIndex_;
	bool iterateOnMove_;
	//The minimum amount of time for additional data requests from this DataState
	double bufferTime_;
	//List of IndexedData items ordered by their index.
	LinkedDataStateList pbDataList_;
	bool waitForData_;
	QSharedPointer<QMutex> mutex_;	//Data writes and reads happen in different threads.  This sorts out threading issues.
};


}; //namespace Picto
#endif