#include "RewardState.h"
#include "PlaybackData.h"
using namespace Picto;

bool RewardState::setReward(double time,qulonglong dataId,int duration,int channel)
{
	setValue(QSharedPointer<IndexedData>(new PlaybackData<PlaybackRewardData>(PlaybackRewardData(duration,channel),time)));
	return true;
}

void RewardState::triggerValueChange(bool reverse,bool last)
{
	if(!reverse)
	{
		PlaybackRewardData* data = &getCurrentValue().staticCast<PlaybackData<PlaybackRewardData>>()->data_;
		emit rewardSupplied(getCurrentValue()->index_.time(),data->duration_,data->channel_);
	}
}

void RewardState::requestMoreData(PlaybackIndex currLast,PlaybackIndex to)
{
	emit needsData(currLast,to);
}
void RewardState::requestNextData(PlaybackIndex currLast,bool backward)
{
	emit needsNextData(currLast,backward);
}