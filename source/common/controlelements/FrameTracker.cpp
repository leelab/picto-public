#include <QObject>
#include "FrameTracker.h"


namespace Picto {
namespace Controller {

FrameTracker::FrameTracker()
{
	lastFrameTime_ = -1;
	nextFrameTime_ = -1;
	firstFrameTime_ = -1;
}

/*! \brief Stores the latest frame time in this object.
 */
void FrameTracker::setLastFrameTime(double frameTime)
{
	if(firstFrameTime_ < 0)
		firstFrameTime_ = frameTime;
	lastFrameTime_ = frameTime;
	
}

/*! \brief Sets the time of the next frame that will appear.
 *	\details This is not available in live experiments, but during playback it can
 *	be provided.
 */
void FrameTracker::setNextFrameTime(double frameTime)
{
	nextFrameTime_ = frameTime;
}

/*! \brief Returns the time that the next frame will occur relative to the beginning of the experiment.
 *	\details If a live experiment is running, this time will be estimated as 16.666ms after the current time.
 *	\note Currently, we are not actually using effectiveAbsoluteTime anywhere meaningful.  Think twice before 
 *	starting to use this.
 *	\sa FrameTimerFactory::effectiveAbsoluteTime()
 */
int FrameTracker::effectiveAbsoluteTime(TimerUnits::TimerUnits units)
{
	double returnTime = nextFrameTime_;
	if(nextFrameTime_ <= lastFrameTime_)
	{	//Looks like we need to estimate the return value.
		returnTime = lastFrameTime_+16.666;
	}

	if(units == TimerUnits::sec)
		return returnTime;
	else if(units == TimerUnits::ms)
		return returnTime * 1000.0;
	else if(units == TimerUnits::us)
		return returnTime * 1000000.0;
	else
	{
		Q_ASSERT(false);
		return 0;
	}
		
}

/*! \brief Resets all frame time values to their initial conditions.
 */
void FrameTracker::reset()
{
	lastFrameTime_ = -1;
	nextFrameTime_ = -1;
	firstFrameTime_ = -1;
}


}	//namespace Picto
}	//namespace ControlElement
