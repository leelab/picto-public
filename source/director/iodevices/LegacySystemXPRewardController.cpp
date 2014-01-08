#include "LegacySystemXPRewardController.h"
#include <windows.h>
#include <NIDAQmx.h>
#include "../../common/memleakdetect.h"

/*! \brief A macro used to check for NiDaq errors and trigger an assertion if they occur.*/
#define DAQmxErrChk(rc) { if (rc) { \
							char error[512]; \
							DAQmxGetErrorString(rc, error,512); \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							QString msg = "DAQ function error:"; \
							msg.append(error); \
							Q_ASSERT_X(!rc, "LegacySystemXPEventCodeGenerator", msg.toLatin1());\
						 } }

// NOTE: I am hard coding the NIDAQ setup, since this code is only intended to run our LegacySystem 
//		 If this is meant to run elsewhere, a more generic RewardController will need to be written
#define DEVICE_NAME "Dev2"
#define PICTO_BOX_NIDAQ_REWARD_CHANNELS "Dev2/ao1"	//THIS SEEMS TO BE WRONG! Ao0 is used.

namespace Picto
{

/*! \brief Constructs a LegacySystemXPRewardController object.
 *	\details Initializes the reward voltage line to low.  In the legacy system, high voltage opens the reward solenoid.
 */
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

/*! \brief Implements RewardController::startReward() to output a correctly timed reward signal on the output line.
 *	\details As far as timing goes, the NiDaq card only supports periodic timing signals.  We have not found a way
 *	to define a simple high-low pulse.  For this reason, we set up a clock pulse that ticks once for each "quantity" milliseconds.
 *	We then attach this clock to a task that sends the output signal high, low, high, low, switching once per clock pulse.
 *	The reward signal then goes high for exactly as long as we want and then low.  Since the reward system is updated once
 *	per frame, we are assured that rewardWasSupplied() will be called once per frame.  rewardWasSupplied() stops the
 *	task once the required reward quantity has been supplied, so the second high pulse never happens.
 *
 *	\note The downside to this is that we can't provide rewards of under 16ms, in practice, rewards this small are
 *	very non-linear and not a good idea anyway.
*/
void LegacySystemXPRewardController::startReward(unsigned int,int quantity)
{
	if(!hasDevice_)
		return;
	//We don't really care about the channel here because there's currenlty 
	//only one channel supported in picto for the legacy system.
	//if(channel > 1 || channel < 1)
	//	return;

	//turn on the reward controller (remember, we're using active high logic)
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

/*! \brief Implements RewardController::rewardWasSupplied() to return true when the latest reward to have been started using startReward() is complete.
 *	\details In the case of the LegacySystemXPRewardController, we piggyback onto this function and
 *	actually stop the reward task once the reward has been completely supplied.  See startReward() for
 *	more details.
 */
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

/*! \brief Implements RewardController::startFlush() to raise the reward line voltage.
 *	\details Unlike in startReward() flushes are not stopped automatically.  The stopFlush() function is used
 *	to lower the reward line.
*/
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

/*! \brief Implements RewardController::stopFlush() to lower the reward line voltage.
 *	\sa startFlush()
*/
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