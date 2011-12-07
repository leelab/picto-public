#ifndef _TIMER_H_
#define _TIMER_H_
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
 *	The controlElements frequently require precise timing.  This object serves that
 *	purpose.  Since the Qt library doesn't provide a good timer, this object will 
 *	have to be platform dependent.  For unsupported platfroms, we'll use the Qt timer,
 *	but for everything else, we'll use a platform dependent timer.  
 *
 *	Since controlElements are used in the rendering loop, the timers are not going to
 *	use the Qt signal/slot architecture, but instead are going to be updated as they
 *	are called.
 *
 *	NOTE: At the moment, this timer is part of the ControlElement namespace, to
 *	indicate that it is designed for use with ControlElements only.  It is possible that in
 *	the future we will want to let it out for more general usage, or we may build a 
 *	second Timer object that uses signals and slots.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API Timer
#else
class Timer
#endif
{

public:
	Timer();

	void start();
	void restart() { start(); };

	int elapsedTime(TimerUnits::TimerUnits units);

private:
#if defined WIN32 || defined WINCE
	LARGE_INTEGER ticksPerSec_;
	LARGE_INTEGER ticksPerMs_;
	LARGE_INTEGER ticksPerUs_;

	LARGE_INTEGER startTicks_;
#else
	QDateTime time_;
#endif
};

}; //namespace ControlElement
}; //namespace Picto

#endif
