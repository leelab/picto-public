#ifndef _LEGACYSYSTEMXPREWARDCONTROLLER_H_
#define _LEGACYSYSTEMXPREWARDCONTROLLER_H_

#include "../../common/iodevices/RewardController.h"
#include "../../common/timing/stopwatch.h"


namespace Picto
{
/*!	\brief A reward controller for the LegacySystem (ie. an old Orion computer being used
 *	as a Picto Director).
 *
 *	In order to support precise timing using a reward clock, we had to change things a little
 *	bit from the Orion system here.  Whereas Orion is connected up such that the PCI-6503 board
 *	used for event delivery is also used for reward delivery, in Picto, in order to be able to use the clock timing
 *	we needed to use the other Orion card, the PCI-MIO-16XE-50.  For this reason, when running Picto on a legacy Orion
 *	box we need to open up the breakout box and connect the reward controller BNC to the Ao0 port (pin 22).
 *
 *	For more detail on the precise reward timing system, see startReward().
 *
 *	\note The Legacy System requires active high logic for reward delivery
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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