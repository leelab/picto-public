#include "simplespikesource.h"
#include "..\..\common\storage\NeuralDataUnit.h"
#include "../../common/memleakdetect.h"

SimpleSpikeSource::SimpleSpikeSource(double secPerEvent, double secPerSample, int channel, int unit)
:
SimpleEventSource(secPerEvent,secPerSample,secPerEvent),
channel_(channel),
unit_(unit)
{}
QSharedPointer<Picto::DataUnit> SimpleSpikeSource::buildEvent(double time)
{
	QSharedPointer<Picto::NeuralDataUnit> newEvent = QSharedPointer<Picto::NeuralDataUnit>(new Picto::NeuralDataUnit());
	newEvent->setChannel(channel_);
	newEvent->setTimestamp(time);
	newEvent->setUnit(unit_);
	QSharedPointer<QVector<float>> waveform(new QVector<float>());
	for(int i=0;i<40;i++)
		waveform->push_back(i);
	newEvent->setWaveform(waveform);
	return newEvent;
}