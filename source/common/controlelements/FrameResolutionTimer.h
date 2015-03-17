#ifndef _FRAMERESOLUTIONTIMER_H_
#define _FRAMERESOLUTIONTIMER_H_
#include <QDateTime>
#include <QHash>

#include "FrameTracker.h"
#include "../common.h"

#ifdef WIN32
#include "Windows.h"
#endif
namespace Picto {
namespace Controller {

/*! \brief A timer object that returns times in single frame increments.
 *
 *	ControlElement objects and other sections of Picto require precise timing aligned with frame intervals.  This object
 *	serves that purpose.  The timer always returns times according to the instant at which the latest first phosphor
 *	occured.  FrameResolutionTimer objects require an underlying FrameTracker object which provides frame timing
 *	information. The FrameResolutionTimer is created by a FrameTimerFactory, and this factory provides the FrameTracker
 *	instance.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015	
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API FrameResolutionTimer
#else
class FrameResolutionTimer
#endif
{

public:
	FrameResolutionTimer(QSharedPointer<FrameTracker> frameTracker);
	~FrameResolutionTimer();

	void start();
	/*! \brief Restarts the timer.  Currently this is functionaly equivalent to start() */
	void restart() { start(); };

	void pause();
	void resume();

	int elapsedTime(Controller::TimerUnits::TimerUnits units);

private:
	double startTime_;
	double stopTime_;
	QSharedPointer<FrameTracker> frameTracker_;
};

}; //namespace ControlElement
}; //namespace Picto

#endif
