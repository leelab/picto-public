//Matthew Gay
//August 5, 2009
//Lee Lab - Yale School of Medicine

#include <QVector>
#include <QtGlobal>
#include <QTextStream>
#include "PictoBoxDaqBoard.h"
#include "PictoBoxDaqBoardRegisterMap.h"

namespace Picto {
//******************************************************/
// DaqBoard()
//******************************************************/
//! Create a new DaqBoard object
/*!
 * This creates a new object and opens the "file" for communicating with the DAQ board.
 */
//******************************************************/

PictoBoxDaqBoard::PictoBoxDaqBoard()
{
	hDAQ_ = CreateFile(_T("DAQ1:"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,0);
	Q_ASSERT(hDAQ_);

	rewardControllersInitialized_ = false;
	eventLinesInitialized_ = false;
}

//******************************************************/
// ~DaqBoard()
//******************************************************/
//! Destroy a DaqBoard object
/*!
 * This closes the "file" used for communicating with the DAQ board.
 */
//******************************************************/
PictoBoxDaqBoard::~PictoBoxDaqBoard()
{
	CloseHandle(hDAQ_);
}

//******************************************************/
// initRewardController
//******************************************************/
//! Initializes the Reward controllers.
 /*!
  * This function must be called before doing anything else with the reward
  * controllers.  It returns true if the controllers were successfully initialized, 
  * otherwise it returns false.
  * 
  * The reward controllers MUST be connected to the DIO pins on the DAQ for this to work.
  * If at some point we decide to connect the reward controllers to the PFI pins,
  * this function will have to be rewritten.
  * 
  * PIN CONNECTIONS:
  * 		Reward controller	DIO bit		Pin on PCI-6221
  * 			1					0			52
  * 			2					1			17
  * 			3					2			49
  * 			4					3			47
  *
  * \note The Crouzet PB-4 Digital I/O board uses active low logic.
  * 
  * \param _numControllers The number of reward controllers to initialize
  * \param _rewardDurations an array containging the durations in ms for each controller. The array must have the same number of elements as the number of controllers being initialized.
  */ 
//******************************************************/
bool PictoBoxDaqBoard::initRewardController(int _numControllers, unsigned int _rewardDurations[])
{
	if(hDAQ_ == NULL)
	{
		OutputDebugString(L"DaqBoard.initRewardController: Null DAQ handle\n");
		return false;
	}
	if(_numControllers > MAX_NUM_REWARD_CONTROLLERS || _numControllers < 0)
	{
		OutputDebugString(L"DaqBoard.initRewardController: Invalid number of reward controllers specified\n");
		return false;
	}
	if(rewardControllersInitialized_)
	{
		OutputDebugString(L"DaqBoard.initRewardController: Reward controllers alread initialized\n");
		return false;
	}

	activeRewardControllers_ = _numControllers;
	for(int i=0; i<activeRewardControllers_; i++)
	{
		rewardDurations_[i] = _rewardDurations[i];
		rewardBitmasks_[i] = 1 << i;
	}

	//Set the DIO pins to output.  I'm setting them all to output because this
	//makes life easier.   If at some point we want to use the other pins for 
	//something else, we'll have to change this.
	DWORD data = 0xFFFFFFFF;
	DWORD bytesWritten;

	SetFilePointer(hDAQ_, REG_DIO_DIRECTION,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, FOUR_BYTES, &bytesWritten, NULL);

	if(bytesWritten != FOUR_BYTES)
	{
		OutputDebugString(L"DaqBoard.initRewardController: Failed writing to DIO_DIRECTION register\n");
		return false;
	}

	//set the reward controller values to 1 (off)
	data = 0x0000000F;
	SetFilePointer(hDAQ_,REG_STATIC_DIGITAL_OUTPUT,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data,FOUR_BYTES,&bytesWritten,NULL);

	if(bytesWritten != FOUR_BYTES)
	{
		OutputDebugString(L"DaqBoard.initRewardController: Failed writing to DIO_DIRECTION register\n");
		return false;
	}


	rewardControllersInitialized_ = true;
	
	return true;

}

//******************************************************/
// setRewardDuration
//******************************************************/
//! Sets the reward duration in ms for the specified controller.
/*!
 * Sets the duration for the reward controllers.  The durations are already set
 * in the initRewardController function, so this function would only be used if 
 * you wanted to change the duration in the middle of a program.
 * 
 * \param _controller Controllers are numbered starting at 1.
 * \param _duration The duration in ms
 * 
 * \return true if the action was successful.
 */
//******************************************************/
bool PictoBoxDaqBoard::setRewardDuration(int _controller, unsigned int _duration)
{
	if(_controller > activeRewardControllers_)
	{
		OutputDebugString(L"DaqBoard.setRewardDuration: Invalid reward controller number\n");
		return false;
	}
	if(_duration < 0 || _duration > 10000)
	{
		OutputDebugString(L"DaqBoard.setRewardDuration: Invalid rewward duration\n");
		return false;
	}
	if(!rewardControllersInitialized_)
	{
		OutputDebugString(L"DaqBoard.setRewardDuration: Reward controllers not initialized\n");
		return false;
	}
	rewardDurations_[_controller-1] = _duration;
	return true;
}

//******************************************************/
// getRewardDuration
//******************************************************/
//! Gets the reward duration in ms for the specified controller.
/*!
 * Gets the duration for the reward controllers.  The durations are already set
 * in the initRewardController function, so this function would only be used if 
 * you wanted to change the duration in the middle of a program.
 * 
 * \param _controller Controllers are numbered starting at 1.
 * 
 * \return currently set duration (or -1 for an invaldi controller).
 */
//******************************************************/
unsigned int PictoBoxDaqBoard::getRewardDuration(int _controller)
{
	if(_controller > activeRewardControllers_)
		return -1;
	else if(!rewardControllersInitialized_)
		return -1;
	else
		return rewardDurations_[_controller-1];
}


//******************************************************/
// giveReward
//******************************************************/
//! Turns on the reward controller for a fixed duration
/*!
 * This function turns on the specified reward controller for the duration
 * to which it has previously been set.
 *
 * \param _controller Controllers are numbered starting at 1.
 *
 * \return true if the action was successful.
 */
//******************************************************/
bool PictoBoxDaqBoard::giveReward(int _controller)
{
	DWORD buffer;
	DWORD bytesWritten,bytesRead;

	if(_controller > activeRewardControllers_)
	{
		OutputDebugString(L"DaqBoard.giveReward: Invalid reward controller number\n");
		return false;
	}
	if(hDAQ_ == NULL)
	{
		OutputDebugString(L"DaqBoard.giveReward: Null DAQ handle\n");
		return false;
	}
	if(!rewardControllersInitialized_)
	{
		OutputDebugString(L"DaqBoard.giveReward: Reward controllers not initialized\n");
		return false;
	}


	//Turn on the reward controller (set the bit to 0)
	SetFilePointer(hDAQ_,REG_STATIC_DIGITAL_OUTPUT,NULL,FILE_BEGIN);
	ReadFile(hDAQ_,&buffer,FOUR_BYTES,&bytesRead,NULL);
	buffer &= ~rewardBitmasks_[_controller-1];
	SetFilePointer(hDAQ_,REG_STATIC_DIGITAL_OUTPUT,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&buffer, FOUR_BYTES,&bytesWritten, NULL);



	//Delay for appropriate duration
	//code stolen/modified from MSDN:
	//http://msdn.microsoft.com/en-us/library/ms836797.aspx
	LARGE_INTEGER ticksPerMs;
	LARGE_INTEGER currTicks,endTicks;
	// Query number of ticks per second
	if (QueryPerformanceFrequency(&ticksPerMs))
	{
		//get ticks per ms
		ticksPerMs.QuadPart /= 1000;

		// Get current ticks
		if (QueryPerformanceCounter(&currTicks))
		{
			// Create timeout value
			endTicks.QuadPart = currTicks.QuadPart + ticksPerMs.QuadPart*rewardDurations_[_controller-1];
			do
			{
				// Get current ticks
				QueryPerformanceCounter(&currTicks);
				// Delay until timeout
			} while (currTicks.QuadPart<endTicks.QuadPart);
		}
	}

	//turn off the reward controller (set the bit to 1)
	SetFilePointer(hDAQ_,REG_STATIC_DIGITAL_OUTPUT,NULL,FILE_BEGIN);
	ReadFile(hDAQ_,&buffer,FOUR_BYTES,&bytesRead,NULL);
	buffer |= rewardBitmasks_[_controller-1];
	SetFilePointer(hDAQ_,REG_STATIC_DIGITAL_OUTPUT,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&buffer, FOUR_BYTES,&bytesWritten, NULL);

	return true;


}

//******************************************************/
// flushReward
//******************************************************/
//! Used to turn on or off the reward controller for flushing.

/*!
 * This function turns is used to flush the reward tubes.  The function opens
 * or closes the reward controller, and leaves it in that state until told 
 * otherwise.
 *
 * \param _controller Controllers are numbered starting at 1.
 * \param _flush Turns on (true) or off (false) flushing
 *
 * \return true if the action was successful.
 */
//******************************************************/
bool PictoBoxDaqBoard::flushReward(int _controller, bool _flush)
{
	DWORD buffer;
	DWORD bytesWritten,bytesRead;

	if(_controller > activeRewardControllers_)
	{
		OutputDebugString(L"DaqBoard.flushReward: Invalid reward controller number\n");
		return false;
	}
	if(hDAQ_ == NULL)
	{
		OutputDebugString(L"DaqBoard.flushReward: Null DAQ handle\n");
		return false;
	}
	if(!rewardControllersInitialized_)
	{
		OutputDebugString(L"DaqBoard.flushReward: Reward controllers not initialized\n");
		return false;
	}


	if(_flush)
	{
		//Turn on the reward controller (set it to 0, since we're using active low logic)
		SetFilePointer(hDAQ_,REG_STATIC_DIGITAL_OUTPUT,NULL,FILE_BEGIN);
		ReadFile(hDAQ_,&buffer,FOUR_BYTES,&bytesRead,NULL);
		buffer &= ~rewardBitmasks_[_controller-1];
		SetFilePointer(hDAQ_,REG_STATIC_DIGITAL_OUTPUT,NULL,FILE_BEGIN);
		WriteFile(hDAQ_,&buffer, FOUR_BYTES,&bytesWritten, NULL);
	}
	else
	{
		//turn off the reward controller
		SetFilePointer(hDAQ_,REG_STATIC_DIGITAL_OUTPUT,NULL,FILE_BEGIN);
		ReadFile(hDAQ_,&buffer,FOUR_BYTES,&bytesRead,NULL);
		buffer |= rewardBitmasks_[_controller-1];
		SetFilePointer(hDAQ_,REG_STATIC_DIGITAL_OUTPUT,NULL,FILE_BEGIN);
		WriteFile(hDAQ_,&buffer, FOUR_BYTES,&bytesWritten, NULL);
	}
	return true;

}

//******************************************************/
// initEventLines
//******************************************************/
//! Sets up the lines used for sending even codes
/*!
 * This function must be called before attempting to send event codes
 * to the neural recording devices.  The function basically sets up
 * PFI[0:7] as digital output.
 *
 *
 * \return true if sucessful
 *
 */
//******************************************************/
bool PictoBoxDaqBoard::initEventLines()
{
	//set PFI[0:7] as output
	//Each register contains 3 5-bit values, which are right justified
	//For example, bits 4:0 of register 1 control PFI0, bits 9:4
	//of register 1 control PFI1, etc.

	//There are a lot of possible settings (see the M-series register map),
	//but the only one we care about is 16, which sets the line as PFI_DO.
	//We're going to set PFI[7:0] to PFI_DO (16), and the remaining lines
	//to default (0).

	if(hDAQ_ == NULL)
	{
		OutputDebugString(L"DaqBoard.initEventLines: Null DAQ handle\n");
		return false;
	}
	if(eventLinesInitialized_)
	{
		OutputDebugString(L"DaqBoard.initEventLines: Event lines alread initialized\n");
		return false;
	}

	DWORD data;
	DWORD bytesWritten;
	
	//REG_IO_BIDIRECTION_PIN: 0x00FF (1=output, 0=input)
	data = 0x00FF;
	SetFilePointer(hDAQ_, REG_IO_BIDIRECTION_PIN, NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten, NULL);
	if(bytesWritten != TWO_BYTES)
	{
		OutputDebugString(L"DaqBoard.initEventLines: Failed writing to REG_IO_BIDIRECTION_PIN register\n");
		return false;
	}

	//  REG_PFI_OUTPUT_SELECT_1: 0x4210
	data = 0x4210;
	SetFilePointer(hDAQ_, REG_PFI_OUTPUT_SELECT_1,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten, NULL);
	if(bytesWritten != TWO_BYTES)
	{
		OutputDebugString(L"DaqBoard.initEventLines: Failed writing to REG_PFI_OUTPUT_SELECT_1 register\n");
		return false;
	}

	//  REG_PFI_OUTPUT_SELECT_2: 0x4210
	data = 0x4210;
	SetFilePointer(hDAQ_, REG_PFI_OUTPUT_SELECT_2,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten, NULL);
	if(bytesWritten != TWO_BYTES)
	{
		OutputDebugString(L"DaqBoard.initEventLines: Failed writing to REG_PFI_OUTPUT_SELECT_2 register\n");
		return false;
	}

	//  REG_PFI_OUTPUT_SELECT_3: 0x0210
	data = 0x0210;
	SetFilePointer(hDAQ_, REG_PFI_OUTPUT_SELECT_3,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten, NULL);
	if(bytesWritten != TWO_BYTES)
	{
		OutputDebugString(L"DaqBoard.initEventLines: Failed writing to REG_PFI_OUTPUT_SELECT_3 register\n");
		return false;
	}

	//  REG_PFI_OUTPUT_SELECT_4: 0x0000
	data = 0x0000;
	SetFilePointer(hDAQ_, REG_PFI_OUTPUT_SELECT_4,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten, NULL);
	if(bytesWritten != TWO_BYTES)
	{
		OutputDebugString(L"DaqBoard.initEventLines: Failed writing to REG_PFI_OUTPUT_SELECT_4 register\n");
		return false;
	}

	//  REG_PFI_OUTPUT_SELECT_5: 0x0000
	data = 0x0000;
	SetFilePointer(hDAQ_, REG_PFI_OUTPUT_SELECT_5,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten, NULL);
	if(bytesWritten != TWO_BYTES)
	{
		OutputDebugString(L"DaqBoard.initEventLines: Failed writing to REG_PFI_OUTPUT_SELECT_5 register\n");
		return false;
	}

	//  REG_PFI_OUTPUT_SELECT_6: 0x0000
	data = 0x0000;
	SetFilePointer(hDAQ_, REG_PFI_OUTPUT_SELECT_6,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten, NULL);
	if(bytesWritten != TWO_BYTES)
	{
		OutputDebugString(L"DaqBoard.initEventLines: Failed writing to REG_PFI_OUTPUT_SELECT_6 register\n");
		return false;
	}

	//clear PFI lines
	data = 0x0;
	SetFilePointer(hDAQ_, PFI_DO,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, NULL, NULL);


	eventLinesInitialized_ = true;
	return true;
}

//******************************************************/
// sendEvent
//******************************************************/
//! Outputs a single 7-bit event code with a 1 bit strobe signal using the PFI outputs.
/*!
 * For sending events to a neural recording device (such as a TDT or Plexon), this
 * function sets an event code on PFI[0:6], and then strobes PFI7 high for 250 us
 *
 * \param _eventCode Seven-bit event code (the MSB will be ignored)
 *
 * \return true if sucessful
 *
 */
//******************************************************/
bool PictoBoxDaqBoard::sendEvent(unsigned char _eventCode)
{
	if(!eventLinesInitialized_)
		return false;

	DWORD data;
	
	//set event code on PFI lines
	data = _eventCode & 0x7F;
	SetFilePointer(hDAQ_, PFI_DO,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, NULL, NULL);

	//set strobe high
	data = (_eventCode & 0x7F) | 0x80;
	SetFilePointer(hDAQ_, PFI_DO,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, NULL, NULL);

	//wait 250 us
	//code stolen/modified from MSDN:
	//http://msdn.microsoft.com/en-us/library/ms836797.aspx
	LARGE_INTEGER ticksPerUs;
	LARGE_INTEGER currTicks,endTicks;
	int delay = 250; //delay in us
	// Query number of ticks per second
	if (QueryPerformanceFrequency(&ticksPerUs))
	{
		//get ticks per ms
		ticksPerUs.QuadPart /= 1000000;

		// Get current ticks
		if (QueryPerformanceCounter(&currTicks))
		{
			// Create timeout value
			endTicks.QuadPart = currTicks.QuadPart + ticksPerUs.QuadPart*delay;
			do
			{
				// Get current ticks
				QueryPerformanceCounter(&currTicks);
				// Delay until timeout
			} while (currTicks.QuadPart<endTicks.QuadPart);
		}
	}

	//set strobe low
	data = _eventCode & 0x7F;
	SetFilePointer(hDAQ_, PFI_DO,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, NULL, NULL);

	//clear PFI lines
	data = 0x0;
	SetFilePointer(hDAQ_, PFI_DO,NULL, FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, NULL, NULL);

	return true;
}

//******************************************************/
// CreateAIChannel
//******************************************************/
//! Creates a new analog input (AI) channel, and adds it to the list of channels
/*!
 * Adds the channel to the channel list stored in the object.  AI Channels can be
 * created in any order, however you can't create a channel more than once without
 * first removing it.  Channel numbers correspond directly to the numbers on the 
 * AI channels for the DAQ.
 *
 * \param _chanNumber The channel number
 * \param _chanType The channel type (using the ChannelType enum)
 *
 * \return true if the channel addition was sucessful
 *
 * /note If using differential channels, the pairs are offset by 8.  So differential channel 0 uses pins AI0 and AI8.
 */
//******************************************************/

bool PictoBoxDaqBoard::CreateAIChannel(int _chanNumber, ChannelType _chanType)
{
	//Is the channel number legal?
	if(_chanNumber<0 || _chanNumber>=MAX_AI_CHANNELS)
	{
		OutputDebugString(L"DaqBoard.CreateAIChannel: Invalid channel number\n");
		return false;
	}

	//Has the channel alreaady been created?
	for(int i=0; i<AIChannelList_.size(); i++)
	{
		if(_chanNumber == AIChannelList_[i].channelNum)
		{
			OutputDebugString(L"DaqBoard.CreateAIChannel: Channel number already in use.\n");
			return false;
		}
	}

	//Add the channel to the list
	AIChannel newChannel;

	newChannel.channelNum = _chanNumber;
	newChannel.chanType = _chanType;

	AIChannelList_.push_back(newChannel);

	return true;
}


//******************************************************/
// RemoveAIChannel
//******************************************************/
//! Removes an analog input channel
/*!
 * This function removes the analog input channel with the passed in channel
 * number from the channel list.  If the channel isn't currently in the 
 * list, the function makes no changes to the list, and returns false
 *
 * \param _chanNumber The channel number to remove
 *
 * \return true if the channel addition was sucessful
 */
//******************************************************/

bool PictoBoxDaqBoard::RemoveAIChannel(int _chanNumber)
{
	//std::vector<AIChannel>::iterator chanIter;
	QVector<AIChannel>::iterator chanIter;
	chanIter = AIChannelList_.begin();

	while(chanIter != AIChannelList_.end())
	{
		if(chanIter->channelNum == _chanNumber)
		{
			AIChannelList_.erase(chanIter);
			return true;
		}
		else
		{
			++chanIter;
		}
	}
	
	OutputDebugString(L"DaqBoard.RemoveAIChannel: Channel number not found.\n");

	return false;
}

//******************************************************/
// ClearAIChannelList
//******************************************************/
//! Removes every channel from the channel list.
/*!
 * All of the channels in the analog input list are removed.
 *
 * \return Always returns true
 */
//******************************************************/
bool PictoBoxDaqBoard::ClearAIChannelList()
{
	AIChannelList_.clear();

	return true;
}

//******************************************************/
// StartAI
//******************************************************/
//! Starts the DAQ running in analog input mode.
/*!
 * This function starts the AI running at the passed in frequency (in Hz)
 * The data is either collected continuously, or a specific number of
 * samples are collected.  If continuous is true, the number of samples is ignored.
 * 
 * Samples are collected from all of the channels currently in the AI channel list
 * A sample from each channel is collected at the passed in frequency.  Samples are
 * seperated by 20MHz/3 (using 3 as the delay divisor)
 *
 * \param _frequency The frequency in Hz at which the DAQ will run
 * \param _continuous Run the DAQ iun continuous mode (true), or collect a finite number of samples (false)
 * \param _numberOfSamples Number of samples to collect.  If _continuous is true, this parameter is ignored (and can be left out of the argument list)
 *
 * EXAMPLE:
 *   StartAI(10000,false,5)
 *
 *   This will start the DAQ running at 10kHz.  Every 0.1ms, the DAQ
 *   will collect data from all of the inputs in the AI channel list (seperated
 *   by 20MHz/3).  This will occur 5 times, and then the DAQ will stop running.
 *   The whole process will have taken 0.5 ms, and there will be 5 samples stored
 *   in each of the AI channels.
 */
//******************************************************/
bool PictoBoxDaqBoard::StartAI(unsigned int _frequency, bool _continuous, unsigned int _numberOfSamples)
{
	//confirm that there is at least one channel in the list
	if(AIChannelList_.size()<1)
		return false;

	//check that the frequency is reasonable
	if(_frequency == 0 || _frequency > 20000000)
	{
		OutputDebugString(L"DaqBoard.StartAI: Frequency out of range\n");
		return false;
	}

	if(!hDAQ_)
	{
		OutputDebugString(L"DaqBoard.StartAI: Failed to create DAQ1 file\n");
		return false;
	}
	
	//download the scaling coefficients from the EEPROM
	_getCalibrationValues(&scale_);

	_ResetAI();

	_ConfigureAI(_frequency, _numberOfSamples, _continuous);

	_StartAI();

	return true;
}

//******************************************************/
// CollectAIData
//******************************************************/
//! Dumps the AI data sitting in the DAQ's data FIFO into the AI channel objects
/*!
 * This function reads data from the DAQ, and converts it to a floating
 * point value which is the actual voltage.  The values are then stored 
 * in the channel structures.The number of samples in the data FIFO could range from 0 to 4096, 
 * depending on how long the DAQ has been running (and at what frequency).
 *
 * /param _numSamples the number of samples to read (this is per channel, so 
 *         if you want 42 samples from 16 channels, this values should be 42)
 *
 * \returns the actual number of values read (this could be less than numSamples, 
 *          depending on the amount of data in the fifo) If this value is equal to numSamples, 
 *          it's possible that there is still data in the FIFO.
 */
//******************************************************/
int PictoBoxDaqBoard::CollectAIData(unsigned int _numSamples)
{
	if(!hDAQ_)
	{
		OutputDebugString(L"DaqBoard.ReadAIData: DAQ1 file NULL.  D\n");
		return 0;
	}
	unsigned int numChannels = AIChannelList_.size();
	unsigned int chan, sample;

	ULONG *dataBuffer;
	dataBuffer = new ULONG[_numSamples*numChannels];

	ULONG fifoBytes;

	//dump the data fifo
	DeviceIoControl(hDAQ_,IOCTRL_DATA_FIFO_DUMP,NULL,0, dataBuffer, _numSamples*numChannels*sizeof(ULONG), &fifoBytes, NULL);

	//sort the data fifo into the channels

	//run through each channel
	for(chan=0; chan<numChannels; chan++)
	{
		//clear out the data currently stored for each channel
		AIChannelList_[chan].rawData.clear();
		AIChannelList_[chan].voltages.clear();


		//for each channel run through the data buffer, storing the raw values
		//and converting them to voltages
		for(sample=0; sample<_numSamples  && sample<fifoBytes/sizeof(ULONG)/numChannels; sample++)
		{
			AIChannelList_[chan].rawData.push_back(dataBuffer[sample*numChannels+chan]);
			AIChannelList_[chan].voltages.push_back(_aiPolynomialScaler(dataBuffer[sample*numChannels+chan],&scale_));
		}
	}

	delete dataBuffer;

	return sample;

}
//******************************************************/
// ReadAIChannel
//******************************************************/
//! Provides access to the data dumped into the channel objects
/*!
 * After calling CollectAIData, the user still needs access to the data
 * itself.  This function reutrns a pointer to the AI Channel structure 
 * of a given channel number.  The user can then read the data from the structure.
 *
 * \returns A pointer to an AIChannel structure.  Or null, if the channel number isn't found.
 */
//******************************************************/

PictoBoxDaqBoard::AIChannel* PictoBoxDaqBoard::GetAIChannel(int _chanNumber)
{
	//Find the channel in the list
	for(int i=0; i<AIChannelList_.size(); i++)
	{
		if(_chanNumber == AIChannelList_[i].channelNum)
		{
			return &AIChannelList_[i];
		}
	}
	return NULL;

}



//******************************************************/
// FlushAIData
//******************************************************/
//! Flushes the DAQ boards FIFO
/*!
 * This function flushes out all of the AI data currently in the
 * DAQ's FIFO data buffer.  This is useful if you want to make sure
 * that the data being read is "fresh".
 *
 * Note that calling this function could be expensive in terms of 
 * time (depending on the amount of data in the buffer).
 *
 * \returns the number of values read from the buffer
 */
//******************************************************/

int PictoBoxDaqBoard::FlushAIData()
{
	if(!hDAQ_)
	{
		OutputDebugString(L"DaqBoard.FlushAIData: DAQ1 file NULL.  D\n");
		return false;
	}

	ULONG bytesRead;
	ULONG data;
	ULONG FIFOEmpty;

	int numReads=0;

	DeviceIoControl(hDAQ_,IOCTRL_USE_BAR1,NULL,0,NULL,0, NULL, NULL);

	//board->AI_Status_1.readAI_FIFO_Empty_St()
	SetFilePointer(hDAQ_,REG_AI_STATUS_1,NULL,FILE_BEGIN);
	ReadFile(hDAQ_,&data, TWO_BYTES, &bytesRead,NULL);
	FIFOEmpty = data & 0x0001000;

	while (!FIFOEmpty)
    { 
        //value = board->AI_FIFO_Data.readRegister ();
		SetFilePointer(hDAQ_,REG_AI_FIFO_DATA,NULL,FILE_BEGIN);
		ReadFile(hDAQ_,&data, FOUR_BYTES, &bytesRead,NULL);

		//board->AI_Status_1.readAI_FIFO_Empty_St()
		SetFilePointer(hDAQ_,REG_AI_STATUS_1,NULL,FILE_BEGIN);
		ReadFile(hDAQ_,&data, TWO_BYTES, &bytesRead,NULL);
		FIFOEmpty = data & 0x0001000;

		numReads++;

		//If, for some reason, the fifo is being read at a slower rate than 
		//samples are being collected, then the FIFO will never be empty.
		//If this happens, break out of the loop
		if(numReads>4096)
			break;
    }
	return numReads;
}

//******************************************************/
// StopAI
//******************************************************/
//! Stops the AI
/*! This stops the DAQ from collecting analog input data.  The data collected remains 
 * in the FIFO, and can be recovered with CollectAIData.  Also note that calling
 * this function when the AI is not currently running has no effect.  
 *
 * \returns true if the DAQ was stopped
 */
//******************************************************/

bool PictoBoxDaqBoard::StopAI()
{
	if(!hDAQ_)
	{
		OutputDebugString(L"DaqBoard.StopAI: DAQ1 file NULL.  D\n");
		return false;
	}

	DWORD bytesWritten;
	ULONG data;

	DeviceIoControl(hDAQ_,IOCTRL_USE_BAR1,NULL,0,NULL,0, NULL, NULL);

	data = 1<<13;
	SetFilePointer(hDAQ_,REG_AI_COMMAND_1,NULL,FILE_BEGIN);
	WriteFile(hDAQ_,&data, TWO_BYTES, &bytesWritten,NULL);

	CloseHandle(hDAQ_);

	return true;
}

};  //end Picto namespace