#include "simplemarksource.h"

SimpleMarkSource::SimpleMarkSource(double secPerEvent, double secPerSample)
:SimpleEventSource(secPerEvent,secPerSample)
{
	currEventCode = 0;
}
QSharedPointer<VirtualEvent> SimpleMarkSource::buildEvent(double time)
{
	QSharedPointer<VirtualEvent> newEvent = QSharedPointer<VirtualEvent>(new VirtualEvent());
	currEventCode++;
	if(currEventCode == 128)
		currEventCode = 1;
	newEvent->eventCode_ = currEventCode;
	newEvent->timeStamp_ = time;
	newEvent->type_ = VirtualEvent::eMARK;
	return newEvent;
}