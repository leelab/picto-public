#ifndef _LiveFrameReader_H_
#define _LiveFrameReader_H_
#include <QVector>
#include "../../common/playback/PlaybackInterfaces.h"

namespace Picto {
/*! \brief Implements the FrameReader class for a live test experiment.
 *	\details This object is filled during run time by using the setLatestFrameTime() function such that Analyses 
 *	have access to all past data but no future data.  Requests for future data from this class will result in 
 *	meaningless values.
 *	\note Since the functions here simply implement the FrameReader class for
 *	data read in during a live Session, there is not much to add in terms of documentation 
 *	beyond what was described above, so we will not be adding additional function level documentation
 *	for many of the functions in this class.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
	QVector<double> frameTimes_;
};

}; //namespace Picto
#endif