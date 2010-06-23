#include "RewardController.h"

namespace Picto
{
RewardController::RewardController(unsigned int channelCount)
	:channelCount_(channelCount)
{
	//initialize the rewrad durations to 250 ms (this is arbitrary)
	for(int i=0; i<4; i++)
		rewardDurations_.append(250);
	//initialize the reset time to 100 ms (this is also arbitrary
	for(int i=0; i<4; i++)
		rewardResetTimes_.append(250);

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

bool RewardController::setRewardDurationMs(unsigned int channel, unsigned int duration)
{
	if(channel > 4 || channel < 1)
		return false;
	rewardDurations_[channel-1] = duration;
	return true;
}

int RewardController::getRewardDurationMs(unsigned int channel)
{
	if(channel > 4 || channel < 1)
		return -1;
	else
		return rewardDurations_[channel-1];

}

bool RewardController::setRewardResetTimeMs(unsigned int channel, unsigned int time)
{
	if(channel > 4 || channel < 1)
		return false;
	rewardResetTimes_[channel-1] = time;
	return true;
}

} //namespace Picto