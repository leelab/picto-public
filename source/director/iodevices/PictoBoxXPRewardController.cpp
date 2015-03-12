#include "PictoBoxXPRewardController.h"
#include <windows.h>
#include <NIDAQmx.h>
#include "../../common/memleakdetect.h"

/*! \brief A macro used to check for NiDaq errors and trigger an assertion if they occur.*/
#define DAQmxErrChk(rc) { if (rc) { \
							char error[512]; \
							DAQmxGetErrorString(rc, error,512); \
							DAQmxStopTask(daqTaskHandleDig_[channel]); \
							DAQmxClearTask(daqTaskHandleDig_[channel]); \
							QString msg = QString("DAQ function error %1:").arg(rc); \
							msg.append(error); \
							Q_ASSERT_X(!rc, "PictoBoxXPEventCodeGenerator", msg.toLatin1());\
						 } }

/*! A macro used to check the output of DAQmx calls for the Enable line and trigger an
* appropriate assertion on failure
*/
#define DAQmxEnErrChk(rc) { if (rc) { \
							char error[512]; \
							DAQmxGetErrorString(rc, error,512); \
							DAQmxStopTask(rewardEnTask); \
							DAQmxClearTask(rewardEnTask); \
							QString msg = QString("DAQ function error %1:").arg(rc); \
							msg.append(error); \
							Q_ASSERT_X(!rc, "PictoBoxXPEventCodeGenerator", msg.toLatin1());\
						 						 } }

// NOTE: The Pictobox only has one NiDaq card which is always Dev1
#define DEVICE_NAME "Dev1"

/*! The Nidaq line used to act as the High-Enable signal for the Relay board.
*/
#define PICTO_BOX_NIDAQ_RELAY_ENABLE_CHAN "Dev1/port2/line1"

/*! \brief The number of reward lines on the Pictobox.
*	\details The reward enable line is on PFI 9, and the reward triggers are on PFI 12,
*	PFI 13, PFI 14, and PFI 15.
*/
#define NUMREWARDLINES 4


namespace Picto
{

/*! \brief Constructs a PictoBoxXPRewardController object.
 *	\details Creates a "RewardTask" and "RewardEnableTask" to set both the reward line and the reward enable
 *	line to high voltages when Picto starts.  Since the Reward relay card uses active low logic, we need to
 *	start the reward line high.  Incidentally, this is why we cant simply attach the relay card's enable
 *	line to 5 volts.  Since all output lines start low when the Pictobox turns on, but the 5V line starts high
 *	if we connected the relay card's enable line to 5V, the reward solenoid would always be open after the 
 *	computer restarted until Picto started.
 */
PictoBoxXPRewardController::PictoBoxXPRewardController()
: RewardController(NUMREWARDLINES)
{
	// Skip if device doesn't exist
	uInt32 tmp;
	int channel = 1;
	DAQmxErrChk(DAQmxGetDevSerialNum(DEVICE_NAME,&tmp))
	Q_ASSERT(tmp);
	hasDevice_ = tmp;
	if(!hasDevice_)
		return;

	uInt8 data[] = { 1 };
	int32 sampsPerChanWritten;

	for (int channel = 1; channel<NUMREWARDLINES + 1; channel++)
	{
		DAQmxErrChk(DAQmxCreateTask("RewardTaskDig", (TaskHandle*)&daqTaskHandleDig_[channel]));
		DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandleDig_[channel], QString("Dev1/port2/line%1").arg(channel + 3).toLatin1(), "", DAQmx_Val_ChanPerLine));
		//If the function fails on StartTask, don't trigger assertions because it might just be that another process
		//is using the pin we want to control.  If there's an error here, just skip the task.
		if (!DAQmxStartTask(daqTaskHandleDig_[channel]))
			DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandleDig_[channel], 1, true, 1, DAQmx_Val_GroupByChannel, data, &sampsPerChanWritten, NULL));
		DAQmxErrChk(DAQmxStopTask(daqTaskHandleDig_[channel]));
		DAQmxErrChk(DAQmxClearTask(daqTaskHandleDig_[channel]));
		daqTaskHandleDig_[channel] = 0;
	}

	//Once the reward trigger lines are high, raise the line being used for +5 to make the relay operational.
	//This setup prevents the relay closing during PictoBox power up when the trigger lines will be low.
	TaskHandle rewardEnTask;
	DAQmxEnErrChk(DAQmxCreateTask("RewardEnableTask", (TaskHandle*)&rewardEnTask));
	DAQmxEnErrChk(DAQmxCreateDOChan(rewardEnTask, QString(PICTO_BOX_NIDAQ_RELAY_ENABLE_CHAN).toLatin1(), "", DAQmx_Val_ChanPerLine));
	if(!DAQmxStartTask(rewardEnTask))
	{
		//Set digital lines High
		DAQmxEnErrChk(DAQmxWriteDigitalLines(rewardEnTask, 1, true, 1, DAQmx_Val_GroupByChannel, data, &sampsPerChanWritten, NULL));
	}
	//Clear this setup task

	if (rewardEnTask)
	{
		DAQmxEnErrChk(DAQmxStopTask(rewardEnTask));
		DAQmxEnErrChk(DAQmxClearTask(rewardEnTask));
	}
}

/*! This desructor doesn't need to bring the reward enable line back to zero.  Since it's an enable line,
 * it will return to zero automatically as soon as things reset
 */
PictoBoxXPRewardController::~PictoBoxXPRewardController()
{
	if(!hasDevice_)
		return;

	// Lines will go back to zero on their own as soon as things reset.
}

