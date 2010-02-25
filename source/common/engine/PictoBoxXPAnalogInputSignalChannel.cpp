#include <NIDAQmx.h>

#include "PictoBoxXPAnalogInputSignalChannel.h"


#define DAQmxErrChk(rc) { if (rc) { \
							DAQmxStopTask(daqTaskHandle_); \
							DAQmxClearTask(daqTaskHandle_); \
							Q_ASSERT_X(!rc, "PictoBoxXPAnalogInputSignalChannel", "DAQ function failure");\
						 } }

namespace Picto {

PictoBoxXPAnalogInputSignalChannel::PictoBoxXPAnalogInputSignalChannel(int sampsPerSecond)
	: SignalChannel(sampsPerSecond)
{
	//all of the initialization takes place in start, while all of the
	//cleanup takes place in stop
	daqTaskHandle_ = 0;
}
void PictoBoxXPAnalogInputSignalChannel::addAiChannel(QString subchannelName, int aiChannelNum)
{
	addSubchannel(subchannelName);
	aiChannelNums_[subchannelName] = aiChannelNum;
}

bool PictoBoxXPAnalogInputSignalChannel::start()
{
	//Clear the existing task
	if(daqTaskHandle_)
		DAQmxErrChk (DAQmxClearTask(daqTaskHandle_));

	//Create the new task
	DAQmxErrChk (DAQmxCreateTask("AnalogInput",&daqTaskHandle_));
	QString physicalChannel;
	foreach(int aiChannel,aiChannelNums_)
	{
		physicalChannel = QString("Dev1/ai%1").arg(aiChannel);
		DAQmxErrChk (DAQmxCreateAIVoltageChan(daqTaskHandle_,physicalChannel.toLatin1(),"",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
	}
	int bufferSize = 5000;
	DAQmxErrChk (DAQmxCfgSampClkTiming(daqTaskHandle_,"",sampleRate_,DAQmx_Val_Rising,DAQmx_Val_ContSamps,bufferSize));
	DAQmxErrChk (DAQmxStartTask(daqTaskHandle_));


	return true;

}

bool PictoBoxXPAnalogInputSignalChannel::stop()
{
	if(daqTaskHandle_)
		DAQmxErrChk (DAQmxStopTask(daqTaskHandle_));

	return true;
}

void PictoBoxXPAnalogInputSignalChannel::updateDataBuffer()
{
	long bufferSizePerChan = 300;
	long sampsPerChanRead = 0;
	double *buffer;
	unsigned int bufferSize  = bufferSizePerChan * aiChannelNums_.size();
	buffer = new double[bufferSize];

	//NOTE: This assumes that the data buffer has the data ordered by channel
	//in the same order as the channels were created.  It is possbile that 
	//the ordering is instead numerical, in which case this will need to be 
	//rewritten
	do
	{
		//read the NIDAQ
		DAQmxErrChk (DAQmxReadAnalogF64(daqTaskHandle_,-1,0.0,DAQmx_Val_GroupByChannel,buffer,bufferSize,&sampsPerChanRead,NULL));

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
	}while(sampsPerChanRead == bufferSizePerChan);

}

};