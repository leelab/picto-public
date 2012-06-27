#include <QApplication>
#include <QTime>

#include "AudioRewardController.h"
#include "../memleakdetect.h"

namespace Picto
{

AudioRewardController::AudioRewardController() : 
RewardController(1) ,
tick_("sounds/tick.wav"),
latestQuantity_(0)
{}

void AudioRewardController::startReward(unsigned int,int quantity)
{
	latestQuantity_ = quantity;
	timer_.start();
	audioThreadFuture_ = QtConcurrent::run(this,&AudioRewardController::playSound,quantity);
}

bool AudioRewardController::rewardWasSupplied(unsigned int channel)
{
	if(!audioThreadFuture_.isFinished())
		return false;
	if(timer_.elapsed() <= latestQuantity_)
		return false;
	return true;
}
void AudioRewardController::startFlush(unsigned int channel)
{
	startReward(0,50);	//Since the sound is so short and flushing is really meaningless, just play it quickly.
}
void AudioRewardController::stopFlush(unsigned int channel)
{
	latestQuantity_ = timer_.elapsed();
}

void AudioRewardController::playSound(int)
{
	tick_.play();
}

} //namespace Picto