#ifndef _NULLREWARDCONTROLLER_H_
#define _NULLREWARDCONTROLLER_H_

#include "RewardController.h"


namespace Picto
{

/*!	\brief An empty reward controller
 *
 *	We don't always have reward hardware connected to the machine
 *	that is running a state machine.  This RewardController simply
 *	outputs a message to the debug console when a reward is issued.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API NullRewardController : public RewardController
#else
class NullRewardController :  public RewardController
#endif
{
public:
	NullRewardController() : RewardController(0) {};
	virtual ~NullRewardController() {};

public slots:
	void flush(unsigned int channel,bool flush);

protected:
	void doReward(unsigned int channel,int quantity, int minRewardPeriod);

};

} //namespace Picto

#endif