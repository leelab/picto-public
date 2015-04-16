#include <NIDAQmx.h>

#include "LegacySystemXPAnalogInputPort.h"

#include "../../common/timing/Timestamper.h"
#include "../../common/memleakdetect.h"
/*! \file */
/*! \brief A macro for checking the results of a NiDaq function call and triggering an assertion in the case of an error.
 */
#define DAQmxErrChk(rc) { if (rc) { \
							char error[512]; \
							DAQmxGetErrorString(rc, error,512); \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							QString msg = "DAQ function error:"; \
							msg.append(error); \
							Q_ASSERT_X(!rc, "LegacySystemXPAnalogInputPort", msg.toLatin1()); \
						} }
						
/*! \brief In the legacy system, device name for the PCI-MIO-16XE-50 should always be Dev2 because Orion wouldn't have
 *	worked otherwise.
 */
#define DEVICE_NAME "Dev2"
//! \brief The name used when creating the Analog Input task
#define TASK_NAME "SigChanCapture"
//! \brief The size of each channel of the signal buffer
#define BUFFER_SIZE_PER_CHAN 300

using namespace Picto;

/*! \brief Constructs a new LegacySystemXPAnalogInputPort.
 */
LegacySystemXPAnalogInputPort::LegacySystemXPAnalogInputPort()
{
	daqTaskHandle_ = 0;
	dataBuffer_ = NULL;
	bufferSize_ = 0;
}

/*!	\copydoc InputPort::startSampling()
 *	\details In the case of NiDaq systems, we create a NiDaq analog input capture "Task" (DAQmxCreateAIVoltageChan()) and
 *	run it whenever we want to sample input data (DAQmxStartTask()).
 *	\details Sample rate is configured using DAQmxCfgSampClkTiming().
 */
bool LegacySystemXPAnalogInputPort::startSampling()
{

	//Clear the existing task
	if(daqTaskHandle_)
	{
		DAQmxErrChk (DAQmxClearTask(daqTaskHandle_));
		daqTaskHandle_ = 0;
	}

	uInt32 hasDevice;
	DAQmxErrChk(DAQmxGetDevSerialNum(DEVICE_NAME,&hasDevice))
	Q_ASSERT(hasDevice);

	if(!hasDevice)
		return false;
	
	//Create the buffer that will be used for read operations.  Its size depends on the number of channels so we generate
	//	it here where we are commited our final list of channels.
	if(dataBuffer_)
	{
		delete[] dataBuffer_;
		dataBuffer_ = NULL;
		bufferSize_ = 0;
	}
	bufferSize_  = BUFFER_SIZE_PER_CHAN * channelNums_.size();
	dataBuffer_ = new int16[bufferSize_];

	//Create the new task
	DAQmxErrChk (DAQmxCreateTask(TASK_NAME,&daqTaskHandle_));
	QString physicalChannel;
	foreach(int aiChannel,channelNums_)
	{
		physicalChannel = QString("%1/ai%2").arg(DEVICE_NAME).arg(aiChannel);
		DAQmxErrChk (DAQmxCreateAIVoltageChan(daqTaskHandle_,physicalChannel.toLatin1(),"",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
	}
	int bufferSize = 5000;
	DAQmxErrChk (DAQmxCfgSampClkTiming(daqTaskHandle_,"",deviceSampRate_,DAQmx_Val_Rising,DAQmx_Val_ContSamps,bufferSize));
	DAQmxErrChk (DAQmxStartTask(daqTaskHandle_));
	return true;
}

void LegacySystemXPAnalogInputPort::stopSampling()
{
	DAQmxErrChk (DAQmxStopTask(daqTaskHandle_));
	//Since we created the dataBuffer_ during startSampling(), we delete it during stopSampling()
	if(dataBuffer_)
	{
		delete[] dataBuffer_;
		dataBuffer_ = NULL;
	}
	return;
}

/*! \copydoc InputPort::updateDataBuffer()
 *	\details The latest data is read in from the NiDaq's buffer using DAQmxReadBinaryI16().  We estimate the precise
 *	read time by storing the time just after that function is called.  That is the time that is returned from the function.
 */
double LegacySystemXPAnalogInputPort::updateDataBuffer()
{
	long totalSampsRead = 0;
	long sampsPerChanRead = 0;
	Timestamper stamper;
	double readTime;

	//NOTE: This assumes that the data buffer has the data ordered by channel in the same order the channels were created.
	//	It is possbile that the ordering is instead numerical, in which case this will need to be rewritten
	do
	{
		//read the NIDAQ
		DAQmxErrChk (DAQmxReadBinaryI16(daqTaskHandle_,DAQmx_Val_Auto,0.0,DAQmx_Val_GroupByChannel,dataBuffer_,bufferSize_,&sampsPerChanRead,NULL));
		readTime = stamper.stampSec();

		//copy the data to our buffer
		int bufferOffset = 0;
		QVector<double>* currList;
		int prevListSize;
		for(int i=0;i<channelNums_.size();i++)
		{
			currList = &(channelBuffers_[channelNums_[i]]);
			prevListSize = currList->size();
			currList->resize(prevListSize+sampsPerChanRead);
			for(int i=0; i<sampsPerChanRead; i++)
				(*currList)[prevListSize+i] = dataBuffer_[bufferOffset + i];
			bufferOffset += sampsPerChanRead;
		}
		totalSampsRead += sampsPerChanRead;
	}
	while ( sampsPerChanRead == BUFFER_SIZE_PER_CHAN ); //In case there are more data points than will fit in the buffer

	return readTime;
}

