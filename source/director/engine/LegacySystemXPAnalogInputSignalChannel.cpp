#include <NIDAQmx.h>

#include "LegacySystemXPAnalogInputSignalChannel.h"

#include "../../common/timing/Timestamper.h"
#include "../../common/memleakdetect.h"

#define DAQmxErrChk(rc) { if (rc) { \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							Q_ASSERT_X(!rc, "LegacySystemXPAnalogInputSignalChannel", "DAQ function failure");\
						 } }
#define DEVICE_NAME "Dev2"
namespace Picto {

LegacySystemXPAnalogInputSignalChannel::LegacySystemXPAnalogInputSignalChannel(QString name, int sampsPerSecond)
	: SignalChannel(name,sampsPerSecond)
{
	//all of the initialization takes place in start, while all of the
	//cleanup takes place in stop
	daqTaskHandle_ = 0;

	//If the analog input represents behavioral data, it will be expected to have
	//a timestamp subchannel
	addSubchannel("time");

}
void LegacySystemXPAnalogInputSignalChannel::addAiChannel(QString subchannelName, int aiChannelNum)
{
	addSubchannel(subchannelName);
	aiChannelNums_[subchannelName] = aiChannelNum;
}

/*!	\brief Starts the DAQ collecting data.
 *
 *	This should be called as close to the point at which the data will actually
 *	be collected as possible.  Otherwise, data will pile up in the buffer.
 *	Also note that calling this clears out any existing data in the channel.
 */
bool LegacySystemXPAnalogInputSignalChannel::start()
{

	Timestamper stamper;

	//Clear the existing task
	if(daqTaskHandle_)
	{
		DAQmxErrChk (DAQmxClearTask(daqTaskHandle_));
	}
	else
	{
		uInt32 tmp;
		DAQmxErrChk(DAQmxGetDevSerialNum(DEVICE_NAME,&tmp))
		Q_ASSERT(tmp);
		hasDevice_ = tmp;
	}

	if(!hasDevice_)
		return false;
	
	//Create the new task
	DAQmxErrChk (DAQmxCreateTask(getName().toAscii(),&daqTaskHandle_));
	QString physicalChannel;
	foreach(int aiChannel,aiChannelNums_)
	{
		physicalChannel = QString("%1/ai%2").arg(DEVICE_NAME).arg(aiChannel);
		DAQmxErrChk (DAQmxCreateAIVoltageChan(daqTaskHandle_,physicalChannel.toLatin1(),"",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
	}
	int bufferSize = 5000;
	DAQmxErrChk (DAQmxCfgSampClkTiming(daqTaskHandle_,"",sampleRate_,DAQmx_Val_Rising,DAQmx_Val_ContSamps,bufferSize));
	DAQmxErrChk (DAQmxStartTask(daqTaskHandle_));
	
	//Mark the start time
	startTime_ = stamper.stampSec();
	sampsCollected_=0;

	//Clear out the subchannels
	foreach(QList<double> subchannel, rawDataBuffer_)
	{
		subchannel.clear();
	}

	return true;

}

bool LegacySystemXPAnalogInputSignalChannel::stop()
{
	if(daqTaskHandle_)
		DAQmxErrChk (DAQmxStopTask(daqTaskHandle_));



	return true;
}

void LegacySystemXPAnalogInputSignalChannel::updateDataBuffer()
{
	long bufferSizePerChan = 300;
	long sampsPerChanRead = 0;
	unsigned int bufferSize  = bufferSizePerChan * aiChannelNums_.size();
	//double *buffer;
	//buffer = new double[bufferSize];
	int16 *buffer;
	buffer = new int16[bufferSize];

	//NOTE: This assumes that the data buffer has the data ordered by channel
	//in the same order as the channels were created.  It is possbile that 
	//the ordering is instead numerical, in which case this will need to be 
	//rewritten
	do
	{
		//read the NIDAQ
		//DAQmxErrChk (DAQmxReadAnalogF64(daqTaskHandle_,-1,0.0,DAQmx_Val_GroupByChannel,buffer,bufferSize,&sampsPerChanRead,NULL));
		DAQmxErrChk (DAQmxReadBinaryI16(daqTaskHandle_,DAQmx_Val_Auto,0.0,DAQmx_Val_GroupByChannel,buffer,bufferSize,&sampsPerChanRead,NULL));

		//copy the data to our buffer
		QMap<QString, int>::iterator chan = aiChannelNums_.begin();
		int bufferOffset = 0;
		while(chan != aiChannelNums_.end())
		{
			for(int y=0; y<sampsPerChanRead; y++)
				rawDataBuffer_[chan.key()].push_back(buffer[bufferOffset + y]);
			bufferOffset += sampsPerChanRead;
			chan++;
		}

		//Add the timestamps
		//double baseTime = startTime_ + sampsCollected_/(double)sampleRate_;

		//for(int x=0; x<sampsPerChanRead; x++)
			//rawDataBuffer_["time"].push_back(baseTime + x/(double)sampleRate_);

		for(int x=0; x<sampsPerChanRead; x++)
		{
			rawDataBuffer_["time"].push_back(startTime_ + sampsCollected_/(double)sampleRate_);
			sampsCollected_++;
		}

		//sampsCollected_ += sampsPerChanRead;

	}while(sampsPerChanRead == bufferSizePerChan);

	delete[] buffer;
}

};