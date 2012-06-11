#include "PictoBoxXPRewardController.h"
#include <windows.h>
#include <NIDAQmx.h>
#include "../../common/memleakdetect.h"

#define DAQmxErrChk(rc) { if (rc) { \
							char error[512]; \
							DAQmxGetErrorString(rc, error,512); \
							DAQmxStopTask(daqTaskHandle_[channel]); \
							DAQmxClearTask(daqTaskHandle_[channel]); \
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
	outputData[0] = 1.0;
	outputData[1] = 0.0;
	Q_ASSERT(channelCount <= 4);	//PictoBox only has 4 reward channels...

	int channel = 0;
	DAQmxErrChk(DAQmxCreateTask("RewardTask",(TaskHandle*)&daqTaskHandle_[channel]));
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_[channel],PICTO_BOX_NIDAQ_REWARD_CHANNELS,"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_[channel]));

	//Leave the solenoids closed by setting all lines to 1
	int32 sampsPerChanWritten;
	unsigned char data[8] = {1,1,1,1,1,1,1,1};
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_[channel],1,1,1.0,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
	DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
	taskExists_[0] = false;
	taskExists_[1] = false;
	taskExists_[2] = false;
	taskExists_[3] = false;
}

PictoBoxXPRewardController::~PictoBoxXPRewardController()
{
	for(int channel=0;channel<4;channel++)
	{
		if(taskExists_)
			DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
	}
}





void PictoBoxXPRewardController::startReward(unsigned int channel,int quantity)
{
	if(channel > 4 || channel < 1)
		return;

	DAQmxErrChk(DAQmxCreateTask(QString("RewardTask%1").arg(channel).toAscii(),(TaskHandle*)&daqTaskHandle_[channel]));
	taskExists_[channel] = true;
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_[channel],QString("Dev1/port0/line%1").arg(channel).toAscii(),"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk (DAQmxCfgSampClkTiming(daqTaskHandle_[channel],"",1000.0/float64(quantity),DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));
	//(remember, we're using active low logic)
	int32 sampsPerChanWritten;
	DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_[channel],2,0,-1,DAQmx_Val_GroupByChannel,outputData,&sampsPerChanWritten,NULL));
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_[channel]));
	//Since this code creates a task that raises the voltage for quantity ms and then lowers it for quantity ms, as it is it 
	//doesn't support inter reward intervals of less than quantity.  Since we couldn't find a better way to do this, we just
	//set our own timer and stop/clear the task anytime that it still exists and more than quantity ms have passed.
	stopwatch_[channel].startWatch();
	latestOnTime_[channel] = quantity;
}

void PictoBoxXPRewardController::stopReward(unsigned int channel)
{
	if(!taskExists_[channel])
		return;
	DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
	//We may need to manually set the pin back to TTL low... see if we do in testing.
	taskExists_[channel] = false;
}

bool PictoBoxXPRewardController::rewardWasSupplied(unsigned int channel)
{
	if(!taskExists_[channel])
		return true;
	//The "+.5" below is just to avoid possible weird racing effects where the Nidaq doesn't turn the 
	//signal off.  This hasn't ever actually happened, I'm just doing it to be safe.
	if(stopwatch_[channel].elapsedMs() <= latestOnTime_[channel]+.5)
		return false;
	DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
	taskExists_[channel] = false;
	return true;
}

void PictoBoxXPRewardController::flush(unsigned int channel, bool flush)
{
	if(channel > 4 || channel < 1)
		return;
	if(taskExists_[channel])
	{
		DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
		taskExists_[channel] = false;
	}
	DAQmxErrChk(DAQmxCreateTask(QString("RewardTask%1").arg(channel).toAscii(),(TaskHandle*)&daqTaskHandle_[channel]));
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_[channel],QString("Dev1/port0/line%1").arg(channel).toAscii(),"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk(DAQmxStartTask(daqTaskHandle_[channel]));
	int32 sampsPerChanWritten;
	const uInt8 data[] = {flush?1:0};
	DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_[channel],1,true,1,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
	DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
}


}; //namespace Picto