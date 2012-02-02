#include "simpleeventsource.h"
#include "../../common/memleakdetect.h"

SimpleEventSource::SimpleEventSource(double secPerEvent, double secPerSample, double randRange)
{
	secPerEvent_ = secPerEvent;
	secPerSample_ = secPerSample;
	randRange_ = randRange;
	nextRandOffset_ = 2*randRange_*(-.5+double(rand())/double(RAND_MAX));
}
QSharedPointer<Picto::DataUnit> SimpleEventSource::getNextEvent(double time)
{
	if((secPerEvent_ <= 0) || (secPerSample_ <= 0))
	{
		return QSharedPointer<Picto::DataUnit>();
	}

	if((startTime_ + secPerEvent_ + nextRandOffset_) < time)
	{
		startTime_ += secPerEvent_ + nextRandOffset_;
		nextRandOffset_ = 2*randRange_*(-.5+double(rand())/double(RAND_MAX));
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