#include <QXmlStreamWriter>
#include <windows.h>
#include "plexon.h"
#include "plexonplugin.h"

QString PlexonPlugin::device() const
{
	return "Plexon";
}


NeuralDataAcqInterface::deviceStatus PlexonPlugin::startDevice()
{
	PL_CloseClient();
	PL_InitClientEx3(0, NULL, NULL);

	if(PL_IsSortClientRunning())
	{
		return started;
	}
	else
	{
		return failedToStart;
	}
}

NeuralDataAcqInterface::deviceStatus PlexonPlugin::stopDevice()
{
	PL_CloseClient();

	return stopped;
}

NeuralDataAcqInterface::deviceStatus PlexonPlugin::getDeviceStatus()
{
	deviceStatus status;

	if(!PL_IsSortClientRunning())
	{
		status = stopped;
	}
	else
	{
		status = running;
	}
	return status;
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
	PL_Wave*     pServerEventBuffer;
	double timestampSec;
	double samplePeriodSec = PL_GetTimeStampTick()/1000000.0;  //clock period in seconds
	int freq, dummy[16];
	PL_GetSlowInfo(&freq, dummy, dummy); //** last two params are unused here
	double lfpPeriodSec = 1.0/freq;

	pServerEventBuffer = (PL_Wave*)malloc(sizeof(PL_Wave)*MAX_MAP_EVENTS_PER_READ);

	if (pServerEventBuffer != NULL)
	{
	    int NumMAPEvents = MAX_MAP_EVENTS_PER_READ;
		double lastLFPTimestamp = -1;
		PL_GetWaveFormStructures(&NumMAPEvents, pServerEventBuffer);
		QSharedPointer<Picto::lfpDataBlock> dataBlock;
		lfpData = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());

		for(int MAPEvent=0; MAPEvent<NumMAPEvents; MAPEvent++)
		{
			//spike events
			if(	pServerEventBuffer[MAPEvent].Type == PL_SingleWFType && 
				pServerEventBuffer[MAPEvent].Unit >= 1 && 
				pServerEventBuffer[MAPEvent].Unit <= 4)
			{
				neuralData = QSharedPointer<Picto::NeuralDataUnit>(new Picto::NeuralDataUnit());
				
				timestampSec = pServerEventBuffer[MAPEvent].TimeStamp*samplePeriodSec;
				neuralData->setTimestamp(timestampSec);
				neuralData->setChannel(pServerEventBuffer[MAPEvent].Channel);
				char chUnit = 'a'+pServerEventBuffer[MAPEvent].Unit-1;
				neuralData->setUnit(chUnit);

				QSharedPointer<QList<int>> waveform(new QList<int>);
				//waveform data
				for(int i=0; i<pServerEventBuffer[MAPEvent].NumberOfDataWords; i++)
				{
					waveform->push_back(pServerEventBuffer[MAPEvent].WaveForm[i]);
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
				alignData->setAlignCode(pServerEventBuffer[MAPEvent].Unit);
				returnList.push_back(alignData);
			}
			// lfp UNTESTED!!!!!!!!!! I HAVE NO IDEA IF THIS IS READING THE CORRECT DATA
			if(pServerEventBuffer[MAPEvent].Type == PL_ADDataType)
			{
				timestampSec = pServerEventBuffer[MAPEvent].TimeStamp*samplePeriodSec;
				if(timestampSec != lastLFPTimestamp)
				{
					if(!dataBlock.isNull())
					{
						if(lfpData->numSamples()>=5000)
						{
							returnList.push_back(lfpData);
							lfpData = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());
						}
						lfpData->addDataByBlock(dataBlock.data());
					}
					dataBlock = QSharedPointer<Picto::lfpDataBlock>(new Picto::lfpDataBlock(timestampSec,lfpPeriodSec));
					lastLFPTimestamp = timestampSec;
				}
				for(int i=0; i<pServerEventBuffer[MAPEvent].NumberOfDataWords; i++)
				{
					//Below, we use the values returned from Plexon as follows:
					//1. +or- 5000000 is the operating range of the A/D converter in uV.  
					//2. + or - 2048 is the A/D resolution.  
					//3. There is a preamp gain of 1000x that seems to be stored in the plexon file and was possibly entered manually by us at some point.
					//		This may be due to a thomas recording preamp gain of 20X and the "blue box" preamp which seems to at least in some cases
					//		by 50X.  In any case, assuming that this value is accurate, it brings the 5000000 down to 5000.
					//4. The value entered should be the voltage in uV assuming that there are no other gains in the system that we didn't include here.
					//The results of all this were compared to results in neuroexplorer and to values returned from Orion for a single file and things matched up.  
					//It does seem like older versions of plexon may have had different A/D operating ranges and/or preamp gains.
					//We really need to just run a test to figure out what the precise values are here...
					dataBlock->data[pServerEventBuffer[MAPEvent].Channel].push_back(5000.0*double(pServerEventBuffer[MAPEvent].WaveForm[i])/2048.0); 
				}
			}
		}
		if(!dataBlock.isNull())
		{
			if(lfpData->numSamples()>=5000)
			{
				returnList.push_back(lfpData);
				lfpData = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());
			}
			lfpData->addDataByBlock(dataBlock.data());
		}
		if(lfpData->numSamples())
			returnList.push_back(lfpData);
	}
	free(pServerEventBuffer);
	return returnList;
}

Q_EXPORT_PLUGIN2(ProxyPluginPlexon, PlexonPlugin)