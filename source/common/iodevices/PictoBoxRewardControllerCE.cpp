#include "PictoBoxRewardController.h"

namespace Picto
{

PictoBoxRewardControllerCE::PictoBoxRewardControllerCE(unsigned int channelCount) 
: RewardController(channelCount)
{
	unsigned int rewardDurations[MAX_NUM_REWARD_CONTROLLERS];
	for(int i=0; i<MAX_NUM_REWARD_CONTROLLERS; i++)
		rewardDurations[i] = 100;
	daqBoard_.initRewardController(channelCount_,rewardDurations);
}

PictoBoxRewardControllerCE::~PictoBoxRewardControllerCE()
{
}

void PictoBoxRewardControllerCE::giveReward(unsigned int channel)
{
	daqBoard_.giveReward(channel);
}

bool PictoBoxRewardControllerCE::setRewardVolume(unsigned int channel, float volume)
{
	return false;
}

bool PictoBoxRewardControllerCE::setRewardDurationMs(unsigned int channel, unsigned int duration)
{
	if(!daqBoard_.setRewardDuration(channel,duration))
		return false;
	return true;
}

} //namespace Picto