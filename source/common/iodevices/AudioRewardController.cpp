#include "AudioRewardController.h"

#include <QApplication>
#include <QTime>

namespace Picto
{

void AudioRewardController::giveReward(unsigned int channel)
{
	//if(channel == 1)
	//{
		QApplication::beep();
		QCoreApplication::processEvents();
		
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