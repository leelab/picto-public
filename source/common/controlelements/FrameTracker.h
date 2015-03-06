#ifndef _FrameTracker_H_
#define _FrameTracker_H_

#include "../common.h"

namespace Picto {
namespace Controller {

namespace TimerUnits
{
	/*! \brief An enumerated type containing timing units
	 * 
	 */
	typedef enum
	{
		sec,	//seconds
		ms,		//milliseconds
		us		//microseconds
	} TimerUnits;
}

/*! \brief Stores frame timing information for use by FrameResolutionTimer objects.
 *	The FrameTracker object used in a particular experiment is created by the FrameTimerFactory.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015	
 */
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
