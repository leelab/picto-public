#include "PictoBoxRewardController.h"

namespace Picto
{

PictoBoxRewardController::PictoBoxRewardController(unsigned int channelCount) 
: RewardController(channelCount)
{
	unsigned int rewardDurations[MAX_NUM_REWARD_CONTROLLERS];
	for(int i=0; i<MAX_NUM_REWARD_CONTROLLERS; i++)
		rewardDurations[i] = 100;
	daqBoard_.initRewardController(channelCount_,rewardDurations);
}

PictoBoxRewardController::~PictoBoxRewardController()
{
}

void PictoBoxRewardController::giveReward(unsigned int channel)
{
	daqBoard_.giveReward(channel);
}

bool PictoBoxRewardController::setRewardVolume(unsigned int channel, float volume)
{
	return false;
}

bool PictoBoxRewardController::setRewardDuration(unsigned int channel, unsigned int duration)
{
	if(!daqBoard_.setRewardDuration(channel,duration))
		return false;
	return true;
}

} //namespace Picto