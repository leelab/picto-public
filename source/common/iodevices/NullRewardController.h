/*!	\brief An empty reward controller
 *
 *	We don't always have reward hardware connected to the machine
 *	that is running a state machine.  This RewardController simply
 *	outputs a message to the debug console when a reward is issued.

 */

#ifndef _NULLREWARDCONTROLLER_H_
#define _NULLREWARDCONTROLLER_H_

#include "RewardController.h"


namespace Picto
{
#if defined WIN32 || defined WINCE
class PICTOLIB_API NullRewardController : public RewardController
#else
class NullRewardController :  public RewardController
#endif
{
public:
	NullRewardController() : RewardController(0) {};
	~NullRewardController() {};

public slots:
	void giveReward(unsigned int channel);
	void flush(unsigned int channel,bool flush);

};

} //namespace Picto

#endif