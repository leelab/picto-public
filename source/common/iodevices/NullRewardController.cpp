#include "NullRewardController.h"

#include <QtDebug>

namespace Picto
{

bool NullRewardController::setRewardVolume(unsigned int channel, float volume)
{
	qDebug()<<"Reward volume on channel "<<channel<<" set to "<<volume;
	return true;
}

bool NullRewardController::setRewardDurationMs(unsigned int channel, unsigned int duration)
{
	qDebug()<<"Reward duration on channel "<<channel<<" set to "<<duration<<" ms.";
	return true;
}

int NullRewardController::getRewardDurationMs(unsigned int channel)
{
	Q_UNUSED(channel);
	return 0;
}

bool NullRewardController::setRewardResetTimeMs(unsigned int channel, unsigned int time)
{
	qDebug()<<"Reward reset time on channel "<<channel<<" set to "<<time<<" ms.";
	return true;
}

void NullRewardController::giveReward(unsigned int channel)
{
	qDebug()<<"Reward given on channel "<<channel;
}

void NullRewardController::flush(unsigned int channel,bool flush)
{
	if(flush)
		qDebug()<<"Start flush on channel "<<channel;
	else
		qDebug()<<"Stop flush on channel "<<channel;

}

} //namespace Picto