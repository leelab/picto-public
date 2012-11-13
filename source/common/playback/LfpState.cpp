#include "LfpState.h"
#include "PlaybackData.h"
using namespace Picto;

LfpState::LfpState(int channel,double sampPeriod) :
channel_(channel),
sampPeriod_(sampPeriod)
{

}

bool LfpState::setLFP(qulonglong dataid,double startTime,int channel,QByteArray dataArray)
{
	int numEntries = dataArray.size()/sizeof(float);
	float* floatArray = reinterpret_cast<float*>(dataArray.data());
	for(int i=0;i<numEntries;i++)
	{
		setValue(QSharedPointer<IndexedData>(new PlaybackData<float>(floatArray[i],startTime+i*sampPeriod_)));
	}
	return true;
}

void LfpState::triggerValueChange(bool reverse,bool last)
{
	if(!reverse || last)
	{
		QSharedPointer<PlaybackData<float>> currVal = getCurrentValue().staticCast<PlaybackData<float>>();
		if(currVal)
			emit lfpChanged(channel_,currVal->data_);
		else
			emit lfpChanged(channel_,0);
	}
}

void LfpState::requestMoreData(PlaybackIndex currLast,PlaybackIndex to)
{
	emit needsData(currLast,to);
}
void LfpState::requestNextData(PlaybackIndex currLast,bool backward)
{
	emit needsNextData(currLast,backward);
}