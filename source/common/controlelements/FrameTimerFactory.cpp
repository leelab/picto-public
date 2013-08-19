#include "FrameTimerFactory.h"


namespace Picto {
namespace Controller {

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

void FrameTimerFactory::setLastFrameTime(double frameTime)
{
	frameTracker_->setLastFrameTime(frameTime);
}
void FrameTimerFactory::setNextFrameTime(double frameTime)
{
	frameTracker_->setNextFrameTime(frameTime);
}

int FrameTimerFactory::effectiveAbsoluteTime(TimerUnits::TimerUnits units)
{
	return frameTracker_->effectiveAbsoluteTime(units);
}

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
