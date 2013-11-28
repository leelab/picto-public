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

/*! \brief This doesn't appear to be used or have any effect and should be removed.
 */
bool RewardController::setRewardVolume(unsigned int channel, float volume)
{
	if(channel > 4 || channel < 1)
		return false;
	rewardVolumes_[channel-1] = volume;
	return true;
}

/*! \brief Set this to true if rewards that are queued up on a given channel, waiting for
 *	the one in progress to finish should just be erased.
 */
void RewardController::discardOverlapingRewards(bool doIt)
{
	discardOverlap_ = doIt;
}
/*! \brief Appends a reward to the list of startable rewards for the input channel
 *	and with the input quantity and minRewardPeriod (both in milliseconds).
 *	\details minRewardPeriod is the minimum time between rewards.
 */
void RewardController::addReward(unsigned int channel,int quantity, int minRewardPeriod)
{
	rewardChannels_[channel].pendingRewards.append(RewardUnit(quantity,minRewardPeriod));
}

/*! \brief Triggers the next reward from addReward() if the previous one on its channel is done.
 *	\details If discardOverlapingRewards() was set true and a previous reward on a channel
 *	is in progress, all pending rewards on that channel will be removed.
 *	If appendToList it true, every reward supplied will be added to a list
 *	that is retrievable using getDeliveredRewards() this allows a record of
 *	delivered rewards to be sent to the PictoServer from the Director.
 */
void RewardController::triggerRewards(bool appendToList)
{
	QHash<int,RewardChannel>::iterator it;
	for(it = rewardChannels_.begin();it!=rewardChannels_.end();it++)
	{
		//Check if the inter reward/flush interval has been met
		if( it.value().stopwatch.elapsedMs() < it.value().lastRewardPeriod)
		{
			if(discardOverlap_)
				it.value().pendingRewards.clear();
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
/*! \brief Returns true if there is a reward in progress on the input channel.
*/
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

/*! \brief returns true if there are rewards waiting to be supplied on any channel.*/
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
/*! \brief returns true if there are rewards waiting to be supplied on the input channel.*/
bool RewardController::hasPendingRewards(unsigned int channel)
{
	if(!rewardChannels_.contains(channel))
		return false;
	if(rewardChannels_[channel].pendingRewards.size())
		return true;
	return false;
}

/*! \brief Flushes on the input channel for the input number of seconds.*/
void RewardController::flush(unsigned int channel, int seconds)
{
	//Don't start a flush unless there are no flushes or rewards in progress or pending
	if(isFlushing(channel) || rewardInProgress(channel) || hasPendingRewards(channel))
		return;
	rewardChannels_[channel].pendingRewards.append(RewardUnit(seconds*1000,seconds*1000,true));
}
/*! \brief Returns true if there is a flush in progress on the input channel*/
bool RewardController::isFlushing(unsigned int channel)
{
	if(!rewardChannels_.contains(channel))
		return false;
	if(rewardChannels_[channel].inFlush)
		return true;
	return false;
}

/*! \brief Stops any active flush on the input channel*/
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

/*! \brief Used by triggerRewards() to append a reward that has been supplied to a list
 * that is accessable from getDeliveredRewards()
 */
void RewardController::appendDeliveredRewards(QSharedPointer<RewardDataUnit> rewardUnit)
{
	deliveredRewards_.append(rewardUnit);
}

/*! \brief Returns a list of the latest rewards to have been supplied, clearing
 *	it in the process.
 */
QList<QSharedPointer<RewardDataUnit>> RewardController::getDeliveredRewards()
{
	QList<QSharedPointer<RewardDataUnit>> returnVal;
	returnVal = deliveredRewards_; 
	deliveredRewards_.clear();
	return returnVal;
}

} //namespace Picto