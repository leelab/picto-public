#ifndef _FRAMERESOLUTIONTIMER_H_
#define _FRAMERESOLUTIONTIMER_H_
#include <QDateTime>

#include "../common.h"

#ifdef WIN32
#include "Windows.h"
#endif
namespace Picto {
namespace Controller {

namespace TimerUnits
{
	/*! \brief An enumerated type containing timing control options for PictoEngine objects
	 *
	 * PictoEngine objects can be set to have precise timing control, or to ignore frame intervals
	 * (to catch up a playback, allow for fast forward and rewind behaviors, enable generation of movie
	 * files, allow analysis of replayed data, etcetera).  PictoEngineTimingType enumerates the
	 * valid timing options.
	 * 
	 */
	typedef enum
	{
		sec,	//seconds
		ms,		//milliseconds
		us		//microseconds
	} TimerUnits;
}

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
	FrameResolutionTimer();

	void start();
	void restart() { start(); };

	int elapsedTime(TimerUnits::TimerUnits units);

	//Called by the rendering engine to mark the time at which the latest frame occured.
	//This is used by all timers in calculating elapsedTime
	static void setLastFrameTime(double frameTime);

private:
	double startTime_;
	static double lastFrameTime_;
	static double firstFrameTime_;

};

}; //namespace ControlElement
}; //namespace Picto

#endif
