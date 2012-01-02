#include "RewardController.h"
#include "../memleakdetect.h"
#include "../timing/Timestamper.h"

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

void RewardController::giveReward(unsigned int channel,int quantity, int minRewardPeriod, bool appendToList)
{
	QMutexLocker locker(&giveRewardMutex_);
	Timestamper stamper;
	double rewardTime = stamper.stampSec();
	doReward(channel,quantity,minRewardPeriod);
	if(!appendToList)
		return;
	appendDeliveredRewards(QSharedPointer<RewardDataUnit>(new RewardDataUnit(quantity,channel,rewardTime)));

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