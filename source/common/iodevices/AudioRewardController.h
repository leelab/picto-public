
#ifndef _AUDIO_REWARD_CONTROLLER_H_
#define _AUDIO_REWARD_CONTROLLER_H_

#include <QSound>
#include <QtConcurrentRun>

#include "RewardController.h"


namespace Picto
{

/*! \brief A RewardController that simulates rewards using an audio signal for testing purposes.
 *
 *	In the Test Viewer, we don't have a real rewarder connected.  Instead, we use this rewarder 
 *	to generate an audible stimulus every time a reward is given.  The audio stimulus is a tick noise
 *	that is included in the Picto binaries.  It is currenly in the Workstation source under a sounds
 *	directory.  It should really be moved into the common directory.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API AudioRewardController : public RewardController
#else
class AudioRewardController :  public RewardController
#endif
{
	Q_OBJECT
public:
	AudioRewardController();
	virtual ~AudioRewardController() {};

protected:
	void startReward(unsigned int channel,int quantity);
	virtual bool rewardWasSupplied(unsigned int channel);
	virtual void startFlush(unsigned int channel);
	virtual void stopFlush(unsigned int channel);
private:

	QSound tick_;
	QTime timer_;
	bool firstRewardSupplied_;
	int latestQuantity_;

};

} //namespace Picto

#endif