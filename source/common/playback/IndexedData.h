#ifndef _INDEXEDDATA_H_
#define _INDEXEDDATA_H_
#include <QPair>

namespace Picto {
/*! \brief This class stores an index used to capture the ordering of different Session events for playback purposes.
 *	\details This class is extremely important for the Playback system.  Every event in Picto occurs with some kind 
 *	of a timestamp, or if doesn't have a timestamp it has a reference to something with a timestamp like a frameId.
 *	The timestamp itself is not enough to capture the precise ordering of Picto events however.  For example,
 *	any two Properties who's values changed during the same frame are marked with the same frameId and therefore the
 *	same timestamp.  It is important to know which Property value changed first though, and whether it occured
 *	before or after a particular transition for the purposes of Analysis.  For this reason, we need a second index that 
 *	we can use to capture ordering of behavioral events.  This is the DataId field that is stored along with every row 
 *	of session data in the session file.  By using both of these indeces, time and DataId we can capture the precise ordering
 *	of events in the session file.
 *
 *	There are some data points that don't have dataId's however, such as spikes and lfp values which come from a
 *	different computer all together.  Input signals like position also don't come with a dataId, but are marked only
 *	with the time that they came in.  In the case of these signals, for comparison purposes we can simply use
 *	a minForTime() or maxForTime() function to create a PlaybackIndex with the minimum or maximum DataID values.
 *	That way we can be sure that a particular value will be either the first or last marked event for a given time.
 *
 *	This class extends QPair<double,qulonglong> which provides all of the necessary comparison operators so that
 *	comparison of two PlaybackIndex values will look first at the time value and then at the dataId.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct PlaybackIndex: public QPair<double,qulonglong>
{
	/*! \brief Constructs an invalid PlaybackIndex (time < 0, dataId = 0).*/
	PlaybackIndex(){first = -1;second = 0;};
	/*! \brief Constructs a PlaybackIndex with the input time and dataId (called pos to indicate that this is the position within the input time).
	 *	\details Possibly the pos input name should be changed to dataId for clarity.
	 */
	PlaybackIndex(double time,qulonglong pos):QPair<double,qulonglong>(time,pos){};
	/*! \brief Returns true if this PlaybackIndex contains a valid value.*/
	bool isValid(){return second > 0;};
	/*! \brief Returns the time component of this PlaybackIndex.*/
	double& time(){return first;};
	/*! \brief Returns the pos component of this playback index (called pos to indicate that this is the position within the input time).
	 *	\details Possibly this function should be renamed to dataId() for clarity.
	 */
	qulonglong& pos(){return second;};
	/*! \brief Returns the next PlaybackIndex after this one for the same time.*/
	PlaybackIndex nextIndexSameTime(){return PlaybackIndex(first,second+1);};
	/*! \brief Returns the previous PlaybackIndex before this one for the same time.*/
	PlaybackIndex prevIndexSameTime(){return PlaybackIndex(first,second-1);};
	/*! \brief Returns the lowest possible PlaybackIndex for the input time.*/
	static PlaybackIndex minForTime(double time){return PlaybackIndex(time,1);};
	/*! \brief Returns the highest possible PlaybackIndex for the input time.*/
	static PlaybackIndex maxForTime(double time){return PlaybackIndex(time,0xFFFFFFFFFFFFFFFF);};
};

/*! \brief A lot has changed since the original Picto Playback Design, this class was part of an attempt
 *	to make code of different DataState types very reusable.  In the end, more code reusability led to
 *	less efficiency in terms of timing and memory space and we opted for a more tailored approach
 *	with each of the DataState classes doing a lot more of its own work to handle the DataState interface
 *	in the most efficient way possible.
 *
 *	We should probably remove this code.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct IndexedData
{
	IndexedData(){};
	IndexedData(PlaybackIndex index){index_ = index;};
	virtual ~IndexedData(){};
	PlaybackIndex index_;
};
}; //namespace Picto
#endif