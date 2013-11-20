#include "FrameTimerFactory.h"


namespace Picto {
namespace Controller {

/*! \brief Constructs a new FrameTimerFactory
 *	\details A FrameTracker is created in the constructor.  This FrameTracker will be used with all
 *	FrameResolutionTimer objects created by this factory.
 */
FrameTimerFactory::FrameTimerFactory()
{
	frameTracker_ = QSharedPointer<FrameTracker>(new FrameTracker());
}

FrameTimerFactory::~FrameTimerFactory()
{
}

QSharedPointer<FrameResolutionTimer> FrameTimerFactory::createTimer()
{
	QSharedPointer<FrameResolutionTimer> returnVal(new FrameResolutionTimer(frameTracker_));
	timerList_.append(returnVal);
	return returnVal;
}
/*! \brief Called by the rendering engine to mark the time at which the latest frame occured.
 *	\details This sets the last frame time to the FrameTracker that is used by all timers 
 *	created by this factory in calculating elapsedTime
 */
void FrameTimerFactory::setLastFrameTime(double frameTime)
{
	frameTracker_->setLastFrameTime(frameTime);
}

/*! \brief Called to set the time of the upcoming frame, when this is known in advance (ie. When replaying an experiment)
 *	\details Currently we are not using the effectiveAbsoluteTime in Picto; however, it could come in handy at some
 *	point so we have included it here.  The 'next frame' is used in the calculation of effectiveAbsoluteFrame
 */

void FrameTimerFactory::setNextFrameTime(double frameTime)
{
	frameTracker_->setNextFrameTime(frameTime);
}

/*! \brief effectiveAbsoluteTime is the time at which changes in the current experiment are expected to take place.
 *	\details If we change the color of a dot in the experiment, the subject doesn't experience that change until
 *	the next frame; therefore, at least conceptually, it would be useful for our frameResolutionTimer objects to
 *	know when the next frame will be, though this is only possible in playback when we know what will happen in 
 *	the future.  The effectiveAbsoluteTime() is this "next frame time." It should be accurate on playback, and is
 *	estimated by adding 16.666ms to the last frame during a live experiment.  In practice, during a live experiment,
 *	it shouldn't be important to know when the next frame will be, and in a played back experiment, our AnalysisFrameData
 *	object provides timing of future frames, so the data is available without using this function.  We put it in because 
 *	it seemed to make sense while we were in the design process.  It isn't being used in practice, and if you do decide to
 *	start using it in practice, make sure to think twice, especially considering that it doesn't return a precise value
 *	during live experiments.
 *	
 *	The value returned is according to the units that are passed in.
 *	\sa FrameTracker::effectiveAbsoluteTime
 */
int FrameTimerFactory::effectiveAbsoluteTime(TimerUnits::TimerUnits units)
{
	return frameTracker_->effectiveAbsoluteTime(units);
}

/*! \brief Resets all FrameResolutionTimers that were created by this FrameTimerFactory
 *	\details This factory keeps track of the pointers to all of the FrameResolutionTimers that it creates,
 *	which allows it to reset all of them when this function is called.  This is useful in the ReplayViewer
 *	where we may playback the same experimental run more than once without recreating the Experiment from
 *	xml.
 */
void FrameTimerFactory::resetAllTimers()
{
	frameTracker_->reset();
	for(QVector<QWeakPointer<FrameResolutionTimer>>::Iterator it = timerList_.begin();it != timerList_.end();)
	{	
		if(!it->toStrongRef())
		{
			it = timerList_.erase(it);
			continue;
		}
		it->toStrongRef()->start();
		it++;
	}
}


}	//namespace Picto
}	//namespace ControlElement
