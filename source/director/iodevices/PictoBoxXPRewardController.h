#ifndef _PICTOBOXXPREWARDCONTROLLER_H_
#define _PICTOBOXXPREWARDCONTROLLER_H_

#include "../../common/iodevices/RewardController.h"


namespace Picto
{
	/*!	\brief A reward controller for the PictoBox
	*
	*	PictoBox uses a National Instruments PCIe-6321 data acquisition board
	*	to deliver rewards (the same board is used for the 8 analog inputs, 8 digital outputs and event code
	*	outputs).
	*	As long as PictoBox is running Windows XP->Windows 7(8?), we can use the traditional NIDAQ MX drivers.
	*
	*  Pictobox5/System10 uses an older NIDAQ version E with reward delivered via analog output while the other
	*  Pictoboxes use the series M NIDAQ with reward delivered via digital output.
	*  The intention of the design is for the four free digital output pins tied to the corresponding clocks
	*  to control the four reward channels.  Currently only one reward channel is connected. Dev/ctr0 outputs
	*  through PFI 12 on the card.  The other pulse-capable pins are PFI 13, 14, and 15, all currently free.
	*
	*  The digital output PFI 9 is the enable line for the relay board.  The High-enable logic is to prevent
	*  free rewards from being dispensed when Picto boots up.
	*
	* \note The Crouzet PB-4 Digital I/O board uses active low logic to activate the reward relays.
	* \authors Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
	* \date 2009-2015
	*/
	class PictoBoxXPRewardController : public RewardController
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
		void*  daqTaskHandle_[5]; //now works for for both numeric (old series E DAQ on pictobox5/system10) and digital (all other pictoboxes with series M DAQ)
		int rewardLines_[5];
		Stopwatch stopwatch_[5];
		int latestOnTime_[5];

	};

} //namespace Picto

#endif