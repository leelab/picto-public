#include "PictoBoxXPRewardController.h"
#include <windows.h>
#include <NIDAQmx.h>
#include "../../common/memleakdetect.h"

#define DAQmxErrChk(rc) { if (rc) { \
							char error[512]; \
							DAQmxGetErrorString(rc, error,512); \
							DAQmxStopTask(daqTaskHandle_[channel]); \
							DAQmxClearTask(daqTaskHandle_[channel]); \
							QString msg = QString("DAQ function error %1:").arg(rc); \
							msg.append(error); \
							Q_ASSERT_X(!rc, "PictoBoxXPEventCodeGenerator", msg.toLatin1());\
						 } }

// NOTE: I am hard coding the NIDAQ setup, since this code is only intended to run on PictoBox 
//		 where we have full hardware control.  If this is meant to run elsewhere, a more
//		 generic RewardController will need to be written
#define DEVICE_NAME "Dev1"
//#define PICTO_BOX_NIDAQ_5V_CHANNEL "Dev1/port0/line0"
#define PICTO_BOX_NIDAQ_5V_CHANNEL "Dev1/ao1"
//Note, currently, the reward enable line is on ao1, and the reward trigger is on a00.  If we increase
//the NUMREWARDLINES, channel 2 will try to overwrite ao1, which won't work.  In order to use more lines
//we are going to need to figure out how to use digital lines for our reward triggers.  This means using
//seperate clocks for each reward trigger line from somewhere in the Nidaq card.  We are theoretically 
//supposed to be able to configure the nidaq card so that digital lines can use clocks from other parts
//of the card, but I have not yet figured out how to do this correctly.
#define NUMREWARDLINES 1


namespace Picto
{

//! Sets up the reward controller
PictoBoxXPRewardController::PictoBoxXPRewardController()
: RewardController(NUMREWARDLINES)
{
	outputData[0] = 0.0;
	outputData[1] = 5.0;
	// Skip if device doesn't exist
	uInt32 tmp;
	int channel = 1;
	DAQmxErrChk(DAQmxGetDevSerialNum(DEVICE_NAME,&tmp))
	Q_ASSERT(tmp);
	hasDevice_ = tmp;
	if(!hasDevice_)
		return;

	float64 data[] = {5.0};
	int32 sampsPerChanWritten;
	for(int channel=1;channel<NUMREWARDLINES+1;channel++)
	{
		DAQmxErrChk(DAQmxCreateTask("RewardTask",(TaskHandle*)&daqTaskHandle_[channel]));
		DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_[channel],QString("Dev1/ao%1").arg(channel-1).toLatin1(),"",0.0,5.0,DAQmx_Val_Volts,NULL));
		//If the function fails on StartTask, don't trigger assertions because it might just be that another process
		//is using the pin we want to control.  If there's an error here, just skip the task.
		if(!DAQmxStartTask(daqTaskHandle_[channel]))
			DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_[channel],1,true,1,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
		DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
		DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
		daqTaskHandle_[channel] = 0;
	}
	channel = 1;

	//Once the reward trigger lines are high, raise the line being used for +5 to make the relay operational.
	//This setup prevents the relay closing during PictoBox power up when the trigger lines will be low.
	TaskHandle rewardEnTask;
	DAQmxErrChk(DAQmxCreateTask("RewardEnableTask",(TaskHandle*)&rewardEnTask));
	//DAQmxErrChk(DAQmxCreateDOChan(rewardEnTask,PICTO_BOX_NIDAQ_5V_CHANNEL,"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk(DAQmxCreateAOVoltageChan(rewardEnTask,QString("Dev1/ao1").toLatin1(),"",0.0,5.0,DAQmx_Val_Volts,NULL));
	if(!DAQmxStartTask(rewardEnTask))
	{
		//Set digital lines to +5
		//const uInt8 rewardEnData[] = {0xFF};
		//DAQmxErrChk(DAQmxWriteDigitalU8(rewardEnTask,1,1,1.0,DAQmx_Val_GroupByChannel,rewardEnData,&sampsPerChanWritten,NULL));
		DAQmxErrChk(DAQmxWriteAnalogF64(rewardEnTask,1,true,1,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
	}
	//Clear this setup task
	DAQmxErrChk(DAQmxStopTask(rewardEnTask));
	DAQmxErrChk(DAQmxClearTask(rewardEnTask));
}

PictoBoxXPRewardController::~PictoBoxXPRewardController()
{
	if(!hasDevice_)
		return;

	//Since the reward enable line is an analog line, it will go back to zero on its own as soon
	//as things reset.
}


void PictoBoxXPRewardController::startReward(unsigned int channel,int quantity)
{
	if(!hasDevice_)
		return;
	if(channel > NUMREWARDLINES+1 || channel < 1)
		return;

	DAQmxErrChk(DAQmxCreateTask(QString("RewardTask%1").arg(channel).toLatin1(),(TaskHandle*)&daqTaskHandle_[channel]));
	//DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_[channel],QString("Dev1/port0/line%1").arg(channel).toLatin1(),"",DAQmx_Val_ChanForAllLines));
	//DAQmxErrChk (DAQmxCfgSampClkTiming(daqTaskHandle_[channel],"",1000.0/float64(quantity),DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));
	//(remember, we're using active low logic)
	//int32 sampsPerChanWritten;
	//DAQmxErrChk(DAQmxWriteDigitalU8(daqTaskHandle_[channel],2,0,-1,DAQmx_Val_GroupByChannel,outputData,&sampsPerChanWritten,NULL));
	//DAQmxErrChk(DAQmxStartTask(daqTaskHandle_[channel]));
	
	DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_[channel],QString("Dev1/ao%1").arg(channel-1).toLatin1(),"",0.0,5.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(daqTaskHandle_[channel],"",1000.0/float64(quantity),DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));
	int32 sampsPerChanWritten;
	//(remember, we're using active low logic)
	DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_[channel],2,0,-1,DAQmx_Val_GroupByChannel,outputData,&sampsPerChanWritten,NULL));
	if(!DAQmxStartTask(daqTaskHandle_[channel]))
	{
		//Since this code creates a task that raises the voltage for quantity ms and then lowers it for quantity ms, as it is it 
		//doesn't support inter reward intervals of less than quantity.  Since we couldn't find a better way to do this, we just
		//set our own timer and stop/clear the task anytime that it still exists and more than quantity ms have passed.
		stopwatch_[channel].startWatch();
		latestOnTime_[channel] = quantity;
	}
	else
	{
		DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
		DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
		daqTaskHandle_[channel] = 0;
	}
}

