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
	return "plexon";
}

QString PlexonPlugin::deviceName()
{
	return "Plexon";
}
QString PlexonPlugin::deviceStatus()
{
	QString status;

	if(!PL_IsSortClientRunning())
	{
		PL_CloseClient();
		PL_InitClientEx3(0, NULL, NULL);

		if(!PL_IsSortClientRunning())
		{
			status = "Failed to start";
		}
		else
		{
			status = "Started";
		}

	}
	else
	{
		status = "Running";
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

				writer.writeStartElement("timestamp");
				writer.writeCharacters(QString("%1").arg(pServerEventBuffer[MAPEvent].TimeStamp));
				writer.writeEndElement();

				writer.writeStartElement("eventType");
				writer.writeCharacters("spike");
				writer.writeEndElement();

				writer.writeStartElement("channel");
				writer.writeCharacters(QString("%1").arg(pServerEventBuffer[MAPEvent].Channel));
				writer.writeEndElement();

				writer.writeStartElement("unit");
				char chUnit = 'a'+pServerEventBuffer[MAPEvent].Unit-1;
				writer.writeCharacters(QString("%1").arg(chUnit));
				writer.writeEndElement();

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

				writer.writeStartElement("timestamp");
				writer.writeCharacters(QString("%1").arg(pServerEventBuffer[MAPEvent].TimeStamp));
				writer.writeEndElement();

				writer.writeStartElement("eventType");
				writer.writeCharacters("external event");
				writer.writeEndElement();

				writer.writeStartElement("eventCode");
				writer.writeCharacters(QString("%1").arg(pServerEventBuffer[MAPEvent].Unit));
				writer.writeEndElement();

				writer.writeEndElement(); //event


			}
		}
	}

	free(pServerEventBuffer);


	return xmlData;
}

Q_EXPORT_PLUGIN2(ProxyPluginPlexon, PlexonPlugin)