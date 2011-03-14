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

QList<QSharedPointer<Picto::DataStore>> PlexonPlugin::dumpData()
{
	QList<QSharedPointer<Picto::DataStore>> returnList;
	QSharedPointer<Picto::NeuralDataStore> neuralData;
	QSharedPointer<Picto::AlignmentDataStore> alignData;

	if(!PL_IsSortClientRunning())
	{
		return returnList;
	}

	//Collect the data from the Plexon Server
	PL_Wave*     pServerEventBuffer;
	double timestampSec;
	double samplePeriodSec = PL_GetTimeStampTick()/1000000.0;  //clock period in seconds

	pServerEventBuffer = (PL_Wave*)malloc(sizeof(PL_Wave)*MAX_MAP_EVENTS_PER_READ);

	if (pServerEventBuffer != NULL)
	{
	    int NumMAPEvents = MAX_MAP_EVENTS_PER_READ;
		PL_GetWaveFormStructures(&NumMAPEvents, pServerEventBuffer);

		for(int MAPEvent=0; MAPEvent<NumMAPEvents; MAPEvent++)
		{
			//spike events
			if(	pServerEventBuffer[MAPEvent].Type == PL_SingleWFType && 
				pServerEventBuffer[MAPEvent].Unit >= 1 && 
				pServerEventBuffer[MAPEvent].Unit <= 4)
			{
				neuralData = QSharedPointer<Picto::NeuralDataStore>(new Picto::NeuralDataStore());
				
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
				alignData = QSharedPointer<Picto::AlignmentDataStore>(new Picto::AlignmentDataStore());
				timestampSec = pServerEventBuffer[MAPEvent].TimeStamp*samplePeriodSec;
				alignData->setTimestamp(timestampSec);
				alignData->setAlignCode(pServerEventBuffer[MAPEvent].Unit);
				returnList.push_back(alignData);
			}
		}
	}
	free(pServerEventBuffer);
	return returnList;
}

Q_EXPORT_PLUGIN2(ProxyPluginPlexon, PlexonPlugin)