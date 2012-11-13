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
	setValue(QSharedPointer<IndexedData>(new PlaybackData<PlaybackSpikeData>(PlaybackSpikeData(channel,unit,waveVec),spikeTime)));
	return true;
}

void SpikeState::triggerValueChange(bool reverse,bool last)
{
	if(!reverse)
	{
		PlaybackSpikeData* data = &getCurrentValue().staticCast<PlaybackData<PlaybackSpikeData>>()->data_;
		emit spikeEvent(data->channel_,data->unit_,data->waveform_);
	}
}

void SpikeState::requestMoreData(PlaybackIndex currLast,PlaybackIndex to)
{
	emit needsData(currLast,to);
}
void SpikeState::requestNextData(PlaybackIndex currLast,bool backward)
{
	emit needsNextData(currLast,backward);
}