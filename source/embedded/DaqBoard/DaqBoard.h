//Matthew Gay
//August 5, 2009
//Lee Lab - Yale School of Medicine

#include <windows.h>
#include <vector>

#include "DAQ_Driver.h"
#include "registerMap.h"

#ifndef DAQBOARD_H
#define DAQBOARD_H

//-------------------------------------------
//! General definitions
//-------------------------------------------
#define FOUR_BYTES 4
#define TWO_BYTES 2
#define ONE_BYTE 1

#define MAX_NUM_REWARD_CONTROLLERS 4
#define MAX_AI_CHANNELS 16

//-------------------------------------------
// Class definition
//-------------------------------------------

//! Used to interact with the daq board
/*!
 * The DaqBoard object allows the programmer to interact with a National Instruments
 * M-Series data acquisition board.  The object currently allows the control of 
 * reward controllers through the digital outputs, and the reading of analog inputs 
 * through all channels.
 *
 * CODE LIMITATIONS
 * The code was tested with a PCI-6221 board, but it should work with other PCI based 
 * DAQ boards (although it hasn't been tested this way).  
 *
 * It should also be noted that you really shouldn't attempt to create more than one 
 * DaqBoard object.  It is possible to do this (and it was tested), but you run the 
 * risk of putting the board into an unexpected state.
 */



class DaqBoard
{
public:
	//General Daq Board
	//-----------------
	DaqBoard();
	~DaqBoard();

	//Reward Controller 
	bool initRewardController(int _numControllers, unsigned int _rewardDurations[]);
	bool setRewardDuration(int _controller, unsigned int _duration);
	bool giveReward(int _controller);
	bool flushReward(int _controller, bool _flush);

	//Analog Input Functions
	//----------------------
	//! Types of analog inputs
	typedef enum
	{
		kChanTypeDifferential=1,	//!< Differential input
		kChanTypeNRSE,				//!< Non-referenced single-ended input
		kChanTypeRSE				//!< Referenced single-ended input
	}ChannelType;

	//! Analog Input structure
	typedef struct aiChannel_st
	{
		ULONG channelNum;				//!< Channel number (corresponds direcly to the AI pins on the DAQ)
		ChannelType chanType;			//!< Type of channle (differential, NRSE, RSE)
		std::vector<ULONG> rawData;		//!< The raw data collected by the channel.  These values are updated when CollectAIData is called
		std::vector<float> voltages;	//!< The scaled voltages collected by the channel.  These values are updated when CollectAIData is called
		
	}AIChannel;


	bool CreateAIChannel(int _chanNumber, ChannelType _chanType);
	bool RemoveAIChannel(int _chanNumber);
	bool ClearAIChannelList();
	bool StartAI(unsigned int _frequency, bool _continuous, unsigned int _numberOfSamples=0);
	int CollectAIData(unsigned int _numSamples);
	AIChannel* GetAIChannel(int chanNumber);
	int FlushAIData();
	bool StopAI();

private:
	//General Daq Board
	//-----------------
	HANDLE hDAQ_;

	//Reward Controller
	//-----------------
	unsigned int rewardDurations_[MAX_NUM_REWARD_CONTROLLERS];
	ULONG rewardBitmasks_[MAX_NUM_REWARD_CONTROLLERS];
	bool rewardControllersInitialized_;
	int activeRewardControllers_;

	//Analog Input
	//------------

	std::vector<AIChannel> AIChannelList_;

	//! Scaling coefficients used to convert digital values to analog voltages.
	typedef struct
	{
		ULONG order;
		float c[4]; 
	}scalingCoeff;

	scalingCoeff scale_;


	//Analog input private Inline functions
	DWORD soft_AI_MODE_1_;
	DWORD soft_AI_MODE_2_;
	DWORD soft_AI_START_STOP_SELECT_;

	void _ResetAI();
	void _ConfigureAI(unsigned int frequency, unsigned int numberOfSamples, bool continuous);
	void _StartAI();

	void _getCalibrationValues(scalingCoeff *scale);
	float _aiPolynomialScaler(int raw, const scalingCoeff *scale);
	void _readEeprom(unsigned int offset, UCHAR *pBuffer, unsigned int bufferLength);




};

#include "DaqBoard_p.h"

#endif