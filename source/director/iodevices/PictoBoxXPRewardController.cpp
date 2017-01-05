#include "PictoBoxXPRewardController.h"
#include <windows.h>
#include <NIDAQmx.h>
#include "../../common/memleakdetect.h"
#include "../../common/globals.h"

/*! \brief A macro used to check for NiDaq errors and trigger an assertion if they occur.*/
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

// NOTE: The Pictobox only has one NiDaq card which is always Dev1
#define DEVICE_NAME "Dev1"
/*! \brief For Pictobox5/System10 (older DAQ, series E): we use one Analog output channel as a reward enable channel which provides power
*	to the Pictobox Reward solenoid.  This defines that channel.
*/
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
				//If the function fails on StartTask, don't trigger assertions because it might just be that another process
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

	PictoBoxXPRewardController::~PictoBoxXPRewardController()
	{
		if (!hasDevice_)
			return;

		// Lines will go back to zero on their own as soon as things reset.
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
	void PictoBoxXPRewardController::startReward(unsigned int channel, int quantity)
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
			//TEST CODE for Digital Channels
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

	/*! \brief Implements RewardController::rewardWasSupplied() to return true when the latest reward to have been started using startReward() is complete.
	*	\details In the case of the PictoBoxXPRewardController, we piggyback onto this function and
	*	actually stop the reward task once the reward has been completely supplied.  See startReward() for
	*	more details.
	*/
	bool PictoBoxXPRewardController::rewardWasSupplied(unsigned int channel)
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

	/*! \brief Implements RewardController::startFlush() to raise the reward line voltage.
	*	\details Unlike in startReward() flushes are not stopped automatically.  The stopFlush() function is used
	*	to lower the reward line.
	*/
	void PictoBoxXPRewardController::startFlush(unsigned int channel)
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

	/*! \brief Implements RewardController::stopFlush() to lower the reward line voltage.
	*	\sa startFlush()
	*/
	void PictoBoxXPRewardController::stopFlush(unsigned int channel)
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
			//Stop the flush task
			DAQmxErrChk(DAQmxStopTask(daqTaskHandle_[channel]));
			DAQmxErrChk(DAQmxClearTask(daqTaskHandle_[channel]));
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