#include "FrameResolutionTimer.h"


namespace Picto {
namespace Controller {

double FrameResolutionTimer::lastFrameTime_ = -1;
double FrameResolutionTimer::firstFrameTime_ = -1;

FrameResolutionTimer::FrameResolutionTimer()
{
	start();
}

void FrameResolutionTimer::start()
{
	startTime_ = lastFrameTime_;
}

int FrameResolutionTimer::elapsedTime(TimerUnits::TimerUnits units)
{
	//Start time can never be before the first frame.  At that point the user has had no
	//experimental input, so that time is not part of the experiment.
	if(startTime_ < 0)
		startTime_ = firstFrameTime_;
	double elapsedSec = lastFrameTime_ -  startTime_;
	if(units == TimerUnits::sec)
		return elapsedSec;
	else if(units == TimerUnits::ms)
		return elapsedSec * 1000.0;
	else if(units == TimerUnits::us)
		return elapsedSec * 1000000.0;
	else
	{
		Q_ASSERT(false);
		return 0;
	}
}

void FrameResolutionTimer::setLastFrameTime(double frameTime)
{
	if(lastFrameTime_ < 0)
		firstFrameTime_ = frameTime;
	lastFrameTime_ = frameTime;
}

}	//namespace Picto
}	//namespace ControlElement
