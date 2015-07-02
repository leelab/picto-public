#include <QXmlStreamWriter>
#include <windows.h>
#include "plexon.h"
#include "plexonplugin.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a PlexonPlugin object.*/
PlexonPlugin::PlexonPlugin()
{
	deviceStatus_ = notStarted;
}

QString PlexonPlugin::device() const
{
	return "Plexon";
}


NeuralDataAcqInterface::deviceStatus PlexonPlugin::startDevice()
{
	lfpData_.clear();
	PL_CloseClient();
	PL_InitClientEx3(0, NULL, NULL);	//! \todo Check if the return value indicates success!
	deviceStatus_ = started;
	return deviceStatus_;
}

NeuralDataAcqInterface::deviceStatus PlexonPlugin::stopDevice()
{
	PL_CloseClient();
	deviceStatus_ = notStarted;
	lfpData_.clear();
	return deviceStatus_;
}

NeuralDataAcqInterface::deviceStatus PlexonPlugin::getDeviceStatus()
{
	if(deviceStatus_ > notStarted)
	{
		if(!PL_IsSortClientRunning())
		{
			deviceStatus_ = noData;
		}
		else
		{
			deviceStatus_ = hasData;
		}
	}
	return deviceStatus_;
}


float PlexonPlugin::samplingRate()
{
	if(!PL_IsSortClientRunning())
		return 0;

	int tick = PL_GetTimeStampTick();  //clock period in us
	return 1/(tick*1E-6);
}

