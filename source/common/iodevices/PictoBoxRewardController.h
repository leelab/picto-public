/*!	\brief A reward controller for the PictoBox
 *
 *	Since PictoBox uses a National Instruments DAQ card to drive reward
 *	solenoids, but doesn't use the standard National Instruments driver
 *	a special reward controller is required.
 */

#ifndef _PICTOBOXREWARDCONTROLLER_H_
#define _PICTOBOXREWARDCONTROLLER_H_

#include "RewardController.h"
#include "PictoBoxDaqBoard.h"

namespace Picto
{

#if defined WIN32 || defined WINCE
class PICTOLIB_API PictoBoxRewardController : public RewardController
#else
class PictoBoxRewardController :  public RewardController
#endif
{
public:
	PictoBoxRewardController(unsigned int channelCount);
	~PictoBoxRewardController();


	bool setRewardVolume(unsigned int channel, float volume);
	bool setRewardDuration(unsigned int channel, unsigned int duration);
public slots:
	void giveReward(unsigned int channel);

private:
	PictoBoxDaqBoard daqBoard_;

};

} //namespace Picto

#endif