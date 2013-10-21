#ifndef _FRAMESTATE_H_
#define _FRAMESTATE_H_
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Frame PlaybackData values for use in Playback system.
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
	//Returns a list of frame times that occured with times > the input time and <= the current time.  
	virtual QVariantList getTimesSince(double time);
	//Returns a list of values with times > the current time and <= the input time.  
	virtual QVariantList getTimesUntil(double time);
	
signals:
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