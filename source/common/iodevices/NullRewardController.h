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
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	void startReward(unsigned int channel,int quantity);
	virtual bool rewardWasSupplied(unsigned int channel);
	virtual void startFlush(unsigned int channel);
	virtual void stopFlush(unsigned int channel);

};

} //namespace Picto

#endif