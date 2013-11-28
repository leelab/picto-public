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

/*! \brief An interface defining many common aspects of classes that extract
 *	the various data states from a Picto Session for playback purposes.
 *	\details "DataState" may not have been the best name for this class.  Possibly, something
 *	like "SessionReader" would have made more sense.  This interface provides functions for
 *	setting a Session database to underlying classes so that they can load the data that they
 *	care about into RAM.  It also provides methods for initializing the class for starting
 *	a new run and for moving through the underlying data.
 *
 *	Everything here depends on the PlaybackIndex class which is a simple class that handles
 *	timing and ordering data.  We can't simply use timestamps in Picto because everything that
 *	happens between two frames can be considered as happening at the same time.  The PlaybackIndex
 *	class uses DataId information too to make sure that it gets ordering correct for different
 *	pieces of Picto data.  See PlaybackIndex for more detail.
 *
 *	\note One important thing to keep in mind in DataState classes is that all motion between
 *	different values must occur continuously, without jumps.  This means that if we tell a Frame
 *	DataState to move to an input PlaybackIndex hat is 5 minutes away, it shouldn't use any kind of efficient binary jumping
 *	algorithm, it should just move step by step through the data until it reaches the input PlaybackIndex.
 *	This is because the DataState objects report changes in their data as they step through them and
 *	for the purposes of proper playback and valid Analyses we want to make sure that even if someone
 *	is trying to "fast forward" through data, all intermediary data states are traversed before the
 *	requested position is reached.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class DataState
{
public:
	DataState(){};
	virtual ~DataState(){};

	/*! \brief Sets the input Sql Database to this DataState so that it can load all of the 
	 *	data relevant to it into RAM for quick access.
	 */
	virtual void setDatabase(QSqlDatabase session) = 0;
	/*! \brief Informs this DataState that a new run is starting along with the time
	 *	that the run is starting and the time that it will end (if available).  Times are
	 *	in seconds.
	 *	\details This is important because PlaybackIndex values returned from DataState
	 *	objects are always with respect to the beginning of the run.  In otherwords, if
	 *	a DataState returns a PlaybackIndex of something that happened at the same time
	 *	that the RunStarted, its PlaybackIndex.time() will be zero, even if the run happened
	 *	at time 213.315.
	 */
	virtual void startRun(double runStartTime,double runEndTime = -1) = 0;
	//CONFIGURE FUNCTIONS-----------------------------------------------------------

	/*! \brief Gets the current PlaybackIndex of this DataState.
	 *	\details DataStates, like File objects, have what we can think at as a read pointer.  They
	 *	always have a current position from which we can move.  This is for the sake of efficiency and to be
	 *	sure that every time we move from one state in a Picto session to another we are not
	 *	skipping any experimental events in the middle.
	 *	The value returned from this function is the PlaybackIndex describing the time/order of the 
	 *	value that the "read pointer" is currently pointing at.
	 *	\sa PlaybackIndex, moveToIndex()
	 */
	virtual PlaybackIndex getCurrentIndex() = 0;

	/*! \brief Gets the PlaybackIndex of the next available value after the current "read pointer" 
	 *	in this DataState.
	 *	@param lookForwardTime The timespan to search forward from the current read position for the
	 *	next value.	Using an input value less than zero is equivalent to inputing a value of infinity.
	 *	The function will look forward as far as it takes until a new value is available (which
	 *	may cause some significant delay).
	 *	\note When we designed this interface, the idea was that we could use it for live playback
	 *	and if there was no "next index" available, the system would just wait for one to come in
	 *	thereby creating an unfortunate but necessary lag in playback.  In practice, we are not
	 *	using DataStates for live playback, since we don't support Analysis during live playback.
	 *	This may be something to think about when we want to start supporting that though.
	 */
	virtual PlaybackIndex getNextIndex(double lookForwardTime) = 0;

	/*! \brief Moves the current read pointer forward step by step until the next value's
	 *	index (ie. the thing getNextIndex() will return) is greater than the input index.
	 *	\details It is important that the DataState move step by step up to the input index
	 *	because all of the changes in its value need to be emitted over signals and sent out to the
	 *	greater playback system as we move to the new value.  If we were to jump over any 
	 *	values and skip them, Analyses that are basing themselves on changes in values
	 *	underlying this DataState might no longer be valid.
	 *
	 *	Imagine, for example, if this DataState were tracking transitions.  If we skipped over
	 *	any transitions on the way to the input index, Analysis scripts that are called when
	 *	those transitions are traversed would be skipped and the Analysis would be totally invalid.
	 */
	virtual void moveToIndex(PlaybackIndex index) = 0;
};

/*! \brief The Playback system went through a lot of minor iterations during its development.
 *	AutoDataState was once going to generalize a lot of the functionality of other types of Data
 *	State's but in the end it is not actually used.
 *	\details It is inherited by EventState and ValueState, but since those are not used, this is not
 *	used either.
 *	\note We should probably just get rid of this class.
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