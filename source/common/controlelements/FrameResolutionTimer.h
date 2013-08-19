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

/*! \brief A timer object used by ControlElements
 *
 *	The controlElements require precise timing aligned with frame intervals.  This object serves that
 *	purpose.  The values that the timer returns are according to the time at which the latest first
 *	phosphor occured.
 *
 *	NOTE: At the moment, this timer is part of the ControlElement namespace, to
 *	indicate that it is designed for use with ControlElements only.
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
	void restart() { start(); };

	int elapsedTime(Controller::TimerUnits::TimerUnits units);

private:
	double startTime_;
	QSharedPointer<FrameTracker> frameTracker_;
};

}; //namespace ControlElement
}; //namespace Picto

#endif
