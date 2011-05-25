#include "simpleeventsource.h"

SimpleEventSource::SimpleEventSource(double secPerEvent, double secPerSample)
{
	secPerEvent_ = secPerEvent;
	secPerSample_ = secPerSample;
}
QSharedPointer<Picto::DataUnit> SimpleEventSource::getNextEvent(double time)
{
	if((secPerEvent_ <= 0) || (secPerSample_ <= 0))
	{
		return QSharedPointer<Picto::DataUnit>();
	}
	if((startTime_ + secPerEvent_) < time)
	{
		startTime_ += secPerEvent_;
		return buildEvent(startTime_);
	}
	return QSharedPointer<Picto::DataUnit>();
}
float SimpleEventSource::samplingRate()
{
	if(secPerSample_ == 0)
		return 0;
	return 1.0f/secPerSample_;
}