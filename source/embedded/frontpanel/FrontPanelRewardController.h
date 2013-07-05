#ifndef _FrontPanelRewardController_H_
#define _FrontPanelRewardController_H_

#include "../../common/iodevices/RewardController.h"


namespace Picto
{
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

class FrontPanelRewardController :  public RewardController
{
public:
	FrontPanelRewardController();
	virtual ~FrontPanelRewardController();

protected:
	void startReward(unsigned int channel, int quantity);
	virtual bool rewardWasSupplied(unsigned int channel);
	virtual void startFlush(unsigned int channel);
	virtual void stopFlush(unsigned int channel);
private:
	bool hasDevice_;
	double outputData[2];
	//quint32 daqTaskHandle_[5]; // For Nidaqmx 8.5
	void* daqTaskHandle_[5];	// For Nidaqmx after 8.5
	int rewardLines_[5];
	Stopwatch stopwatch_[5];
	int latestOnTime_[5];

};

} //namespace Picto

#endif