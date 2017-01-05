#include "FrontPanelRewardController.h"
#include <windows.h>
#include <NIDAQmx.h>
#include "../../common/memleakdetect.h"
#include "../../common/globals.h"

/*! A marcro used to check the output of DAQmx calls and trigger an appropriate assertion
* on failure
*/
#define DAQmxErrChk(rc) { if (rc) { \
							char error[512]; \
							DAQmxGetErrorString(rc, error,512); \
							DAQmxStopTask(daqTaskHandle_[channel]); \
							DAQmxClearTask(daqTaskHandle_[channel]); \
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
//#define PICTO_BOX_NIDAQ_5V_CHANNEL "Dev1/port0/line0"
#define PICTO_BOX_NIDAQ_5V_CHANNEL "Dev1/ao1"
/*! Other Pictoboxes (DAQ Series M):
*/
/*! The Nidaq line used to act as the High-Enable signal for the Relay board.
*/
#define PICTO_BOX_NIDAQ_RELAY_ENABLE_CHAN "Dev1/port2/line1"
/*! \brief The number of reward lines on the Pictobox.
*	\details Newer Pictoboxes (not PB5/System10): the reward enable line is on PFI 9, and the reward triggers are on PFI 12,
*	PFI 13, PFI 14, and PFI 15.
*/
int NUMREWARDLINES = 4;



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
		// For the older system Pictobox5 / System10 (series E), the reward enable line is on ao1, and the reward trigger is on a00.If we increase
		//the NUMREWARDLINES, channel 2 will try to overwrite ao1, which won't work.
		if (Picto::portNums->getSystemNumber() == 10)
			NUMREWARDLINES = 1;

		outputData[0] = 0.0;
		outputData[1] = 5.0;
		// Skip if device doesn't exist
		uInt32 tmp;
		int channel = 1;
		DAQmxErrChk(DAQmxGetDevSerialNum(DEVICE_NAME, &tmp))
			Q_ASSERT(tmp);
		hasDevice_ = tmp;
		if (!hasDevice_)
			return;

		if (Picto::portNums->getSystemNumber() == 10)
		{
			float64 data[] = { 5.0 };
			int32 sampsPerChanWritten;
			for (int channel = 1; channel < NUMREWARDLINES + 1; channel++)
			{
				DAQmxErrChk(DAQmxCreateTask("RewardTask", (TaskHandle*)&daqTaskHandle_[channel]));
				DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_[channel], QString("Dev1/ao%1").arg(channel - 1).toLatin1(), "", 0.0, 5.0, DAQmx_Val_Volts, NULL));
				//If the function fails on StartTask, don't trigger assertions because it might just be that another process (ie. legacy Orion)
				//is using the pin we want to control.  If there's an error here, just skip the task.
				if (!DAQmxStartTask(daqTaskHandle_[channel]))
					DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_[channel], 1, true, 1, DAQmx_Val_GroupByChannel, data, &sampsPerChanWritten, NULL));
				DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
				DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
				daqTaskHandle_[channel] = 0;
			}
			channel = 1;

			//Once the reward trigger lines are high, raise the line being used for +5 to make the relay operational.
			//This setup prevents the relay closing during PictoBox power up when the trigger lines will be low.
			TaskHandle rewardEnTask;
			DAQmxErrChk(DAQmxCreateTask("RewardEnableTask", (TaskHandle*)&rewardEnTask));
			//DAQmxErrChk(DAQmxCreateDOChan(rewardEnTask,PICTO_BOX_NIDAQ_5V_CHANNEL,"",DAQmx_Val_ChanForAllLines));
			DAQmxErrChk(DAQmxCreateAOVoltageChan(rewardEnTask, QString("Dev1/ao1").toLatin1(), "", 0.0, 5.0, DAQmx_Val_Volts, NULL));
			if (!DAQmxStartTask(rewardEnTask))
			{
				//Set digital lines to +5
				//const uInt8 rewardEnData[] = {0xFF};
				//DAQmxErrChk(DAQmxWriteDigitalU8(rewardEnTask,1,1,1.0,DAQmx_Val_GroupByChannel,rewardEnData,&sampsPerChanWritten,NULL));
				DAQmxErrChk(DAQmxWriteAnalogF64(rewardEnTask, 1, true, 1, DAQmx_Val_GroupByChannel, data, &sampsPerChanWritten, NULL));
			}
			//Clear this setup task
			DAQmxErrChk(DAQmxStopTask(rewardEnTask));
			DAQmxErrChk(DAQmxClearTask(rewardEnTask));
		}
		else
		{
			uInt8 data[] = { 1 };
			int32 sampsPerChanWritten;

			for (int channel = 1; channel<NUMREWARDLINES + 1; channel++)
			{
				DAQmxErrChk(DAQmxCreateTask("RewardTaskDig", (TaskHandle*)&daqTaskHandle_[channel]));
				DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_[channel], QString("Dev1/port2/line%1").arg(channel + 3).toLatin1(), "", DAQmx_Val_ChanPerLine));
				//If the function fails on StartTask, don't trigger assertions because it might just be that another process
				//is using the pin we want to control.  If there's an error here, just skip the task.
				if (!DAQmxStartTask(daqTaskHandle_[channel]))
					DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_[channel], 1, true, 1, DAQmx_Val_GroupByChannel, data, &sampsPerChanWritten, NULL));
				DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
				DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
				daqTaskHandle_[channel] = 0;
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
	}

	/*! This desructor doesn't need to bring the reward enable line back to zero.  Since it's an enable line,
	* it will return to zero automatically as soon as things reset
	*/
	FrontPanelRewardController::~FrontPanelRewardController()
	{
		if (!hasDevice_)
			return;

		//Since the reward enable line is an analog line, it will go back to zero on its own as soon
		//as things reset.
	}

	/*! \brief Starts a reward an the input channel for the 'quantity' number of milliseconds
	* \detail old Pictobox5/System10: Due to the limited capabilities of our current version of the NiDaq card, the way
	* that rewards are precisely timed is that we start a clock with frequency equal to the input
	* quantity.  We then start a task that switches the A0.0 pin to 0V, then 5V, then 0V, etc, (remember, active
	* low logic) each time the clock triggers.  Since we make sure to call rewardWasSupplied() once every 16ms, we can
	* be sure that the reward task will be shut off after the A0.0 pin goes to 5V so long as the reward duration is longer
	* than 16ms.  Ideally we would set up the NiDaq task to simply bring the A0.0 pin to 0V, then back to 5V after
	* quantity ms, but Nidaq doesn't support this, so we had to create a work around.
	* \sa rewardWasSupplied()
	*/
	/*! \brief All other Pictoboxes (not Pictobox5/System10): starts a reward an the input channel for the 'quantity' number of milliseconds
	* \details Previous limitations due to the old implementation have been lifted, and now rewards can be
	* given with any amount of pause between.
	* \sa rewardWasSupplied()
	*/
	void FrontPanelRewardController::startReward(unsigned int channel, int quantity)
	{
		if (!hasDevice_)
			return;

		// For the older system Pictobox5 / System10 (series E), the reward enable line is on ao1, and the reward trigger is on a00.If we increase
		//the NUMREWARDLINES, channel 2 will try to overwrite ao1, which won't work.
		if (Picto::portNums->getSystemNumber() == 10)
			NUMREWARDLINES = 1;

		if (channel > NUMREWARDLINES + 1 || channel < 1)
			return;

		if (Picto::portNums->getSystemNumber() == 10)
		{
			DAQmxErrChk(DAQmxCreateTask(QString("RewardTask%1").arg(channel).toLatin1(), (TaskHandle*)&daqTaskHandle_[channel]));
			DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_[channel], QString("Dev1/ao%1").arg(channel - 1).toLatin1(), "", 0.0, 5.0, DAQmx_Val_Volts, NULL));
			DAQmxErrChk(DAQmxCfgSampClkTiming(daqTaskHandle_[channel], "", 1000.0 / float64(quantity), DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, 2));
			int32 sampsPerChanWritten;
			//(remember, we're using active low logic)
			DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_[channel], 2, 0, -1, DAQmx_Val_GroupByChannel, outputData, &sampsPerChanWritten, NULL));
		}
		else
		{
			DAQmxErrChk(DAQmxCreateTask(QString("RewardTaskDig%1").arg(channel).toLatin1(), (TaskHandle*)&daqTaskHandle_[channel]));
			DAQmxErrChk(DAQmxCreateCOPulseChanTime(daqTaskHandle_[channel], QString("Dev1/ctr%1").arg(channel - 1).toLatin1(), "", DAQmx_Val_Seconds, DAQmx_Val_High, 0.00, float64(quantity) / 1000.0, 0.01));
		}


		if (!DAQmxStartTask(daqTaskHandle_[channel]))
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

	/*! \brief Returns true if the latest reward has been fully supplied, also turns off the reward task in this case if it is still active.
	* \details This function should be called at least once every 16ms.
	* \sa startReward()
	*/
	bool FrontPanelRewardController::rewardWasSupplied(unsigned int channel)
	{
		// For the older system Pictobox5 / System10 (series E), the reward enable line is on ao1, and the reward trigger is on a00.If we increase
		//the NUMREWARDLINES, channel 2 will try to overwrite ao1, which won't work.
		if (Picto::portNums->getSystemNumber() == 10)
			NUMREWARDLINES = 1;

		if (channel > NUMREWARDLINES + 1 || channel < 1)
			return true;
		if (!daqTaskHandle_[channel])
			return true;
		//The "+.5" below is just to avoid possible weird racing effects where the Nidaq doesn't turn the 
		//signal off.  This hasn't ever actually happened, I'm just doing it to be safe.
		if (stopwatch_[channel].elapsedMs() <= latestOnTime_[channel] + .5)
			return false;
		DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
		DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
		daqTaskHandle_[channel] = 0;
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

		// For the older system Pictobox5 / System10 (series E), the reward enable line is on ao1, and the reward trigger is on a00.If we increase
		//the NUMREWARDLINES, channel 2 will try to overwrite ao1, which won't work.
		if (Picto::portNums->getSystemNumber() == 10)
			NUMREWARDLINES = 1;

		if (channel > NUMREWARDLINES + 1 || channel < 1)
			return;

		if (Picto::portNums->getSystemNumber() == 10)
		{
			//We don't really care about the channel here because there's currently 
			//only one channel supported in picto for the legacy system.
			DAQmxErrChk(DAQmxCreateTask(QString("RewardTask%1").arg(channel).toLatin1(), (TaskHandle*)&daqTaskHandle_[channel]));
			DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_[channel], QString("Dev1/ao%1").arg(channel - 1).toLatin1(), "", 0.0, 5.0, DAQmx_Val_Volts, NULL));
			int32 sampsPerChanWritten;
			//(remember, we're using active low logic)
			DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_[channel], 1, true, 1, DAQmx_Val_GroupByChannel, outputData, &sampsPerChanWritten, NULL));
		}
		else
		{
			//We don't really care about the channel here because there's currently 
			//only one channel supported in picto for the legacy system.
			uInt8 data[] = { 0 };
			int32 sampsPerChanWritten;

			DAQmxErrChk(DAQmxCreateTask(QString("RewardTaskDig%1").arg(channel).toLatin1(), (TaskHandle*)&daqTaskHandle_[channel]));
			DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_[channel], QString("Dev1/port2/line%1").arg(channel + 3).toLatin1(), "", DAQmx_Val_ChanPerLine));
			//(remember, we're using active low logic)
			DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_[channel], 1, true, 1, DAQmx_Val_GroupByChannel, data, &sampsPerChanWritten, NULL));
		}

		if (!DAQmxStartTask(daqTaskHandle_[channel]))
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
	/*! \brief Stops an active flush by setting the A0.0 line back to high and then clearing the NiDaq task.
	* \sa startFlush()
	*/
	void FrontPanelRewardController::stopFlush(unsigned int channel)
	{
		if (!hasDevice_)
			return;

		// For the older system Pictobox5 / System10 (series E), the reward enable line is on ao1, and the reward trigger is on a00.If we increase
		//the NUMREWARDLINES, channel 2 will try to overwrite ao1, which won't work.
		if (Picto::portNums->getSystemNumber() == 10)
			NUMREWARDLINES = 1;


		if (channel > NUMREWARDLINES + 1 || channel < 1)
			return;
		if (!daqTaskHandle_[channel])
			return;
		//Stop the flush task
		DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
		DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));

		if (Picto::portNums->getSystemNumber() == 10)
		{
			//Bring the reward enable pin back up
			float64 data[] = { 5.0 };
			int32 sampsPerChanWritten;
			DAQmxErrChk(DAQmxCreateTask("RewardTask", (TaskHandle*)&daqTaskHandle_[channel]));
			DAQmxErrChk(DAQmxCreateAOVoltageChan(daqTaskHandle_[channel], QString("Dev1/ao%1").arg(channel - 1).toLatin1(), "", 0.0, 5.0, DAQmx_Val_Volts, NULL));
			if (!DAQmxStartTask(daqTaskHandle_[channel]))
			{
				DAQmxErrChk(DAQmxWriteAnalogF64(daqTaskHandle_[channel], 1, true, 1, DAQmx_Val_GroupByChannel, data, &sampsPerChanWritten, NULL));
			}
		}
		else
		{
			//Bring the reward enable pin back up
			uInt8 data[] = { 1 };
			int32 sampsPerChanWritten;
			DAQmxErrChk(DAQmxCreateTask("RewardTask", (TaskHandle*)&daqTaskHandle_[channel]));
			DAQmxErrChk(DAQmxCreateDOChan(daqTaskHandle_[channel], QString("Dev1/port2/line%1").arg(channel + 3).toLatin1(), "", DAQmx_Val_ChanPerLine));
			if (!DAQmxStartTask(daqTaskHandle_[channel]))
			{
				DAQmxErrChk(DAQmxWriteDigitalLines(daqTaskHandle_[channel], 1, true, 1, DAQmx_Val_GroupByChannel, data, &sampsPerChanWritten, NULL));
			}
		}

		DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
		DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
		daqTaskHandle_[channel] = 0;
	}


}; //namespace Picto