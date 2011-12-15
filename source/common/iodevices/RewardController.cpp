#include "RewardController.h"
#include "../memleakdetect.h"
#include "../timing/Timestamper.h"

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

void RewardController::giveReward(unsigned int channel,int quantity,bool appendToList)
{
	QMutexLocker locker(&giveRewardMutex_);
	doReward(channel,quantity);
	if(!appendToList)
		return;
	Timestamper stamper;
	appendDeliveredRewards(QSharedPointer<RewardDataUnit>(new RewardDataUnit(quantity,channel,stamper.stampSec())));

}

void RewardController::appendDeliveredRewards(QSharedPointer<RewardDataUnit> rewardUnit)
{
	listLock_.lockForWrite();
	deliveredRewards_.append(rewardUnit);
	listLock_.unlock();
}

QList<QSharedPointer<RewardDataUnit>> RewardController::getDeliveredRewards()
{
	QList<QSharedPointer<RewardDataUnit>> returnVal;
	if(listLock_.tryLockForRead())
	{
		returnVal = deliveredRewards_; 
		deliveredRewards_.clear();
		listLock_.unlock();
	}
	return returnVal;
}

} //namespace Picto