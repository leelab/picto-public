#include "AudioRewardController.h"

#include <QApplication>
#include <QTime>

namespace Picto
{

bool AudioRewardController::setRewardVolume(unsigned int channel, float volume)
{
	Q_UNUSED(channel);
	Q_UNUSED(volume);
	return true;
}

bool AudioRewardController::setRewardDurationMs(unsigned int channel, unsigned int duration)
{
	Q_UNUSED(channel);
	Q_UNUSED(duration);

	return true;
}

int AudioRewardController::getRewardDurationMs(unsigned int channel)
{
	Q_UNUSED(channel);
	return 0;
}

bool AudioRewardController::setRewardResetTimeMs(unsigned int channel, unsigned int time)
{
	Q_UNUSED(channel);
	Q_UNUSED(time);
	return true;
}

void AudioRewardController::giveReward(unsigned int channel)
{
	if(channel == 1)
	{
		QApplication::beep();
		
		//If we call beep too quickly, they all run together, 
		//so we have to pause after each call
		QTime timer;
		timer.start();
		while(timer.elapsed()<300);
	}

}

void AudioRewardController::flush(unsigned int channel,bool flush)
{
	Q_UNUSED(channel);
	Q_UNUSED(flush);
}

} //namespace Picto