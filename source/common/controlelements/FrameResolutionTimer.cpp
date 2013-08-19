#include "FrameResolutionTimer.h"


namespace Picto {
namespace Controller {

FrameResolutionTimer::FrameResolutionTimer(QSharedPointer<FrameTracker> frameTracker)
{
	frameTracker_ = frameTracker;
	start();
}

FrameResolutionTimer::~FrameResolutionTimer()
{
}

void FrameResolutionTimer::start()
{
	startTime_ = frameTracker_->lastFrameTime_;
}

int FrameResolutionTimer::elapsedTime(Controller::TimerUnits::TimerUnits units)
{
	//Start time can never be before the first frame.  At that point the user has had no
	//experimental input, so that time is not part of the experiment.
	if(startTime_ < 0)
		startTime_ = frameTracker_->firstFrameTime_;
	double elapsedSec = frameTracker_->lastFrameTime_ -  startTime_;
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


}	//namespace Picto
}	//namespace ControlElement
