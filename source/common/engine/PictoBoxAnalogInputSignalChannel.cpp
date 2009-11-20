#include "PictoBoxAnalogInputSignalChannel.h"

namespace Picto {

PictoBoxAnalongInputSignalChannel::PictoBoxAnalongInputSignalChannel(QSharedPointer<PictoBoxDaqBoard> daqBoard, 
																	 int sampsPerSecond)
	: daqBoard(daqBoard),
	  SignalChannel(sampsPerSecond)
{
}
void PictoBoxAnalongInputSignalChannel::addAiChannel(QString subchannelName, int aiChannelNum)
{
	addSubchannel(subchannelName);
	aiChannelNums[subchannelName] = aiChannelNum;
}

bool PictoBoxAnalongInputSignalChannel::start()
{
	daqBoard->StopAI();
	daqBoard->ClearAIChannelList();

	QMap<QString, int>::iterator x = aiChannelNums.begin();
	while(x != aiChannelNums.end())
	{
		if(!daqBoard->CreateAIChannel(x.value(), daqBoard->kChanTypeRSE))
			return false;
		x++;
	}
	if(!daqBoard->StartAI(sampleRate,true))
		return false;
	return true;
}

bool PictoBoxAnalongInputSignalChannel::stop()
{
	if(daqBoard->StopAI())
		return true;
	else
		return false;
}

void PictoBoxAnalongInputSignalChannel::updateDataBuffer()
{
	int maxSamples = 1000;
	int samplesCollected = maxSamples;
	PictoBoxDaqBoard::AIChannel *aiChan;

	//collect data until the number of samples collected is less than the buffer size
	//This gets everything.
	while(samplesCollected == maxSamples)
	{
		samplesCollected = daqBoard->CollectAIData(maxSamples);

		//for every subchannel, dump the data
		QMap<QString, int>::iterator x = aiChannelNums.begin();
		while(x != aiChannelNums.end())
		{
			aiChan = daqBoard->GetAIChannel(x.value());
			for(int y=0; y<aiChan->voltages.size(); y++)
			{
				rawDataBuffer[x.key()].push_back((double)aiChan->voltages[y]);
			}
			x++;
		}

	}

}

};