#include "simplelfpsource.h"
#include "..\..\common\storage\LFPDataUnitPackage.h"
#include "../../common/memleakdetect.h"
SimpleLFPSource::SimpleLFPSource(double secPerEvent, double secPerSample,int channel)
:SimpleEventSource(secPerEvent,secPerSample)
{
	nextTime_ = 0;
	channel_ = channel;
	sampsPerUnit_ = secPerEvent/secPerSample;
	vals_ = new double[sampsPerUnit_];
	for(int i=0;i<sampsPerUnit_;i++)
	{
		vals_[i] = i;
	}
}

SimpleLFPSource::~SimpleLFPSource()
{
	delete[] vals_;
}

QSharedPointer<Picto::DataUnit> SimpleLFPSource::buildEvent(double time)
{
	QSharedPointer<Picto::LFPDataUnitPackage> newEvent;
	if(nextTime_+secPerEvent_ > time)
		return newEvent;
	newEvent = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());
	newEvent->setChannel(channel_);
	newEvent->setResolution(secPerSample_);
	newEvent->setTimestamp(nextTime_);
	newEvent->addData(vals_,sampsPerUnit_,secPerSample_);
	nextTime_+=secPerEvent_;
	
	return newEvent;
}