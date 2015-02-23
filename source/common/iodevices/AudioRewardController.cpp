#include <QApplication>
#include <QTime>
#include <QTimer>

#include "AudioRewardController.h"
#include "../memleakdetect.h"

namespace Picto
{

AudioRewardController::AudioRewardController() : 
RewardController(1) ,
tick_(":/sounds/tick.wav"),
latestQuantity_(0),
firstRewardSupplied_(false)
{
}

/*! \brief Extends RewardController::startReward() to play a simple tick sound.
 */
void AudioRewardController::startReward(unsigned int,int quantity)
{
	latestQuantity_ = quantity;
	timer_.start();
	tick_.play();
	firstRewardSupplied_ = true;
}

/*! \brief Extends RewardController::rewardWasSupplied() to return whether the time
 *	specified in quantity of startReward() has elapsed.  
 *	\details We just use a timer to check this since the reward sound is of a fixed length.
 */
bool AudioRewardController::rewardWasSupplied(unsigned int)
{
	if((timer_.elapsed() <= latestQuantity_) && firstRewardSupplied_)
		return false;
	return true;
}
void AudioRewardController::startFlush(unsigned int)
{
	startReward(0,50);	//Since the sound is so short and flushing is really meaningless, just play it quickly.
}
void AudioRewardController::stopFlush(unsigned int channel)
{
	UNREFERENCED_PARAMETER(channel);
	latestQuantity_ = timer_.elapsed();
}

} //namespace Picto