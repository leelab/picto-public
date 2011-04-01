#include "simplelfpsource.h"
#include "..\..\common\storage\LFPDataStore.h"
SimpleLFPSource::SimpleLFPSource(double secPerEvent, double secPerSample)
:SimpleEventSource(secPerEvent,secPerSample)
{
	latestTime_ = 0;
}
QSharedPointer<Picto::DataStore> SimpleLFPSource::buildEvent(double time)
{
	QSharedPointer<Picto::LFPDataStore> newEvent = QSharedPointer<Picto::LFPDataStore>(new Picto::LFPDataStore());
	int potentials[6];
		
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