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

void AudioRewardController::stopReward(unsigned int)
{
	latestQuantity_ = timer_.elapsed();
	//I should really stop the audio thread from running as well, but there was no simple
	//way to do this, the sound is very short, and we are only using this reward controller
	//for test purposes anyway so I'm just going to leave it alone.
}

bool AudioRewardController::rewardWasSupplied(unsigned int channel)
{
	if(!audioThreadFuture_.isFinished())
		return false;
	if(timer_.elapsed() <= latestQuantity_)
		return false;
	return true;
}
void AudioRewardController::flush(unsigned int channel,bool flush)
{
	Q_UNUSED(channel);
	Q_UNUSED(flush);
}

void AudioRewardController::playSound(int)
{
	tick_.play();
}

} //namespace Picto