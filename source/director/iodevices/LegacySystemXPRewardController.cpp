#include "LegacySystemXPRewardController.h"
#include <windows.h>
#include <NIDAQmx.h>
#include "../../common/memleakdetect.h"

#define DAQmxErrChk(rc) { if (rc) { \
							char error[512]; \
							DAQmxGetErrorString(rc, error,512); \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							QString msg = "DAQ function error:"; \
							msg.append(error); \
							Q_ASSERT_X(!rc, "LegacySystemXPEventCodeGenerator", msg.toAscii());\
						 } }

// NOTE: I am hard coding the NIDAQ setup, since this code is only intended to run our LegacySystem 
//		 If this is meant to run elsewhere, a more generic RewardController will need to be written
#define DEVICE_NAME "Dev2"
#define PICTO_BOX_NIDAQ_REWARD_CHANNELS "Dev2/ao1"

namespace Picto
{

//! Sets up the reward controller
LegacySystemXPRewardController::LegacySystemXPRewardController(unsigned int channelCount)
: RewardController(channelCount)
{
	outputData[0] = 5.0;
	outputData[1] = 0.0;
	Q_ASSERT(channelCount <= 1);	//LegacySystem only has 1 reward channel...
	// Skip if device doesn't exist
	uInt32 tmp;
	DAQmxErrChk(DAQmxGetDevSerialNum(DEVICE_NAME,&tmp))
	Q_ASSERT(tmp);
	hasDevice_ = tmp;
	if(!hasDevice_)
		return;

	DAQmxErrChk(DAQmxCreateTask("RewardTask",(TaskHandle*)&daqTaskHandle_));
	DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_,"Dev2/ao0","",0.0,5.0,DAQmx_Val_Volts,NULL));

	//Leave the solenoid closed by setting lines to 0
	int32 sampsPerChanWritten;
	float64 data[] = {0};
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_));
	DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_,1,true,1,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
	DAQmxErrChk(DAQmxStopTask(daqTaskHandle_));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_));
	taskExists_ = false;
}

LegacySystemXPRewardController::~LegacySystemXPRewardController()
{
	if(!hasDevice_)
		return;
	if(taskExists_)
	{
		DAQmxErrChk(DAQmxClearTask(daqTaskHandle_));
	}
}





void LegacySystemXPRewardController::startReward(unsigned int,int quantity)
{
	if(!hasDevice_)
		return;
	//We don't really care about the channel here because there's currenlty 
	//only one channel supported in picto for the legacy system.
	//if(channel > 1 || channel < 1)
	//	return;

	////turn on the reward controller (remember, we're using active high logic)
	DAQmxErrChk(DAQmxCreateTask("RewardTask",(TaskHandle*)&daqTaskHandle_));
	taskExists_ = true;
	DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_,"Dev2/ao0","",0.0,5.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(daqTaskHandle_,"",1000.0/float64(quantity),DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));
	int32 sampsPerChanWritten;
	DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_,2,0,-1,DAQmx_Val_GroupByChannel,outputData,&sampsPerChanWritten,NULL));
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_));
	//Since this code creates a task that raises the voltage for quantity ms and then lowers it for quantity ms, as it is it 
	//doesn't support inter reward intervals of less than quantity.  Since we couldn't find a better way to do this, we just
	//set our own timer and stop/clear the task anytime that it still exists and more than quantity ms have passed.
	stopwatch_.startWatch();
	latestOnTime_ = quantity;
}

bool LegacySystemXPRewardController::rewardWasSupplied(unsigned int)
{
	if(!taskExists_)
		return true;
	//The "+.5" below is just to avoid possible weird racing effects where the Nidaq doesn't turn the 
	//signal off.  This hasn't ever actually happened, I'm just doing it to be safe.
	if(stopwatch_.elapsedMs() <= latestOnTime_+.5)
		return false;
	DAQmxErrChk(DAQmxStopTask(daqTaskHandle_));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_));
	taskExists_ = false;
	return true;
}
void LegacySystemXPRewardController::startFlush(unsigned int channel)
{
	if(!hasDevice_)
		return;
	//We don't really care about the channel here because there's currenlty 
	//only one channel supported in picto for the legacy system.

	////turn on the reward controller (remember, we're using active high logic)
	DAQmxErrChk(DAQmxCreateTask("RewardTask",(TaskHandle*)&daqTaskHandle_));
	taskExists_ = true;
	DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_,"Dev2/ao0","",0.0,5.0,DAQmx_Val_Volts,NULL));
	int32 sampsPerChanWritten;
	DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_,1,true,1,DAQmx_Val_GroupByChannel,outputData,&sampsPerChanWritten,NULL));
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_));
	stopwatch_.startWatch();
	latestOnTime_ = INT_MAX;
}
void LegacySystemXPRewardController::stopFlush(unsigned int channel)
{
	//Stop flush task
	DAQmxErrChk(DAQmxStopTask(daqTaskHandle_));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_));
	//Bring reward pin back to zero
	DAQmxErrChk(DAQmxCreateTask("RewardTask",(TaskHandle*)&daqTaskHandle_));
	DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_,"Dev2/ao0","",0.0,5.0,DAQmx_Val_Volts,NULL));

	//Leave the solenoid closed by setting lines to 0
	int32 sampsPerChanWritten;
	float64 data[] = {0};
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_));
	DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_,1,true,1,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
	DAQmxErrChk(DAQmxStopTask(daqTaskHandle_));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_));
	taskExists_ = false;
}


}; //namespace Picto