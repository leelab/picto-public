/*!	\brief A reward controller for the PictoBox
 *
 *	Since PictoBox uses a National Instruments DAQ card to drive reward
 *	solenoids, but doesn't use the standard National Instruments driver
 *	a special reward controller is required.
 */

#ifndef _PICTOBOXREWARDCONTROLLERCE_H_
#define _PICTOBOXREWARDCONTROLLERCE_H_

#include "RewardController.h"
#include "PictoBoxDaqBoard.h"

namespace Picto
{

#if defined WIN32 || defined WINCE
class PICTOLIB_API PictoBoxRewardControllerCE : public RewardController
#else
class PictoBoxRewardControllerCE :  public RewardController
#endif
{
public:
	PictoBoxRewardControllerCE(unsigned int channelCount);
	~PictoBoxRewardControllerCE();


	bool setRewardVolume(unsigned int channel, float volume);
	bool setRewardDurationMs(unsigned int channel, unsigned int duration);
public slots:
	void giveReward(unsigned int channel);

private:
	PictoBoxDaqBoard daqBoard_;

};

} //namespace Picto

#endif