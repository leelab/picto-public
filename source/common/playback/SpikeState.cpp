#include "SpikeState.h"
#include "PlaybackData.h"
using namespace Picto;

bool SpikeState::setSpike(qulonglong dataid,double spikeTime,int channel,int unit,QByteArray waveform)
{
	int numEntries = waveform.size()/sizeof(float);
	float* floatArray = reinterpret_cast<float*>(waveform.data());
	QVector<float> waveVec(numEntries);
	for(int i=0;i<numEntries;i++)
	{
		waveVec[i] = floatArray[i];
	}
	setValue(QSharedPointer<IndexedData<double>>(new PlaybackData<double,PlaybackSpikeData>(spikeTime,spikeTime,PlaybackSpikeData(channel,unit,waveVec))));
	return true;
}

void SpikeState::triggerValueChange(bool reverse,bool last)
{
	if(!reverse)
	{
		PlaybackSpikeData* data = &getCurrentValue().staticCast<PlaybackData<double,PlaybackSpikeData>>()->data_;
		emit spikeEvent(data->channel_,data->unit_,data->waveform_);
	}
}

void SpikeState::requestMoreData(double index)
{
}

void SpikeState::requestMoreDataByTime(double time)
{
}
