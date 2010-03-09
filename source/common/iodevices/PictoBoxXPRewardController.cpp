#include "PictoBoxXPRewardController.h"
#include <windows.h>
#include <NIDAQmx.h>


#define DAQmxErrChk(rc) { if (rc) { \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							Q_ASSERT_X(!rc, "PictoBoxXPEventCodeGenerator", "DAQ function failure");\
						 } }

// NOTE: I am hard coding the NIDAQ setup, since this code is only intended to run on PictoBox 
//		 where we have full hardware control.  If this is meant to run elsewhere, a more
//		 generic RewardController will need to be written
#define PICTO_BOX_NIDAQ_REWARD_CHANNELS "Dev1/port0/line0:3"

namespace Picto
{

//! Sets up the reward controller
PictoBoxXPRewardController::PictoBoxXPRewardController(unsigned int channelCount)
: RewardController(channelCount)
{
	Q_ASSERT(channelCount <= 4);	//PictoBox only has 4 reward channels...

	//initialize the rewrad durations to 250 ms (this is arbitrary
	for(int i=0; i<4; i++)
		rewardDurations_.append(250);
	//initialize the reset time to  0 ms
	for(int i=0; i<4; i++)
		rewardResetTimes_.append(250);

	DAQmxErrChk(DAQmxCreateTask("RewardTask",&daqTaskHandle_));
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_,PICTO_BOX_NIDAQ_REWARD_CHANNELS,"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_));

	//Leave the solenoids closed by setting all lines to 1
	int32 sampsPerChanWritten;
	unsigned char data[8] = {1,1,1,1,1,1,1,1};
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
}

PictoBoxXPRewardController::~PictoBoxXPRewardController()
{
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_));
}


//! With PictoBox, we can't easily control reward volume, so this function fails
bool PictoBoxXPRewardController::setRewardVolume(unsigned int channel, float volume)
{
	Q_UNUSED(channel);
	Q_UNUSED(volume);
	return false;
}

bool PictoBoxXPRewardController::setRewardDurationMs(unsigned int channel, unsigned int duration)
{
	if(channel > 4 || channel < 1)
		return false;
	rewardDurations_[channel-1] = duration;
	return true;
}

int PictoBoxXPRewardController::getRewardDurationMs(unsigned int channel)
{
	if(channel > 4 || channel < 1)
		return -1;
	else
		return rewardDurations_[channel-1];

}

bool PictoBoxXPRewardController::setRewardResetTimeMs(unsigned int channel, unsigned int time)
{
	if(channel > 4 || channel < 1)
		return false;
	rewardResetTimes_[channel-1] = time;
	return true;
}


void PictoBoxXPRewardController::giveReward(unsigned int channel)
{
	if(channel > 4 || channel < 1)
		return;

	//Since this code can only run in Win32 devices anyway (PictoBox), we might
	//as well use the PerformanceCounter for maximum timing resolution
	LARGE_INTEGER ticksPerSec;
	LARGE_INTEGER tick, tock;
	double elapsedTime;
	QueryPerformanceFrequency(&ticksPerSec);

	//turn on the reward controller (remember, we're using active low logic)
	int32 sampsPerChanWritten;
	unsigned char data[4] = {1,1,1,1};
	data[channel-1] = 0;
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));

	QueryPerformanceCounter(&tick);
	do
	{
		QueryPerformanceCounter(&tock);
		elapsedTime = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerSec.LowPart);
	}
	while(elapsedTime * 1000.0 < rewardDurations_[channel-1]);

	data[channel-1] = 1;
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));

	//wait for the reset time
	if(rewardResetTimes_[channel-1] <= 0)
		return;

	QueryPerformanceCounter(&tick);
	do
	{
		QueryPerformanceCounter(&tock);
		elapsedTime = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerSec.LowPart);
	}
	while(elapsedTime * 1000.0 < rewardResetTimes_[channel-1]);



}

void PictoBoxXPRewardController::flush(unsigned int channel, bool flush)
{
	if(channel > 4 || channel < 1)
		return;

	if(flush)
	{
		//turn on the reward controller (remember, we're using active low logic)
		int32 sampsPerChanWritten;
		unsigned char data[4] = {1,1,1,1};
		data[channel-1] = 0;
		DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
	}
	else
	{
		//turn off the reward controller (remember, we're using active low logic)
		int32 sampsPerChanWritten;
		unsigned char data[4] = {1,1,1,1};
		DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
	}

}


}; //namespace Picto