/*! \file plexonplugin.cpp
 * \ingroup proxypluginplexon
 * \brief A plugin allowing the proxy server to interface with a Plexon MAP
 */

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

QString PlexonPlugin::dumpData()
{
	QString xmlData;
	QXmlStreamWriter writer(&xmlData);

	writer.setAutoFormatting(true);

	if(!PL_IsSortClientRunning())
	{
		return QString::null;
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

		//number of events
		writer.writeStartElement("numEvents");
		writer.writeCharacters(QString("%1").arg(NumMAPEvents));
		writer.writeEndElement();

		for(int MAPEvent=0; MAPEvent<NumMAPEvents; MAPEvent++)
		{

			//spike events
			if(	pServerEventBuffer[MAPEvent].Type == PL_SingleWFType && 
				pServerEventBuffer[MAPEvent].Unit >= 1 && 
				pServerEventBuffer[MAPEvent].Unit <= 4)
			{
				writer.writeStartElement("event");
				writer.writeAttribute("type", "spike");

				timestampSec = pServerEventBuffer[MAPEvent].TimeStamp*samplePeriodSec;
				writer.writeTextElement("timestamp",QString("%1").arg(timestampSec,0,'f',4));

				writer.writeTextElement("channel",QString("%1").arg(pServerEventBuffer[MAPEvent].Channel));

				char chUnit = 'a'+pServerEventBuffer[MAPEvent].Unit-1;
				writer.writeTextElement("unit",QString("%1").arg(chUnit));

				//waveform data
				writer.writeStartElement("wave");
				for(int i=0; i<pServerEventBuffer[MAPEvent].NumberOfDataWords; i++)
				{
					writer.writeCharacters(QString("%1 ").arg(pServerEventBuffer[MAPEvent].WaveForm[i]));
				}
				writer.writeEndElement();
				

				writer.writeEndElement(); //end event
			}
			// eventcodes
			if(pServerEventBuffer[MAPEvent].Type == PL_ExtEventType)
			{
				writer.writeStartElement("event");
				writer.writeAttribute("type","externalEvent");

				timestampSec = pServerEventBuffer[MAPEvent].TimeStamp*samplePeriodSec;
				writer.writeTextElement("timestamp",QString("%1").arg(timestampSec,0,'f',4));

				writer.writeTextElement("eventCode",QString("%1").arg(pServerEventBuffer[MAPEvent].Unit));

				writer.writeEndElement(); //event


			}
		}
	}

	free(pServerEventBuffer);


	return xmlData;
}

Q_EXPORT_PLUGIN2(ProxyPluginPlexon, PlexonPlugin)