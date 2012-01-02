#include "PictoBoxXPRewardController.h"
#include <windows.h>
#include <NIDAQmx.h>
#include "../../common/memleakdetect.h"

#define DAQmxErrChk(rc) { if (rc) { \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							char error[512]; \
							DAQmxGetErrorString(rc, error,512); \
							QString msg = "DAQ function error:"; \
							msg.append(error); \
							Q_ASSERT_X(!rc, "PictoBoxXPEventCodeGenerator", msg.toAscii());\
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


	DAQmxErrChk(DAQmxCreateTask("RewardTask",(TaskHandle*)&daqTaskHandle_));
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





void PictoBoxXPRewardController::doReward(unsigned int channel,int quantity, int minRewardPeriod)
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
	while(elapsedTime * 1000.0 < quantity);

	data[channel-1] = 1;
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));

	//wait for the reset time
	while(elapsedTime * 1000.0 < minRewardPeriod)
	{
		QueryPerformanceCounter(&tock);
		elapsedTime = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerSec.LowPart);
	}



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