#include "simplelfpsource.h"
#include "..\..\common\storage\LFPDataUnitPackage.h"
SimpleLFPSource::SimpleLFPSource(double secPerEvent, double secPerSample)
:SimpleEventSource(secPerEvent,secPerSample)
{
	latestTime_ = 0;
}
QSharedPointer<Picto::DataUnit> SimpleLFPSource::buildEvent(double time)
{
	QSharedPointer<Picto::LFPDataUnitPackage> newEvent = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());
	double potentials[6];
		
	double sampTime = latestTime_;
	for(sampTime += secPerSample_;sampTime<time;sampTime+=secPerSample_)
	{
		if(newEvent->numSamples() >= 10000)
			break;
		newEvent->addData(sampTime,potentials,6);
	}
	latestTime_ = sampTime-secPerSample_;
	
	return newEvent;
}