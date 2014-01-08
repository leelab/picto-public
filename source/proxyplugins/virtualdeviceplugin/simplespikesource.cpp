#include "simplespikesource.h"
#include "..\..\common\storage\NeuralDataUnit.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a SimpleSpikeSource object that will generate NeuralDataUnit objects once 
 *	for every input number of seconds with a randomization range of the full secPerEvent timespan.
 *	\details secPerSample is just used to define the samplingRate().  The spike will be marked as
 *	appearing on the input channel and unit.
 */
SimpleSpikeSource::SimpleSpikeSource(double secPerEvent, double secPerSample, int channel, int unit)
:
SimpleEventSource(secPerEvent,secPerSample,secPerEvent),
channel_(channel),
unit_(unit)
{}

/*! \brief Creates and returns a single NeuralDataUnit at the input time with channel and unit according to the values 
 *	passed into the constructor.  
 *	\details The waveform used here is always 40 samples long and has resolution of 1us.  We should probably
 *	set this resolution to the secPerSample reading from the constructor instead.  
 */
QSharedPointer<Picto::DataUnit> SimpleSpikeSource::buildEvent(double time)
{
	QSharedPointer<Picto::NeuralDataUnit> newEvent = QSharedPointer<Picto::NeuralDataUnit>(new Picto::NeuralDataUnit());
	newEvent->setChannel(channel_);
	newEvent->setTimestamp(time);
	newEvent->setUnit(unit_);
	newEvent->setResolution(0.000001);
	QSharedPointer<QVector<float>> waveform(new QVector<float>());
	for(int i=0;i<40;i++)
		waveform->push_back(i);
	newEvent->setWaveform(waveform);
	return newEvent;
}