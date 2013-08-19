#ifndef _FrameTracker_H_
#define _FrameTracker_H_

#include "../common.h"

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

struct FrameTracker
{
	FrameTracker();
	void setLastFrameTime(double frameTime);
	void setNextFrameTime(double frameTime);
	int effectiveAbsoluteTime(TimerUnits::TimerUnits units);
	void reset();

	double lastFrameTime_;
	double firstFrameTime_;
	double nextFrameTime_;
};

}; //namespace ControlElement
}; //namespace Picto

#endif
