#include "PictoCompRewardController.h"
#include <windows.h>
#include "../../common/globals.h"

/*! The virtual COM port established between the Arduino and this PC.
*	\note Kristian - All of the PCs I've worked on have defaulted to COM3. This should probably be updated to search the 
*	active COM ports for the Arduino instead. But for now, please set the Arduino's COM port to 3 in Windows device manager.
*/
const char *PORT = "\\\\.\\COM3";

/*! The length in bytes of an Arduino command
*/
#define COMMAND_LENGTH 6

namespace Picto
{
	/*! \brief Constructs a PictoCompRewardController object.
	*	\details Creates a connection to an Arduino via a virtual COM Port. Reward and flush commands are sent to be handled by
	*	the separate reward box.
	*/
	PictoCompRewardController::PictoCompRewardController()
		: RewardController(NUMREWARDLINES)
	{
		arduino = new SerialPort(PORT);

		hasDevice_ = true;
	}

	PictoCompRewardController::~PictoCompRewardController()
	{
		arduino->~SerialPort();

		// Lines will go back to zero on their own as soon as things reset.
	}

	/*! \brief Sends a command to the arduino to give a reward for 'quantity' ms.
	*/
	void PictoCompRewardController::startReward(unsigned int channel, int quantity)
	{
		if (arduino->isConnected())
		{
			char buffer[COMMAND_LENGTH];

			sprintf(buffer, "1 %-4.4d", quantity);
			
			bool success = arduino->writeSerialPort(buffer, COMMAND_LENGTH);
			
			if (!success)
			{

			}
		}
		else
			arduino->reconnect(PORT);
	}

	/*! \brief Returns true once the Arduino sends a confirmation that the latest reward was supplied
	*/
	bool PictoCompRewardController::rewardWasSupplied(unsigned int channel)
	{
		if (arduino->reconnected)
		{
			arduino->reconnected = false;
			return true;
		}

		if (arduino->isConnected())
		{
			char result;

			int bytesRead = arduino->readSerialPort(&result, 1);

			if (bytesRead > 0)
			{
				return result == '1';
			}	
			else
				return false;	
		}
		else
			arduino->reconnect(PORT);

		return true;
	}

	/*! \brief Sends a command to the Arduino to start a flush.
	*/
	void PictoCompRewardController::startFlush(unsigned int channel)
	{
		if (arduino->isConnected())
		{
			bool success = arduino->writeSerialPort("2 0000", COMMAND_LENGTH);

			if (!success)
			{

			}
		}
		else
			arduino->reconnect(PORT);
	}

	/*! \brief Sends a command to the Arduino to stop a flush.
	*/
	void PictoCompRewardController::stopFlush(unsigned int channel)
	{
		if (arduino->isConnected())
		{
			bool success = arduino->writeSerialPort("3 0000", COMMAND_LENGTH);

			if (!success)
			{

			}
		}
		else
			arduino->reconnect(PORT);
	}

}; //namespace Picto