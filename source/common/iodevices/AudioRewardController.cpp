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

void AudioRewardController::doReward(unsigned int,int quantity, int minRewardPeriod)
{
		QTime timer;
		timer.start();
		tick_.play();
		int totalTime = minRewardPeriod;
		if(quantity > totalTime)
			totalTime = quantity;
		while(timer.elapsed() < totalTime)
			QCoreApplication::processEvents();
}

void AudioRewardController::flush(unsigned int channel,bool flush)
{
	Q_UNUSED(channel);
	Q_UNUSED(flush);
}

} //namespace Picto