bool PictoBoxXPRewardController::rewardWasSupplied(unsigned int channel)
{
	if(channel > NUMREWARDLINES+1 || channel < 1)
		return true;
	if(!daqTaskHandle_[channel])
		return true;
	//The "+.5" below is just to avoid possible weird racing effects where the Nidaq doesn't turn the 
	//signal off.  This hasn't ever actually happened, I'm just doing it to be safe.
	if(stopwatch_[channel].elapsedMs() <= latestOnTime_[channel]+.5)
		return false;
	DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
	daqTaskHandle_[channel] = 0;
	return true;
}
void PictoBoxXPRewardController::startFlush(unsigned int channel)
{
	if(!hasDevice_)
		return;
	if(channel > NUMREWARDLINES+1 || channel < 1)
		return;
	//We don't really care about the channel here because there's currently 
	//only one channel supported in picto for the legacy system.

	DAQmxErrChk(DAQmxCreateTask(QString("RewardTask%1").arg(channel).toLatin1(),(TaskHandle*)&daqTaskHandle_[channel]));	
	DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_[channel],QString("Dev1/ao%1").arg(channel-1).toLatin1(),"",0.0,5.0,DAQmx_Val_Volts,NULL));
	int32 sampsPerChanWritten;
	//(remember, we're using active low logic)
	DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_[channel],1,true,1,DAQmx_Val_GroupByChannel,outputData,&sampsPerChanWritten,NULL));
	if(!DAQmxStartTask(daqTaskHandle_[channel]))
	{
		stopwatch_[channel].startWatch();
		latestOnTime_[channel] = INT_MAX;
	}
	else
	{
		DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
		DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
		daqTaskHandle_[channel] = 0;
	}

}
void PictoBoxXPRewardController::stopFlush(unsigned int channel)
{
	if(!hasDevice_)
		return;
	if(channel > NUMREWARDLINES+1 || channel < 1)
		return;
	if(!daqTaskHandle_[channel])
		return;
	//Stop the flush task
	DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
	//Bring the reward enable pin back up
	float64 data[] = {5.0};
	int32 sampsPerChanWritten;
	DAQmxErrChk(DAQmxCreateTask("RewardTask",(TaskHandle*)&daqTaskHandle_[channel]));
	DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_[channel],QString("Dev1/ao%1").arg(channel-1).toLatin1(),"",0.0,5.0,DAQmx_Val_Volts,NULL));
	if(!DAQmxStartTask(daqTaskHandle_[channel]))
	{
		DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_[channel],1,true,1,DAQmx_Val_GroupByChannel,data,&sampsPerChanWritten,NULL));
	}
	DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
	daqTaskHandle_[channel] = 0;
}


}; //namespace Picto