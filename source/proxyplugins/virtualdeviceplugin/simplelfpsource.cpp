#include "simplelfpsource.h"
#include "..\..\common\storage\LFPDataUnitPackage.h"
#include "../../common/memleakdetect.h"
#include <math.h>
#define PI 3.14159265
SimpleLFPSource::SimpleLFPSource(double secPerEvent, double secPerSample,int channel)
:SimpleEventSource(secPerEvent,secPerSample,0)
{
	channel_ = channel;
	sampsPerUnit_ = secPerEvent/secPerSample;
	vals_ = new float[sampsPerUnit_];
}

SimpleLFPSource::~SimpleLFPSource()
{
	delete[] vals_;
}

QSharedPointer<Picto::DataUnit> SimpleLFPSource::buildEvent(double time)
{
	QSharedPointer<Picto::LFPDataUnitPackage> newEvent;
	
	for(int i=0;i<sampsPerUnit_;i++)
	{
		vals_[i] = channel_+sin((time+(i*secPerSample_))*2*PI*channel_);
	}
	newEvent = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());
	newEvent->setChannel(channel_);
	newEvent->setResolution(secPerSample_);
	newEvent->setTimestamp(time);
	newEvent->appendData(vals_,sampsPerUnit_);
	
	return newEvent;
}