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

void AudioRewardController::giveReward(unsigned int channel,int quantity)
{
		QTime timer;
		timer.start();
		tick_.play();
		while(timer.elapsed() < (quantity+rewardResetTimes_[channel-1]))
			QCoreApplication::processEvents();
}

void AudioRewardController::flush(unsigned int channel,bool flush)
{
	Q_UNUSED(channel);
	Q_UNUSED(flush);
}

} //namespace Picto