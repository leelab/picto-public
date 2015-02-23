#include <QtDebug>
#include <QCoreApplication>
#include <QTime>

#include "NullRewardController.h"
#include "../memleakdetect.h"

namespace Picto
{

/*! \brief Extends RewardController::startReward() to print a message to the debug output.
 */
void NullRewardController::startReward(unsigned int channel,int)
{
	qDebug()<<"Reward given on channel "<<channel;
}

/*! \brief Extends RewardController::rewardWasSupplied() to simply always return true
 */
bool NullRewardController::rewardWasSupplied(unsigned int channel)
{
	UNREFERENCED_PARAMETER(channel);
	return true;
}
/*! \brief Extends RewardController::startFlush() to print out a message to the debug output.*/
void NullRewardController::startFlush(unsigned int channel)
{
	qDebug()<<"Flush started on channel "<<channel;
}
/*! \brief Extends RewardController::stopFlush() to print out a message to the debug output.*/
void NullRewardController::stopFlush(unsigned int channel)
{
	qDebug()<<"Stop Flush called on channel "<<channel;
}

/*! \brief This should be removed since RewardController::flush() is no longer virtual.
 */
void NullRewardController::flush(unsigned int channel,bool flush)
{
	if(flush)
		qDebug()<<"Start flush on channel "<<channel;
	else
		qDebug()<<"Stop flush on channel "<<channel;

}

} //namespace Picto