#ifndef _PICTOBOXXPREWARDCONTROLLER_H_
#define _PICTOBOXXPREWARDCONTROLLER_H_

#include "../../common/iodevices/RewardController.h"


namespace Picto
{
/*!	\brief A reward controller for the PictoBox
 *
 *	PictoBox uses a National Instruments PCI-6221 data acquisition board
 *	to deliver rewards (the same board is used for the 8 analog inputs, 8 digital outputs and event code
 *	outputs).  
 *	As long as PictoBox is running Windows XP->Windows 7, we can use the traditional NIDAQ MX drivers.
 *
 * The original concept for handling reward controller connections was to simply use the first four
 * lines of port 0 (ie. P0.0-P0.3) for to control the relays that handle the reward outputs.  The problem
 * with this was that we wanted to be able to use timers to turn off the pins for precise reward delivery
 * timing.  We are theoretically supposed to be able to configure the nidaq card so that digital lines 
 * can use clocks from other parts of the card, but I have not yet figured out how to do this correctly.
 * Instead, currently we only support one reward controller where the reward trigger line is the analog
 * output A0.0.  We are using A0.1 as the line that enables the relays, so with the current setup we can't
 * use more than one reward line.  The reason for using A0.1 as a reward enable line is that if we simply 
 * connect reward enable to 5V, then when the Pictobox is restarted, 5V is on the reward enable line, but 
 * the A0.0 line defaults to 0V on startup.  Since the Crouznet PB-4 Digital I/O board containing our realays
 * uses active low logic to enable the relays, we would therefore end up basically flushing on the reward
 * controller every time we restarted Picto, which would be a bad thing.  If we ever need to support multiple
 * reward lines, someone will either have to spend a lot more time than I had working through the NiDaq
 * documentation and experimenting, or, more likely, we will need to purchase an updated NiDaq card that 
 * has individual clocks available for 4 output lines.
 *
 * \note The Crouzet PB-4 Digital I/O board uses active low logic to activate the reward relays.
 * \author Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2013
 */
class PictoBoxXPRewardController :  public RewardController
{
public:
	PictoBoxXPRewardController();
	virtual ~PictoBoxXPRewardController();

protected:
	void startReward(unsigned int channel, int quantity);
	virtual bool rewardWasSupplied(unsigned int channel);
	virtual void startFlush(unsigned int channel);
	virtual void stopFlush(unsigned int channel);
private:
	bool hasDevice_;
	double outputData[2];
	//quint32 daqTaskHandle_[5]; // For Nidaqmx 8.5
	void*  daqTaskHandle_[5];	// For Nidaqmx after 8.5
	int rewardLines_[5];
	Stopwatch stopwatch_[5];
	int latestOnTime_[5];

};

} //namespace Picto

#endif