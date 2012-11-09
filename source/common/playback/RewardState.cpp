#include "RewardState.h"
#include "PlaybackData.h"
using namespace Picto;

bool RewardState::setReward(double time,qulonglong dataId,int duration,int channel)
{
	setValue(QSharedPointer<IndexedData<double>>(new PlaybackData<double,PlaybackRewardData>(time,time,PlaybackRewardData(duration,channel))));
	return true;
}

void RewardState::triggerValueChange(bool reverse,bool last)
{
	if(!reverse)
	{
		PlaybackRewardData* data = &getCurrentValue().staticCast<PlaybackData<double,PlaybackRewardData>>()->data_;
		emit rewardSupplied(data->duration_,data->channel_);
	}
}

void RewardState::requestMoreData(double index)
{
}

void RewardState::requestMoreDataByTime(double time)
{
}
