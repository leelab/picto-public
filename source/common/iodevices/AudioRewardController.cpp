#include <QApplication>
#include <QTime>
#include <QTimer>

#include "AudioRewardController.h"
#include "../memleakdetect.h"

namespace Picto
{

AudioRewardController::AudioRewardController() : 
RewardController(1) ,
tick_(":/sounds/tick.wav"),
latestQuantity_(0),
firstRewardSupplied_(false)
{
}

void AudioRewardController::startReward(unsigned int,int quantity)
{
	latestQuantity_ = quantity;
	timer_.start();
	tick_.play();
	firstRewardSupplied_ = true;
}

bool AudioRewardController::rewardWasSupplied(unsigned int)
{
	if((timer_.elapsed() <= latestQuantity_) && firstRewardSupplied_)
		return false;
	return true;
}
void AudioRewardController::startFlush(unsigned int)
{
	startReward(0,50);	//Since the sound is so short and flushing is really meaningless, just play it quickly.
}
void AudioRewardController::stopFlush(unsigned int channel)
{
	latestQuantity_ = timer_.elapsed();
}

} //namespace Picto