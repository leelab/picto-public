#ifndef _PICTOCOMPREWARDCONTROLLER_H_
#define _PICTOCOMPREWARDCONTROLLER_H_

#include "../../common/iodevices/RewardController.h"
#include "SerialPort.hpp"


namespace Picto
{
	/*!	\brief A reward controller for the new Pictocomputer setup
	*
	*	The Pictocomputer separates the old pictobox into three components: A PC, a reward box, and a breakout board for the NI DAQ. This should make it 
	*	easier for labs to use Picto, since most of the components can be purchased in full working order from the vendor of their choice. They'll still
	*	have to build their own reward delivery system, but that is standard for animal research labs.
	*
	*	The Pictocomputer can connect to an Arduino-controlled reward box to deliver rewards. This is done over a virtual COM port that the Arduino creates 
	*	when connected to the PC.
	*
	* \authors Kristian Osborne
	* \date 2021
	*/
	class PictoCompRewardController : public RewardController
	{
	public:
		PictoCompRewardController();
		virtual ~PictoCompRewardController();

	protected:
		void startReward(unsigned int channel, int quantity);
		virtual bool rewardWasSupplied(unsigned int channel);
		virtual void startFlush(unsigned int channel);
		virtual void stopFlush(unsigned int channel);
	private:
		bool hasDevice_;
		int NUMREWARDLINES = 1;
		SerialPort *arduino;

	};

} //namespace Picto

#endif