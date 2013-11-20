#ifndef _FrameTimerFactory_H_
#define _FrameTimerFactory_H_
#include <QDateTime>
#include <QVector>
#include <QWeakPointer>
#include <QSharedPointer>

#include "FrameResolutionTimer.h"
#include "../common.h"

#ifdef WIN32
#include "Windows.h"
#endif
namespace Picto {
namespace Controller {

/*! \brief A Factory object used to create FrameResolutionTimer object while maintaing a single FrameTracker object to keep them update with the latest frame times.
 *	\details An Experimental Design's DesignConfig object creates and stores a single FrameTimerFactory object for its Design.
 *	All elements in the design that need a FrameResolutionTimer use this single factory to create it, and since
 *	The FrameTimerFactory creates a single FrameTracker and uses it to create all of its FrameResolutionTimers, only
 *	the single FrameTracker needs to be updated each time new first phosphor times are detected in order to provide
 *	timing data for all FrameResolutionTimers throughout the experiment.
 *	\note We could not simply use a static function in FrameResolutionTimer to update times because there can
 *	be multiple designs running simultaneously within the same application; ie. In the ReplayViewer and TestViewer.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API FrameTimerFactory
#else
class FrameTimerFactory
#endif
{

public:
	FrameTimerFactory();
	~FrameTimerFactory();

	QSharedPointer<FrameResolutionTimer> createTimer();
	void setLastFrameTime(double frameTime);
	void setNextFrameTime(double frameTime);
	int effectiveAbsoluteTime(TimerUnits::TimerUnits units);
	
	//Sets stored data back to its initial conditions and restarts all timers
	void resetAllTimers();

protected:
	QVector<QWeakPointer<FrameResolutionTimer>> timerList_;
private:
	QSharedPointer<FrameTracker> frameTracker_;

};

}; //namespace ControlElement
}; //namespace Picto

#endif
