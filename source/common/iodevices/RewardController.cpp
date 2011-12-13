#include "RewardController.h"
#include "../memleakdetect.h"

namespace Picto
{
RewardController::RewardController(unsigned int channelCount)
	:channelCount_(channelCount)
{
	//initialize the reset time to 150 ms (this is arbitrary
	for(int i=0; i<4; i++)
		rewardResetTimes_.append(150);

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

bool RewardController::setRewardResetTimeMs(unsigned int channel, unsigned int time)
{
	if(channel > 4 || channel < 1)
		return false;
	rewardResetTimes_[channel-1] = time;
	return true;
}

} //namespace Picto