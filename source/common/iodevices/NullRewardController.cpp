#include <QtDebug>
#include <QCoreApplication>
#include <QTime>

#include "NullRewardController.h"
#include "../memleakdetect.h"

namespace Picto
{


void NullRewardController::startReward(unsigned int channel,int)
{
	qDebug()<<"Reward given on channel "<<channel;
}

void NullRewardController::stopReward(unsigned int channel)
{
	qDebug()<<"Reward stoppped on channel "<<channel;
}

bool NullRewardController::rewardWasSupplied(unsigned int channel)
{
	return true;
}

void NullRewardController::flush(unsigned int channel,bool flush)
{
	if(flush)
		qDebug()<<"Start flush on channel "<<channel;
	else
		qDebug()<<"Stop flush on channel "<<channel;

}

} //namespace Picto