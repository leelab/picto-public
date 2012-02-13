#include <QApplication>
#include <QTime>

#include "AudioRewardController.h"
#include "../memleakdetect.h"

namespace Picto
{

AudioRewardController::AudioRewardController() : 
RewardController(1) ,
tick_("sounds/tick.wav")
{}

void AudioRewardController::startReward(unsigned int,int quantity)
{
	audioThreadFuture_ = QtConcurrent::run(this,&AudioRewardController::playSound,quantity);
}

bool AudioRewardController::rewardWasSupplied(unsigned int channel)
{
	return audioThreadFuture_.isFinished();
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