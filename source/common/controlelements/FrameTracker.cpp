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

void FrameTracker::setLastFrameTime(double frameTime)
{
	if(firstFrameTime_ < 0)
		firstFrameTime_ = frameTime;
	lastFrameTime_ = frameTime;
	
}
void FrameTracker::setNextFrameTime(double frameTime)
{
	nextFrameTime_ = frameTime;
}

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

void FrameTracker::reset()
{
	lastFrameTime_ = -1;
	nextFrameTime_ = -1;
	firstFrameTime_ = -1;
}


}	//namespace Picto
}	//namespace ControlElement
