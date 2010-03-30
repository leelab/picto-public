/*!	\brief A reward controller for the PictoBox used when running Windows XP
 *
 *	PictoBox uses a National Instruments PCI-6221 data acquisition board
 *	to deliver rewards (the same board is used for a number of other 
 *	functions as well).  As long as PictoBox is running Windows XP, we can 
 *	use the traditional NIDAQ MX drivers.
 *
 * The reward controllers MUST be connected to the Port 0 pins on the DAQ for this to work.
 * If at some point we decide to connect the reward controllers to the PFI pins,
 * this function will have to be rewritten.
 * 
 * PIN CONNECTIONS:
 * 		Reward controller	  Port0 bit		Pin on PCI-6221
 * 			1					P0.0			52
 * 			2					P0.1			17
 * 			3					P0.2			49
 * 			4					P0.3			47
 *
 * \note The Crouzet PB-4 Digital I/O board uses active low logic.

 */

#ifndef _PICTOBOXXPREWARDCONTROLLER_H_
#define _PICTOBOXXPREWARDCONTROLLER_H_

#include "../../common/iodevices/RewardController.h"


namespace Picto
{
//#if defined WIN32 || defined WINCE
//class PICTOLIB_API PictoBoxXPRewardController : public RewardController
//#else
class PictoBoxXPRewardController :  public RewardController
//#endif
{
public:
	PictoBoxXPRewardController(unsigned int channelCount);
	~PictoBoxXPRewardController();

	bool setRewardVolume(unsigned int channel, float volume);
	bool setRewardDurationMs(unsigned int channel, unsigned int duration);
	int getRewardDurationMs(unsigned int channel);
	bool setRewardResetTimeMs(unsigned int channel, unsigned int time);

public slots:
	void giveReward(unsigned int channel);
	void flush(unsigned int channel,bool flush);

private:
	quint32 daqTaskHandle_;
	int rewardLines_[4];

};

} //namespace Picto

#endif