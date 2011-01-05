#include "simplemarksource.h"

SimpleMarkSource::SimpleMarkSource(double secPerEvent, double secPerSample)
:SimpleEventSource(secPerEvent,secPerSample)
{
	lastWasTrialEnd = true;
	currEventCode = 0;
}
QSharedPointer<VirtualEvent> SimpleMarkSource::buildEvent(double time)
{
	QSharedPointer<VirtualEvent> newEvent = QSharedPointer<VirtualEvent>(new VirtualEvent());
	if(lastWasTrialEnd)
	{
		currEventCode++;
	}
	lastWasTrialEnd = !lastWasTrialEnd;
	newEvent->eventCode_ = currEventCode;
	newEvent->timeStamp_ = time;
	newEvent->type_ = VirtualEvent::eMARK;
	return newEvent;
}