#include "FrontPanelRewardController.h"
#include <windows.h>
#include <NIDAQmx.h>
#include "../../common/memleakdetect.h"

/*! A macro used to check the output of DAQmx calls and trigger an appropriate assertion
 * on failure
 */
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

/*! The name of the Nidaq device handling reward triggering (it is the only Nidaq device in the Pictobox) 
 * \note We hard coding the NIDAQ setup, since this code is only intended to run on PictoBox 
 * where we have full hardware control.  If this is meant to run elsewhere, a more
 * generic RewardController will need to be written
 */
#define DEVICE_NAME "Dev1"

/*! The Nidaq line used to act as the High-Enable signal for the Relay board.
 */
#define PICTO_BOX_NIDAQ_RELAY_ENABLE_CHAN "Dev1/port2/line1"

/*! \brief The number of reward lines on the Pictobox.  Currently only one is allowed.
 * \details currently, the reward enable line is on PFI 9, and the reward trigger is on PFI 12.
 */
#define NUMREWARDLINES 2


namespace Picto
{

/*! \brief Sets up the reward controller
 * \details Attempts to set the reward trigger and then reward enable lines high.  This way to provide a reward we need
 * only set the reward trigger line low for the input reward duration.
 * \note If the Nidaq reward tasks fail to start, this is not considered an error here. The reason for this is that the FrontPanelRewardController
 * is built to work along side the Legacy Orion application so that there is still the option to run old Orion tasks on 
 * PictoBox.  If Orion is running, it will be taking control of the reward lines whenever it needs to provide a reward,
 * in which case this reward controller might be locked out of using them temporarily.  Either way, someone will be enabling
 * the reward relays though, and so even when Orion is running Front Panel rewards can still be provided unless an Orion
 * reward is already in progress (in which case we don't really care that the front panel reward won't work).
 */
FrontPanelRewardController::FrontPanelRewardController()
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
	if (!DAQmxStartTask(rewardEnTask))
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
FrontPanelRewardController::~FrontPanelRewardController()
{
	if(!hasDevice_)
		return;

	// Lines will go back to zero on their own as soon as things reset.
}

/*! \brief Starts a reward an the input channel for the 'quantity' number of milliseconds
 * \details Previous limitations due to the old implementation have been lifted, and now rewards can be
 * given with any amount of pause between.
 * \sa rewardWasSupplied()
 */
void FrontPanelRewardController::startReward(unsigned int channel,int quantity)
{
	if (!hasDevice_)
		return;
	if (channel > NUMREWARDLINES + 1 || channel < 1)
		return;

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

/*! \brief Returns true if the latest reward has been fully supplied, also turns off the reward task in this case if it is still active.
 * \details This function should be called at least once every 16ms.
 * \note This may not be necessary anymore, with the new implementation of the reward delivery.
 * \sa startReward()
 */
bool FrontPanelRewardController::rewardWasSupplied(unsigned int channel)
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

/*! \brief Starts a flush on the input channel
 * \details Since flush timing doesn't need to be precise, we don't bother ourselves with creating a clock and using
 * it to control the reward trigger line (as in startReward()).  Here we simply turn the flush on, and the let the 
 * call to stopFlush(), which the RewardController should handle, stop it.  Since the reward controller should be checking
 * this at least once every 16ms, the resolution is fine as far as the flush is concerned.
 * \sa stopFlush(), RewardController::triggerRewards()
 */
void FrontPanelRewardController::startFlush(unsigned int channel)
{
	if (!hasDevice_)
		return;
	if (channel > NUMREWARDLINES + 1 || channel < 1)
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
/*! \brief Stops an active flush by setting the reward signal line back to high and then clearing the NiDaq task.
 * \sa startFlush()
 */
void FrontPanelRewardController::stopFlush(unsigned int channel)
{
	if (!hasDevice_)
		return;
	if (channel > NUMREWARDLINES + 1 || channel < 1)
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