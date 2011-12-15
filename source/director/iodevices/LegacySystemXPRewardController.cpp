#include "LegacySystemXPRewardController.h"
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
							Q_ASSERT_X(!rc, "LegacySystemXPEventCodeGenerator", msg.toAscii());\
						 } }

// NOTE: I am hard coding the NIDAQ setup, since this code is only intended to run our LegacySystem 
//		 If this is meant to run elsewhere, a more generic RewardController will need to be written
#define DEVICE_NAME "Dev1"
#define PICTO_BOX_NIDAQ_REWARD_CHANNELS "Dev1/port2/line7"

namespace Picto
{

//! Sets up the reward controller
LegacySystemXPRewardController::LegacySystemXPRewardController(unsigned int channelCount)
: RewardController(channelCount)
{
	Q_ASSERT(channelCount <= 1);	//LegacySystem only has 1 reward channel...
	// Skip if device doesn't exist
	uInt32 tmp;
	DAQmxErrChk(DAQmxGetDevSerialNum(DEVICE_NAME,&tmp))
	Q_ASSERT(tmp);
	hasDevice_ = tmp;
	if(!hasDevice_)
		return;

	DAQmxErrChk(DAQmxCreateTask("RewardTask",(TaskHandle*)&daqTaskHandle_));
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_,PICTO_BOX_NIDAQ_REWARD_CHANNELS,"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_));

	//Leave the solenoid closed by setting lines to 0
	int32 sampsPerChanWritten;
	unsigned char data[] = {0};
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
}

LegacySystemXPRewardController::~LegacySystemXPRewardController()
{
	if(!hasDevice_)
		return;
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_));
}





void LegacySystemXPRewardController::doReward(unsigned int,int quantity)
{
	if(!hasDevice_)
		return;
	//We don't really care about the channel here because there's currenlty 
	//only one channel supported in picto for the legacy system.
	//if(channel > 1 || channel < 1)
	//	return;

	//Since this code can only run in Win32 devices anyway (LegacySystem), we might
	//as well use the PerformanceCounter for maximum timing resolution
	LARGE_INTEGER ticksPerSec;
	LARGE_INTEGER tick, tock;
	double elapsedTime;
	QueryPerformanceFrequency(&ticksPerSec);

	//turn on the reward controller (remember, we're using active high logic)
	int32 sampsPerChanWritten;
	unsigned char data[] = {1};
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));

	QueryPerformanceCounter(&tick);
	do
	{
		QueryPerformanceCounter(&tock);
		elapsedTime = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerSec.LowPart);
	}
	while(elapsedTime * 1000.0 < quantity);

	data[0] = 0;
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));

	//wait for the reset time
	if(rewardResetTimes_[0] <= 0)
		return;

	QueryPerformanceCounter(&tick);
	do
	{
		QueryPerformanceCounter(&tock);
		elapsedTime = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerSec.LowPart);
	}
	while(elapsedTime * 1000.0 < rewardResetTimes_[0]);



}

void LegacySystemXPRewardController::flush(unsigned int, bool flush)
{
	if(!hasDevice_)
		return;
	//We don't really care about the channel here because there's currenlty 
	//only one channel supported in picto for the legacy system.
	//if(channel > 1 || channel < 1)
	//	return;

	if(flush)
	{
		//turn on the reward controller (remember, we're using active high logic)
		int32 sampsPerChanWritten;
		unsigned char data[] = {1};
		DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
	}
	else
	{
		//turn off the reward controller (remember, we're using active low logic)
		int32 sampsPerChanWritten;
		unsigned char data[] = {0};
		DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_,1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
	}

}


}; //namespace Picto