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

void AudioRewardController::giveReward(unsigned int channel,int)
{
	//if(channel == 1)
	//{
		
		tick_.play();
		////If we call beep too quickly, they all run together, 
		////so we have to pause after each call
		//QTime timer;
		//timer.start();
		//while(timer.elapsed() < rewardResetTimes_[channel-1] + rewardDurations_[channel-1])
		//	QCoreApplication::processEvents();
	//}

}

void AudioRewardController::flush(unsigned int channel,bool flush)
{
	Q_UNUSED(channel);
	Q_UNUSED(flush);
}

} //namespace Picto