#include "FrameResolutionTimer.h"


namespace Picto {
namespace Controller {

/*! \brief Constructs a timer, starting it running in the process
 *	\details The passed in frameTracker is this Timer's source for frame timing.
 *	This allows a single frameTracker to be updated by the Engine and provide frame
 *	timing information to multiple FrameResolutionTimer objects.  Construction of
 *	the FrameTracker instance and generation of the FrameResolutionTimers is handled 
 *	by the FrameTimerFactory.
 */
 
FrameResolutionTimer::FrameResolutionTimer(QSharedPointer<FrameTracker> frameTracker)
{
	frameTracker_ = frameTracker;
	start();
}

FrameResolutionTimer::~FrameResolutionTimer()
{
}

/*! \brief Resets the "zero time" of the timer to the latest first phosphor time.
 *	\details Future returned times will be with respect to this start time.
 */
void FrameResolutionTimer::start()
{
	startTime_ = frameTracker_->lastFrameTime_;
}

/*! \brief Returns the time that has passed from the first phophor of the frame before the latest call to start() to the first phophor of the latest frame.
 *	\details Returned time is according to the passed in units value.
*/
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
