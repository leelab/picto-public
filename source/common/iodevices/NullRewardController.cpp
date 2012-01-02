#include <QtDebug>
#include <QCoreApplication>
#include <QTime>

#include "NullRewardController.h"
#include "../memleakdetect.h"

namespace Picto
{


void NullRewardController::doReward(unsigned int channel,int quantity, int minRewardPeriod)
{
	qDebug()<<"Reward given on channel "<<channel;
	QTime timer;
	timer.start();
	int totalTime = minRewardPeriod;
	if(quantity > totalTime)
		totalTime = quantity;
	while(timer.elapsed() < totalTime)
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