#include "RewardController.h"
#include "../memleakdetect.h"

namespace Picto
{
RewardController::RewardController(unsigned int channelCount)
	:channelCount_(channelCount)
{
}

RewardController::~RewardController()
{
}

bool RewardController::setRewardVolume(unsigned int channel, float volume)
{
	if(channel > 4 || channel < 1)
		return false;
	rewardVolumes_[channel-1] = volume;
	return true;
}

void RewardController::addReward(unsigned int channel,int quantity, int minRewardPeriod)
{
	rewardChannels_[channel].pendingRewards.append(RewardUnit(quantity,minRewardPeriod));
}

void RewardController::triggerRewards(bool appendToList)
{
	QHash<int,RewardChannel>::iterator it;
	for(it = rewardChannels_.begin();it!=rewardChannels_.end();it++)
	{
		//Check if this channel has any new rewards
		if(!it.value().pendingRewards.size())
			continue;
		//Check if the last reward is still being supplied
		if(!rewardWasSupplied(it.key()))
			continue;
		//Check if the inter reward interval has been met
		if( it.value().stopwatch.elapsedMs() < it.value().lastRewardPeriod)
			continue;

		//Get the new reward for this channel
		RewardUnit reward = it.value().pendingRewards.takeFirst();
		//Start reward
		startReward(it.key(),reward.quantity);
		double rewardTime = stamper_.stampSec();
		it.value().stopwatch.startWatch();
		it.value().lastRewardPeriod = reward.minRewardPeriod;
		if(!appendToList)
			continue;
		appendDeliveredRewards(QSharedPointer<RewardDataUnit>(new RewardDataUnit(reward.quantity,it.key(),rewardTime)));
	}
}

bool RewardController::hasPendingRewards()
{
	bool returnVal = false;
	foreach(RewardChannel rewardChan,rewardChannels_)
	{
		if(rewardChan.pendingRewards.size())
		{
			returnVal = true;
			break;
		}
	}
	return returnVal;
}

void RewardController::appendDeliveredRewards(QSharedPointer<RewardDataUnit> rewardUnit)
{
	deliveredRewards_.append(rewardUnit);
}

QList<QSharedPointer<RewardDataUnit>> RewardController::getDeliveredRewards()
{
	QList<QSharedPointer<RewardDataUnit>> returnVal;
	returnVal = deliveredRewards_; 
	deliveredRewards_.clear();
	return returnVal;
}

} //namespace Picto