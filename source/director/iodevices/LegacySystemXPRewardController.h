#ifndef _LEGACYSYSTEMXPREWARDCONTROLLER_H_
#define _LEGACYSYSTEMXPREWARDCONTROLLER_H_

#include "../../common/iodevices/RewardController.h"


namespace Picto
{
/*!	\brief A reward controller for the LegacySystem used when running Windows XP
 *
 *	LegacySystem uses a National Instruments PCI-6503 data acquisition board
 *	to deliver rewards (the same board is used for triggering event codes).  
 *	As long as LegacySystem is running Windows XP, we can 
 *	use the traditional NIDAQ MX drivers.
 *
 * The reward controller MUST be connected to the Port 2 pin 7 on the DAQ for this to work.
 * 
 * PIN CONNECTIONS:
 * 		Reward controller	  Port0 bit
 * 			1					P2.7	
 *
 * \note The Legacy System uses requires active high logic for reward delivery
 */

class LegacySystemXPRewardController :  public RewardController
{
public:
	LegacySystemXPRewardController(unsigned int channelCount);
	~LegacySystemXPRewardController();

public slots:
	void giveReward(unsigned int channel);
	void flush(unsigned int channel,bool flush);

private:
	quint32 daqTaskHandle_;
	int rewardLines_[4];
	bool hasDevice_;

};

} //namespace Picto

#endif