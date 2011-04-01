#include "simplespikesource.h"
#include "..\..\common\storage\NeuralDataStore.h"

SimpleSpikeSource::SimpleSpikeSource(double secPerEvent, double secPerSample)
:SimpleEventSource(secPerEvent,secPerSample)
{}
QSharedPointer<Picto::DataStore> SimpleSpikeSource::buildEvent(double time)
{
	QSharedPointer<Picto::NeuralDataStore> newEvent = QSharedPointer<Picto::NeuralDataStore>(new Picto::NeuralDataStore());
	newEvent->setChannel(0);
	newEvent->setTimestamp(time);
	newEvent->setUnit(0);
	QSharedPointer<QList<int>> waveform(new QList<int>());
	for(int i=0;i<10;i++)
		waveform->push_back(i);
	newEvent->setWaveform(waveform);
	return newEvent;
}