QList<QSharedPointer<Picto::DataUnit>> PlexonPlugin::dumpData()
{
	QList<QSharedPointer<Picto::DataUnit>> returnList;
	QSharedPointer<Picto::NeuralDataUnit> neuralData;
	QSharedPointer<Picto::AlignmentDataUnit> alignData;
	QSharedPointer<Picto::LFPDataUnitPackage> lfpData;

	if(!PL_IsSortClientRunning())
	{
		return returnList;
	}

	//Collect the data from the Plexon Server
	PL_WaveLong *pServerEventBuffer;
	double timestampSec;
	double samplePeriodSec = PL_GetTimeStampTick()/1000000.0;  //clock period in seconds
	PL_GetSlowInfo(&freq, channels, lfpGains);
	PL_GetGain(spikeGains);
	double lfpPeriodSec = 1.0/freq;

	pServerEventBuffer = (PL_WaveLong*)malloc(sizeof(PL_WaveLong)*MAX_MAP_EVENTS_PER_READ);

	if (pServerEventBuffer != NULL)
	{
	    int NumMAPEvents = MAX_MAP_EVENTS_PER_READ;
		//double lastLFPTimestamp = -1;
		int serverDropped,mmfDropped;
		//Joey, before we were using PL_GetWaveFormStructures but the lfp waveforms
		//could be longer than the regular PL_WaveLong's allowed.  Now were using the longWaveForm structure.
		PL_GetLongWaveFormStructures(&NumMAPEvents, pServerEventBuffer,&serverDropped,&mmfDropped);
		lfpData = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());

		for(int MAPEvent=0; MAPEvent<NumMAPEvents; MAPEvent++)
		{
			//spike events
			//Spike channels are numbered with the lowest channel as 1.  Unit 0 is all units, Units 1-4 are
			//sorted units.
			if(	pServerEventBuffer[MAPEvent].Type == PL_SingleWFType && 
				pServerEventBuffer[MAPEvent].Unit >= 1 && 
				pServerEventBuffer[MAPEvent].Unit <= 4)
			{
				int spikeChan = pServerEventBuffer[MAPEvent].Channel;
				neuralData = QSharedPointer<Picto::NeuralDataUnit>(new Picto::NeuralDataUnit());
				
				timestampSec = pServerEventBuffer[MAPEvent].TimeStamp*samplePeriodSec;
				neuralData->setTimestamp(timestampSec);
				neuralData->setChannel(spikeChan);
				neuralData->setResolution(samplePeriodSec);
				int chUnit = pServerEventBuffer[MAPEvent].Unit;
				neuralData->setUnit(chUnit);

				QSharedPointer<QVector<float>> waveform(new QVector<float>);
				//waveform data
				//The preamp is typically configured to provide a gain of 1000 and the SIG gain is specified by the
				//PL_ChanHeader.Gain field which ranges from 1 to 32 in steps of 1. The following equations convert
				//the digital spike samples of the waveform back to the original analog voltage (mV):
				//For file versions <103:
				//Voltage = (sample_value)(3000)/((2048)(PL_ChanHeader.Gain)(1000))
				//For file versions >=103:
				//Voltage = (sample_value)(PL_FileHeader.SpikeMaxMagnitudeMV)/(.5(2^PL_FileHeader.BitsPerSpikeSample)(PL_ChanHeader.Gain)(1000))
				//For file version >=105:
				//Voltage = (sample_value)(PL_FileHeader.SpikeMaxMagnitudeMV)/(.5(2^PL_FileHeader.BitsPerSpikeSample)(PL_ChanHeader.Gain)(PL_FileHeader.SpikePreAmpGain))
				//	Currently, everything is being treated like file version <103
				for(int i=0; i<pServerEventBuffer[MAPEvent].NumberOfDataWords; i++)
				{
					waveform->push_back(3.0*double(pServerEventBuffer[MAPEvent].WaveForm[i])/(2048.0*spikeGains[spikeChan-1]));
				}
				neuralData->setWaveform(waveform);
				returnList.push_back(neuralData);
			}
			// eventcodes
			if(pServerEventBuffer[MAPEvent].Type == PL_ExtEventType)
			{
				alignData = QSharedPointer<Picto::AlignmentDataUnit>(new Picto::AlignmentDataUnit());
				timestampSec = pServerEventBuffer[MAPEvent].TimeStamp*samplePeriodSec;
				alignData->setTimestamp(timestampSec);
				//We maske the align code with 0x007F because we only care about the 
				//bottom 7 lines since Picto's event codes are always 0x7f and lower.
				//Anything that accidentally came in on another line should be ignored.
				alignData->setAlignCode((pServerEventBuffer[MAPEvent].Unit & 0x007F));
				returnList.push_back(alignData);
			}
			
			//LFP - Note: When using softserver with plexon files generated by tdt2plx
			//funny things happen here.  If you're testing, use a genuine plx file in softserver.
			//LFP channels in Plexon are numbered starting with zero.  This means that LFP channel zero
			//is coming from the same electrode as Spike Channel one!  Here we add 1 to the LFP channel
			//read from plexon to make everything consistant.
			if(pServerEventBuffer[MAPEvent].Type == PL_ADDataType)
			{
				timestampSec = pServerEventBuffer[MAPEvent].TimeStamp*samplePeriodSec;
				int lfpChan = pServerEventBuffer[MAPEvent].Channel+1;
				//If lfpData_ vector is too small, make it bigger
				if(lfpData_.size() < lfpChan+1)
					lfpData_.resize(lfpChan+1);

				for(int i=0; i<pServerEventBuffer[MAPEvent].NumberOfDataWords; i++)
				{
					//If the current channel's lfpDataUnit is big enough.  (ie. on the next entry it will include over 500 ms of data)
					//Add it to the list.
					if(!lfpData_[lfpChan].isNull() 
						&& (lfpData_[lfpChan]->numSamples()*lfpData_[lfpChan]->getResolution()) >= 0.5)
					{
						returnList.push_back(lfpData_[lfpChan]);
						lfpData_[lfpChan].clear();
					}


					//If this channel's lfpDataUnit hasn't been initialized, initialize it.
					if(lfpData_[lfpChan].isNull())
					{
						lfpData_[lfpChan] = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());
						lfpData_[lfpChan]->setChannel(lfpChan);
						lfpData_[lfpChan]->setResolution(lfpPeriodSec);
						lfpData_[lfpChan]->setTimestamp(timestampSec + (lfpPeriodSec*i));
					}




					//The preamp is typically configured to provide a gain of 1000 and the NI-DAQ gain is specified by the
					//PL_SlowChannelHeader.Gain field having values of 1, 2, 5, 10, 20, 50, and 100. The following equations
					//convert the digital continuous A/D samples of the waveform back to the original analog voltage (mV):
					//For file versions 100 and 101:
					//Voltage = (sample_value)(5000)/((2048)(PL_SlowChannelHeader.Gain)(1000))
					//For file version 102:
					//Voltage = (sample_value)(5000)/((2048)(Gain)(PreAmpGain))
					//where:
					//� Gain is PL_SlowChannelHeader.Gain
					//� PreAmpGain is PL_SlowChannelHeader.PreAmpGain
					//For file version 103 and greater:
					//Voltage = (sample_value)(PL_FileHeader.SlowMaxMagnitudeMV)/(.5(2^BitsPerSlowSample)(Gain)(PreampGain)
					//where:
					//� BitsPerSlowSample is PL_FileHeader.BitsPerSlowSample
					//� Gain is PL_SlowChannelHeader.Gain
					//� PreAmpGain is PL_SlowChannelHeader.PreAmpGain
					//	Currently, everything is being treated like file version 100/101
					lfpData_[lfpChan]->appendData(5.0*double(pServerEventBuffer[MAPEvent].WaveForm[i])/(2048.0*lfpGains[lfpChan-1])); 
				}
			}
		}
		//if(!dataBlock.isNull())
		//{
		//	if(lfpData->numSamples()>=5000)
		//	{
		//		returnList.push_back(lfpData);
		//		lfpData = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());
		//	}
		//	lfpData->addDataByBlock(dataBlock.data());
		//}
		//if(lfpData->numSamples())
		//	returnList.push_back(lfpData);
	}
	free(pServerEventBuffer);
	return returnList;
}

bool PlexonPlugin::acqDataAfterNow()
{
	int NumMAPEvents = MAX_MAP_EVENTS_PER_READ;
	PL_Event* pServerSkipDataBuffer = (PL_Event*)malloc(sizeof(PL_Event)*MAX_MAP_EVENTS_PER_READ);
	//Every time we call PL_GetTimeStampArrays it moves the plexon's read pointer forward
	//we just have to call it until it stops returning data to move the read pointer to
	//the current time.
	while(NumMAPEvents == MAX_MAP_EVENTS_PER_READ)
	{
		NumMAPEvents = MAX_MAP_EVENTS_PER_READ;
		PL_GetTimeStampStructures(&NumMAPEvents,pServerSkipDataBuffer);
	}
	free(pServerSkipDataBuffer);

	return true;
}

//Q_EXPORT_PLUGIN2(ProxyPluginPlexon, PlexonPlugin)