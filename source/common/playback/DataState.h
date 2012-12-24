#ifndef _DATASTATE_H_
#define _DATASTATE_H_
#include <QSharedPointer>
#include <QPair>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QMutexLocker>

#include "IndexedData.h"
#include "LinkedDataStateList.h"
#include "VectorDataStateList.h"
#include "ListDataStateList.h"

namespace Picto {

class DataState
{
public:
	DataState(){};
	virtual ~DataState(){};

	virtual void setDatabase(QSqlDatabase session) = 0;
	virtual void startRun(double runStartTime,double runEndTime = -1) = 0;
	//CONFIGURE FUNCTIONS-----------------------------------------------------------

	//Gets the current index of this DataState
	virtual PlaybackIndex getCurrentIndex() = 0;

	//Gets the next index available in this DataState after the current one
	//@param lookForwardTime The time to look forward to for the
	//	next value.  Note that this function will request data if the look forward
	//	time cannot be met with current data and this may cause some delay in cases
	//	where data is not yet available (such as live playback).  Using an input
	//	value less than zero is equivalent to inputing a value of infinity.  The function
	//	will look forward as far as it takes until a new value is available (which
	//	may cause some significant delay).
	virtual PlaybackIndex getNextIndex(double lookForwardTime) = 0;

	virtual void moveToIndex(PlaybackIndex index) = 0;
};

/*! \brief Component of Picto Playback system that stores IndexedData values.
 *	This class stores indexed playback data and offers several functions for
 *  querying that data.
 */
class AutoDataState : public DataState
{
public:
	AutoDataState();
	virtual ~AutoDataState();

	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);

protected:
	virtual bool querySessionForData(QSharedPointer<QSqlQuery> query,double after,double upTo) = 0;
	virtual QVector<QSharedPointer<IndexedData>> convertQueryToDataVector(QSharedPointer<QSqlQuery> query, double zeroTime) = 0;
	virtual void triggerDataChange(QSharedPointer<IndexedData> data) = 0;

private:
	void reset();
	QSharedPointer<IndexedData> getCurrentValue();
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	QVector<QSharedPointer<IndexedData>> currQueryData_;
	QVector<QSharedPointer<IndexedData>> nextQueryData_;
	int dataIndex_;
	double runStartTime_;
	double runEndTime_;
	double lastQueryEndTime_;
	//Returns the index of the first data unit tracked by this DataState
	//PlaybackIndex getFirstIndex();

	//Returns the index of the last data unit tracked by this DataState
	//PlaybackIndex getLastIndex();

//protected:
//	//IndexedData values must be set either in increasing index order after latest
//	//or in decreaseing index order before the first
//	void setValue(QSharedPointer<IndexedData> pbData);
//	//Configures the AutoDataState such that when functions are called requesting data
//	//or changing the active data state it waits until enough data has been added
//	//to procede.  If this is not enabled, the AutoDataState will function as if it has
//	//enough data to procede.  (ie. SetCurrentIndex(infinity) will set the current
//	//index to the highest available location in the data list.
//	//Default is enabled.
//	void shouldWaitForData(bool wait);
//
//	//@param reverse indicates if the value was reached by
//	//moving the current index backward (ie. back in time)
//	//@param last indicates that this is the last iteration
//	//from the current setCurrentIndex() call.
//	virtual void triggerValueChange(bool reverse,bool last) = 0;
//	//Tells child classes that we need more data in order to
//	//carry out a request.
//	//@param currLast indicates the last index that we currently have available
//	//@param to indicates the index to which we are requesting data
//	virtual void requestMoreData(PlaybackIndex currLast,PlaybackIndex to) = 0;
//	//Tells child classes that we need at least one more data entry after/before
//	//the currLast entry.
//	//carry out a request.
//	//@param currLast indicates the last/first index that we currently have available
//	//@param backward indicates the direction of the data request, before or after.
//	virtual void requestNextData(PlaybackIndex currLast,bool backward) = 0;
//
//private:
//
//	//Sets the maximum/minimum PlaybackIndeces for the window in which this AutoDataState can
//	//provide valid data.  This is important because it lets the AutoDataState know if a lack 
//	//of available data is a result data not having been loaded yet or to a simple lack of 
//	//data points.
//	//Note: The value is automatically detected as a max or min and set accordingly.
//	void setBoundIndex(PlaybackIndex index);
//	//Data may not be available after getLastIndex() but there may just not have been 
//	//any physical data in that region.  getMaxIndex() gets index after which this
//	//DataState's data is no longer sufficient.
//	PlaybackIndex getMaxIndex();
//	PlaybackIndex getMinIndex();
//
//	//Moves current cell to the next one.
//	//@param last Indicates that this is the last iteration
//	//to move to the current index
//	bool moveToNext(bool last);
//	//Informs child class that value change occured.
//	bool triggerChange(bool last);
//	//Returns the index of the cell two cells after the current one.
//	PlaybackIndex get2NextIndex();
//	//Returns true if the input index is within the data limits of this DataState
//	bool inDataWindow(PlaybackIndex index);
//	//Yields this thread until sufficient data is available to fulfil requests.
//	//Note: Assumes mutex is locked when called.
//	void waitForSufficientData(QMutexLocker* mutexLocker,PlaybackIndex dataIndex);
//
//	//Separating functions from the public interface into public and private versions
//	//allows us to enact mutex locking when the function is called from outside of the
//	//object
//	QSharedPointer<IndexedData> getCurrentValuePriv();
//	QSharedPointer<IndexedData> getPrevValuePriv();
//	QSharedPointer<IndexedData> getNextValuePriv();
//	QList<QSharedPointer<IndexedData>> getValuesSincePriv(double time);
//	QList<QSharedPointer<IndexedData>> getValuesUntilPriv(double time);
//	PlaybackIndex getCurrentIndexPriv();
//	PlaybackIndex getPrevIndexPriv();
//	PlaybackIndex getNextIndexPriv();
//	PlaybackIndex getFirstIndexPriv();
//	PlaybackIndex getLastIndexPriv();
//	
//	//Data Fields
//	bool finishedLoading_;
//	PlaybackIndex maxIndex_;
//	PlaybackIndex minIndex_;
//	bool iterateOnMove_;
//	//The minimum amount of time for additional data requests from this DataState
//	double bufferTime_;
//	//List of IndexedData items ordered by their index.
//	LinkedDataStateList pbDataList_;
//	bool waitForData_;
//	QSharedPointer<QMutex> mutex_;	//Data writes and reads happen in different threads.  This sorts out threading issues.
};


}; //namespace Picto
#endif