
#ifndef _AUDIO_REWARD_CONTROLLER_H_
#define _AUDIO_REWARD_CONTROLLER_H_

#include <QSound>
#include <QtConcurrentRun>

#include "RewardController.h"


namespace Picto
{

/*! \brief An audio rewarder
 *
 *	During testing, it is likely that we won't have a real rewarder 
 *	connected.  Instead, we can use this rewarder to generate an
 *	audible stimulus every time a reward is given.  
 *
 *	Initially, this will use the QApplication::beep() function, but
 *	there's no reason the object can't be expanded to generate different
 *	types of noises.  (Phonon is probably the best approach for this...)
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API AudioRewardController : public RewardController
#else
class AudioRewardController :  public RewardController
#endif
{
public:
	AudioRewardController();
	virtual ~AudioRewardController() {};

public slots:
	void flush(unsigned int channel,bool flush);
protected:
	void startReward(unsigned int channel,int quantity);
	virtual bool rewardWasSupplied(unsigned int channel);
private:
	void playSound(int lengthMs);
	QSound tick_;
	QFuture<void> audioThreadFuture_;
	QTime timer_;
	int latestQuantity_;

};

} //namespace Picto

#endif