/*! \brief Implements RewardController::startReward() to output a correctly timed reward signal on the output line.
 *  \details Previous limitations due to the old implementation have been lifted, and now rewards can be
 *  given with any amount of pause between.
 *	\note The downside to this is that we can't provide rewards of under 16ms, in practice, rewards this small are
 *	very non-linear and not a good idea anyway.
 *  \sa rewardWasSupplied()
*/
void PictoBoxXPRewardController::startReward(unsigned int channel,int quantity)
{
	if(!hasDevice_)
		return;
	if(channel > NUMREWARDLINES+1 || channel < 1)
		return;

	//TEST CODE for Digital Channels
	DAQmxErrChk(DAQmxCreateTask(QString("RewardTaskDig%1").arg(channel).toLatin1(), (TaskHandle*)&daqTaskHandleDig_[channel]));
	DAQmxErrChk(DAQmxCreateCOPulseChanTime(daqTaskHandleDig_[channel], QString("Dev1/ctr%1").arg(channel - 1).toLatin1(), "", DAQmx_Val_Seconds, DAQmx_Val_High, 0.00, float64(quantity) / 1000.0, 0.01));
	if (!DAQmxStartTask(daqTaskHandleDig_[channel]))
	{
		stopwatch_[channel].startWatch();
		latestOnTime_[channel] = quantity;
	}
	else
	{
		DAQmxErrChk(DAQmxStopTask(daqTaskHandleDig_[channel]));
		DAQmxErrChk(DAQmxClearTask(daqTaskHandleDig_[channel]));
		daqTaskHandleDig_[channel] = 0;
	}
}

/*! \brief Implements RewardController::rewardWasSupplied() to return true when the latest reward to have been started using startReward() is complete.
 *	\details In the case of the PictoBoxXPRewardController, we piggyback onto this function and
 *	actually stop the reward task once the reward has been completely supplied.  See startReward() for
 *	more details.
 *  \note This may not be necessary anymore, with the new implementation of the reward delivery.
 *  \sa startReward()
 */
bool PictoBoxXPRewardController::rewardWasSupplied(unsigned int channel)
{
	if (channel > NUMREWARDLINES + 1 || channel < 1)
		return true;
	if (!daqTaskHandleDig_[channel])
		return true;
	//The "+.5" below is just to avoid possible weird racing effects where the Nidaq doesn't turn the 
	//signal off.  This hasn't ever actually happened, I'm just doing it to be safe.
	if (stopwatch_[channel].elapsedMs() <= latestOnTime_[channel] + .5)
		return false;
	DAQmxErrChk(DAQmxStopTask(daqTaskHandleDig_[channel]));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandleDig_[channel]));
	daqTaskHandleDig_[channel] = 0;

	return true;
}

/*! \brief Implements RewardController::startFlush() to raise the reward line voltage.
 *	\details Unlike in startReward() flushes are not stopped automatically.  The stopFlush() function is used
 *	to lower the reward line.
*/
void PictoBoxXPRewardController::startFlush(unsigned int channel)
{
	if(!hasDevice_)
		return;
	if(channel > NUMREWARDLINES+1 || channel < 1)
		return;
	//We don't really care about the channel here because there's currently 
	//only one channel supported in picto for the legacy system.

	uInt8 data[] = { 0 };
	int32 sampsPerChanWritten;

	DAQmxErrChk(DAQmxCreateTask(QString("RewardTaskDig%1").arg(channel).toLatin1(), (TaskHandle*)&daqTaskHandleDig_[channel]));
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandleDig_[channel], QString("Dev1/port2/line%1").arg(channel + 3).toLatin1(), "", DAQmx_Val_ChanPerLine));
	//(remember, we're using active low logic)
	DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandleDig_[channel], 1, true, 1, DAQmx_Val_GroupByChannel, data, &sampsPerChanWritten, NULL));
	if (!DAQmxStartTask(daqTaskHandleDig_[channel]))
	{
		stopwatch_[channel].startWatch();
		latestOnTime_[channel] = INT_MAX;
	}
	else
	{
		DAQmxErrChk(DAQmxStopTask(daqTaskHandleDig_[channel]));
		DAQmxErrChk(DAQmxClearTask(daqTaskHandleDig_[channel]));
		daqTaskHandleDig_[channel] = 0;
	}
}

/*! \brief Implements RewardController::stopFlush() to lower the reward line voltage.
 *	\sa startFlush()
*/
void PictoBoxXPRewardController::stopFlush(unsigned int channel)
{
	if(!hasDevice_)
		return;
	if(channel > NUMREWARDLINES+1 || channel < 1)
		return;

	if (!daqTaskHandleDig_[channel])
		return;
	//Stop the flush task
	DAQmxErrChk(DAQmxStopTask(daqTaskHandleDig_[channel]));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandleDig_[channel]));
	//Bring the reward enable pin back up
	uInt8 data[] = { 1 };
	int32 sampsPerChanWritten;
	DAQmxErrChk(DAQmxCreateTask("RewardTask", (TaskHandle*)&daqTaskHandleDig_[channel]));
	DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandleDig_[channel], QString("Dev1/port2/line%1").arg(channel + 3).toLatin1(), "", DAQmx_Val_ChanPerLine));
	if (!DAQmxStartTask(daqTaskHandleDig_[channel]))
	{
		DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandleDig_[channel], 1, true, 1, DAQmx_Val_GroupByChannel, data, &sampsPerChanWritten, NULL));
	}
	DAQmxErrChk(DAQmxStopTask(daqTaskHandleDig_[channel]));
	DAQmxErrChk(DAQmxClearTask(daqTaskHandleDig_[channel]));
	daqTaskHandleDig_[channel] = 0;
}


}; //namespace Picto