#include "simplespikesource.h"
#include "..\..\common\storage\NeuralDataUnit.h"
#include "../../common/memleakdetect.h"

SimpleSpikeSource::SimpleSpikeSource(double secPerEvent, double secPerSample, int channel, int unit)
:
SimpleEventSource(secPerEvent,secPerSample),
channel_(channel),
unit_(unit)
{}
QSharedPointer<Picto::DataUnit> SimpleSpikeSource::buildEvent(double time)
{
	QSharedPointer<Picto::NeuralDataUnit> newEvent = QSharedPointer<Picto::NeuralDataUnit>(new Picto::NeuralDataUnit());
	newEvent->setChannel(channel_);
	newEvent->setTimestamp(time);
	newEvent->setUnit(unit_);
	QSharedPointer<QList<int>> waveform(new QList<int>());
	for(int i=0;i<10;i++)
		waveform->push_back(i);
	newEvent->setWaveform(waveform);
	return newEvent;
}