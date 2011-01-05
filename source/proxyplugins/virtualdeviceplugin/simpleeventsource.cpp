#include "simpleeventsource.h"

SimpleEventSource::SimpleEventSource(double secPerEvent, double secPerSample)
{
	secPerEvent_ = secPerEvent;
	secPerSample_ = secPerSample;
}
QSharedPointer<VirtualEvent> SimpleEventSource::getNextEvent(double time)
{
	if((secPerEvent_ <= 0) || (secPerSample_ <= 0))
	{
		return QSharedPointer<VirtualEvent>();
	}
	if((startTime_ + secPerEvent_) < time)
	{
		startTime_ += secPerEvent_;
		return buildEvent(startTime_);
	}
	return QSharedPointer<VirtualEvent>();
}
float SimpleEventSource::samplingRate()
{
	if(secPerSample_ == 0)
		return 0;
	return 1.0f/secPerSample_;
}