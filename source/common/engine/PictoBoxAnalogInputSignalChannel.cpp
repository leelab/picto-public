#include "PictoBoxAnalogInputSignalChannel.h"

namespace Picto {

PictoBoxAnalogInputSignalChannel::PictoBoxAnalogInputSignalChannel(QSharedPointer<PictoBoxdaqBoard_> daqBoard_, 
																	 int sampsPerSecond)
	: daqBoard_(daqBoard_),
	  SignalChannel(sampsPerSecond)
{
}
void PictoBoxAnalogInputSignalChannel::addAiChannel(QString subchannelName, int aiChannelNum)
{
	addSubchannel(subchannelName);
	aiChannelNums_[subchannelName] = aiChannelNum;
}

bool PictoBoxAnalogInputSignalChannel::start()
{
	daqBoard_->ClearAIChannelList();


	QMap<QString, int>::const_iterator x = aiChannelNums_.constBegin();
	while(x != aiChannelNums_.constEnd())
	{
		if(!daqBoard_->CreateAIChannel(x.value(), daqBoard_->kChanTypeRSE))
			return false;
		++x;
	}

	if(!daqBoard_->StartAI(sampleRate_,true))
	{
		return false;
	}
	return true;
}

bool PictoBoxAnalogInputSignalChannel::stop()
{
	if(daqBoard_->StopAI())
		return true;
	else
		return false;
}

void PictoBoxAnalogInputSignalChannel::updateDataBuffer()
{
	int maxSamples = 1000;
	int samplesCollected = maxSamples;
	PictoBoxdaqBoard_::AIChannel *aiChan;

	//collect data until the number of samples collected is less than the buffer size
	//This gets everything.
	while(samplesCollected == maxSamples)
	{
		samplesCollected = daqBoard_->CollectAIData(maxSamples);

		//for every subchannel, dump the data
		QMap<QString, int>::iterator x = aiChannelNums_.begin();
		while(x != aiChannelNums_.end())
		{
			aiChan = daqBoard_->GetAIChannel(x.value());
			for(int y=0; y<aiChan->voltages.size(); y++)
			{
				rawDataBuffer_[x.key()].push_back((double)aiChan->voltages[y]);
			}
			x++;
		}

	}

}

};