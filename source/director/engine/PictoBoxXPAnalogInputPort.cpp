#include <NIDAQmx.h>

#include "PictoBoxXPAnalogInputPort.h"

#include "../../common/timing/Timestamper.h"
#include "../../common/memleakdetect.h"

#define DAQmxErrChk(rc) { if (rc) { \
							char error[512]; \
							DAQmxGetErrorString(rc, error,512); \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							QString msg = "DAQ function error:"; \
							msg.append(error); \
							Q_ASSERT_X(!rc, "PictoBoxXPAnalogInputPort", msg.toAscii()); \
						} }
						

#define DEVICE_NAME "Dev1"
#define TASK_NAME "SigChanCapture"
#define BUFFER_SIZE_PER_CHAN 300
using namespace Picto;

PictoBoxXPAnalogInputPort::PictoBoxXPAnalogInputPort()
{
	daqTaskHandle_ = 0;
	dataBuffer_ = NULL;
	bufferSize_ = 0;
}

/*!	\brief Starts the DAQ collecting data.
 *
 *	This should be called as close to the point at which the data will actually
 *	be collected as possible.  Otherwise, data will pile up in the buffer.
 *	Also note that calling this clears out any existing data in the channel.
 */
bool PictoBoxXPAnalogInputPort::startSampling()
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
	
	//Create the buffer that will be used for read operations.  Its size depends on the number of channels
	//so we generate it here where we are commited our final list of channels.
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

void PictoBoxXPAnalogInputPort::stopSampling()
{
	DAQmxErrChk (DAQmxStopTask(daqTaskHandle_));
	if(dataBuffer_)
	{
		delete[] dataBuffer_;
		dataBuffer_ = NULL;
	}
	return;
}

double PictoBoxXPAnalogInputPort::updateDataBuffer()
{
	long totalSampsRead = 0;
	long sampsPerChanRead = 0;
	Timestamper stamper;
	double readTime;

	//NOTE: This assumes that the data buffer has the data ordered by channel
	//in the same order as the channels were created.  It is possbile that 
	//the ordering is instead numerical, in which case this will need to be 
	//rewritten
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
	}while(sampsPerChanRead == BUFFER_SIZE_PER_CHAN); //In case there are more data points than will fit in the buffer
	return readTime;
}

//void PictoBoxXPAnalogInputPort::updateDataBuffer()
//{
//	port_->updateDataBuffer(/*PUT FIRST PHOSPHOR TIMESTAMP HERE*/);
//	QMap<QString,int>::iterator iter = channelNumMap_.begin();
//	int frameOffset = 0;
//	for(;iter != channelNumMap_.end();iter++)
//	{
//		rawDataBuffer_[iter.key()] << port_->getData(iter.value());
//	}
//	int frameOffset = port_->getFrameToSampleOffset(iter.value());
//	iter = channelNumMap_.begin();
//	int i=0;
//	while(rawDataBuffer_["time"].size() < rawDataBuffer_[iter.key()])
//	{
//		rawDataBuffer_["time"] << frameOffset+msPerSample_*i++;
//	}
//		
//}
