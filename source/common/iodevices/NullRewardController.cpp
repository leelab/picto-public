#include <QtDebug>
#include <QCoreApplication>
#include <QTime>

#include "NullRewardController.h"
#include "../memleakdetect.h"

namespace Picto
{


void NullRewardController::giveReward(unsigned int channel,int quantity)
{
	qDebug()<<"Reward given on channel "<<channel;
	QTime timer;
	timer.start();
	while(timer.elapsed() < rewardResetTimes_[channel-1] + quantity)
		QCoreApplication::processEvents();
}

void NullRewardController::flush(unsigned int channel,bool flush)
{
	if(flush)
		qDebug()<<"Start flush on channel "<<channel;
	else
		qDebug()<<"Stop flush on channel "<<channel;

}

} //namespace Picto