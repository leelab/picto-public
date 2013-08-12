#ifndef _LiveFrameReader_H_
#define _LiveFrameReader_H_
#include <QVector>
#include "../../common/playback/PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Frame PlaybackData values for use in Playback system.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API LiveFrameReader : public FrameReader
#else
class LiveFrameReader : public FrameReader
#endif
{
	Q_OBJECT
public:
	LiveFrameReader(){};
	void setRunStart();
	void setLatestFrameTime(double time);
	void setRunEnd();

	//Frame Reader Interface
	virtual double getPrevTime();
	virtual double getLatestTime();
	virtual double getNextTime();
	//Returns a list of frame times that occured with times > the input time and <= the current time.  
	virtual QVariantList getTimesSince(double time);
	//Returns a list of values with times > the current time and <= the input time.  
	virtual QVariantList getTimesUntil(double time);

private:
	double runStart_;
	QVector<double> frameTimes_;
};

}; //namespace Picto
#endif