#include "simplespikesource.h"

SimpleSpikeSource::SimpleSpikeSource(double secPerEvent, double secPerSample)
:SimpleEventSource(secPerEvent,secPerSample)
{}
QSharedPointer<VirtualEvent> SimpleSpikeSource::buildEvent(double time)
{
	QSharedPointer<VirtualEvent> newEvent = QSharedPointer<VirtualEvent>(new VirtualEvent());
	newEvent->channel_ = 0;
	newEvent->timeStamp_ = time;
	newEvent->type_ = VirtualEvent::eSPIKE;
	newEvent->unit_ = 0;
	newEvent->waveform_.push_back(0);
	return newEvent;
}