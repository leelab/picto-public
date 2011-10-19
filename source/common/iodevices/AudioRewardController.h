
#ifndef _AUDIO_REWARD_CONTROLLER_H_
#define _AUDIO_REWARD_CONTROLLER_H_

#include "RewardController.h"
#include <QSound>


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
	~AudioRewardController() {};

public slots:
	void giveReward(unsigned int channel);
	void flush(unsigned int channel,bool flush);
private:
	QSound tick_;

};

} //namespace Picto

#endif