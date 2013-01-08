#include "RewardController.h"
#include "../memleakdetect.h"

namespace Picto
{
RewardController::RewardController(unsigned int channelCount)
	:channelCount_(channelCount),
	discardOverlap_(false)
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

void RewardController::discardOverlapingRewards(bool doIt)
{
	discardOverlap_ = doIt;
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
		//Check if the inter reward/flush interval has been met
		if( it.value().stopwatch.elapsedMs() < it.value().lastRewardPeriod)
		{
			if(discardOverlap_)
				it.value().pendingRewards.takeFirst();
			continue;
		}
		//If it was met once, set it to zero to avoid stopwatch wrapping issues.
		it.value().lastRewardPeriod = 0;

		//If there is a flush in progress stop it
		if(it.value().inFlush)
		{
			stopFlush(it.key());
			it.value().inFlush = false;
		}

		//Check if this channel has any new rewards
		if(!it.value().pendingRewards.size())
			continue;	
		
		//Check if the last reward is still being supplied
		if(!rewardWasSupplied(it.key()))
		{
			if(discardOverlap_)
				it.value().pendingRewards.takeFirst();
			continue;
		}

		//Get the new reward for this channel
		RewardUnit reward = it.value().pendingRewards.takeFirst();
		if(!reward.isFlush)
		{	//Start reward
			startReward(it.key(),reward.quantity);
		}
		else
		{	//Start flush			
			startFlush(it.key());
			it.value().inFlush = true;
		}
		double rewardTime = stamper_.stampSec();
		it.value().stopwatch.startWatch();
		it.value().lastRewardPeriod = reward.minRewardPeriod;
		if(!appendToList)
			continue;
		if(!reward.isFlush)
			appendDeliveredRewards(QSharedPointer<RewardDataUnit>(new RewardDataUnit(reward.quantity,it.key(),rewardTime)));
	}
}

bool RewardController::rewardInProgress(unsigned int channel)
{
	if(!rewardChannels_.contains(channel))
		return false;
	if(!rewardWasSupplied(channel))
	{
		return true;
	}
	return false;
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

bool RewardController::hasPendingRewards(unsigned int channel)
{
	if(!rewardChannels_.contains(channel))
		return false;
	if(rewardChannels_[channel].pendingRewards.size())
		return true;
	return false;
}

void RewardController::flush(unsigned int channel, int seconds)
{
	//Don't start a flush unless there are no flushes or rewards in progress or pending
	if(isFlushing(channel) || rewardInProgress(channel) || hasPendingRewards(channel))
		return;
	rewardChannels_[channel].pendingRewards.append(RewardUnit(seconds*1000,seconds*1000,true));
}

bool RewardController::isFlushing(unsigned int channel)
{
	if(!rewardChannels_.contains(channel))
		return false;
	if(rewardChannels_[channel].inFlush)
		return true;
	return false;
}

void RewardController::abortFlush(unsigned int channel)
{
	if(!rewardChannels_.contains(channel))
		return;
	if(!rewardChannels_[channel].inFlush)
		return;
	stopFlush(channel);
	rewardChannels_[channel].inFlush = false;
	rewardChannels_[channel].lastRewardPeriod = 0;
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