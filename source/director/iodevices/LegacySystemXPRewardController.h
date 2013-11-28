#ifndef _LEGACYSYSTEMXPREWARDCONTROLLER_H_
#define _LEGACYSYSTEMXPREWARDCONTROLLER_H_

#include "../../common/iodevices/RewardController.h"
#include "../../common/timing/stopwatch.h"


namespace Picto
{
/*!	\brief A reward controller for the LegacySystem (ie. an old Orion computer being used
 *	as a Picto Director).
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
	virtual ~LegacySystemXPRewardController();

protected:
	void startReward(unsigned int channel,int quantity);
	virtual bool rewardWasSupplied(unsigned int channel);
	virtual void startFlush(unsigned int channel);
	virtual void stopFlush(unsigned int channel);
private:
	//quint32 daqTaskHandle_; // For Nidaqmx 8.5
	void*  daqTaskHandle_;	// For Nidaqmx after 8.5
	int rewardLines_[4];
	bool hasDevice_;
	double outputData[2];
	bool taskExists_;
	Stopwatch stopwatch_;
	int latestOnTime_;

};

} //namespace Picto

#endif