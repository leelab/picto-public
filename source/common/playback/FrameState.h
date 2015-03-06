#ifndef _FRAMESTATE_H_
#define _FRAMESTATE_H_
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Implements the DataState and FrameReader classes to load a Picto Session database, 
 *	extract the frame data and implement functions for traversing through that data.
 *	\details The class is fairly simple, a QList of PlaybackIndex objects is loaded from the 
 *	session data.  Each PlaybackIndex represents a frame presentation and when moveToIndex() 
 *	is called, we just traverse through the list until we reach the appropriate PlaybackIndex.
 *	Each time moveToIndex() causes us to pass through a PlaybackIndex, the framePresented() 
 *	signal is called, which tells the rest of the playback system that a frame occured.
 *
 *	\note Since the functions here simply implement the FrameReader and DataState classes for
 *	data read in from a Session Database, there is not much to add in terms of documentation 
 *	beyond what was described above, so we will not be adding function level documentation
 *	for this class.
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class FrameState : public FrameReader, public DataState
{
	Q_OBJECT
public:
	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);

	//Frame Reader Interface
	virtual double getPrevTime();
	virtual double getLatestTime();
	virtual double getNextTime();
	virtual QVariantList getTimesSince(double time);
	virtual QVariantList getTimesUntil(double time);
	
signals:
	/*! \brief Emitted whenever a new frame presentation is traveresed due to a call to moveToIndex().
	 *	\details The "time" input is the frame time.
	*/
	void framePresented(double time);

private:
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	double runStart_;
	double runEnd_;
	int curr_;
	QList<PlaybackIndex> data_;
};

}; //namespace Picto
#endif