#include "ACQGetCommandHandler.h"

#include <QDateTime>
#include <QNetworkInterface>
#include <QXmlStreamWriter>


#include "../../common/globals.h"

#include <windows.h>
#include "plexon.h"

#define MAX_MAP_EVENTS_PER_READ 5000

//! Command handler for the ACQ protocol's GET command

/*!
 *  See the Trac wiki for a better description of this protocol. 
 *
 *  NOTE: The protocol grabs the full waveforms from the Plexon every single time.
 *  This may be a waste of time/data if we aren't using the waveforms, but given the
 *  way the Plexon server works (only giving us the data since the last time we
 *  asked for data), this seems to make the most sense.
 */

ACQGetCommandHandler::ACQGetCommandHandler()
{
}

QSharedPointer<Picto::ProtocolResponse> ACQGetCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse("ACQ","1.0",Picto::ProtocolResponseType::OK));
	
	//set up XML writer
	QString xmlData;
	QXmlStreamWriter writer(&xmlData);

	writer.setAutoFormatting(true);
	writer.writeStartDocument();

	//Start writing the XML document
	writer.writeStartElement("device");
	writer.writeCharacters("Plexon");
	writer.writeEndElement();

	//check to see if the Plexon connection is up and running.  If it is, great!  
	//Otherwise set it up, and send a response indicating whether the setup was successful 
	writer.writeStartElement("PlexonServerStatus");

	if(!PL_IsSortClientRunning())
	{
		PL_CloseClient();
		Sleep(1000);
		PL_InitClientEx3(0, NULL, NULL);

		if(!PL_IsSortClientRunning())
		{
			writer.writeCharacters("Failed to start");
			writer.writeEndElement();
			writer.writeEndDocument();
	
			response->setContent(xmlData.toUtf8());

			return response;
		}
		else
		{
			writer.writeCharacters("Started");
		}

	}
	else
	{
		writer.writeCharacters("running");
	}
	writer.writeEndElement(); //PlexonServerStatus


	//sampling rate
	if(command->getFieldValue("rate") == "1" || command->getFieldValue("all") == "1")
	{
		int tick = PL_GetTimeStampTick();  //clock period in us
		float sampleRate = 1/(tick*1E-6);

		writer.writeStartElement("sampleRate");
		writer.writeCharacters(QString("%1").arg(sampleRate));
		writer.writeEndElement();
	}


	if(command->hasField("timestamps") || command->hasField("wave") || command->hasField("all"))
	{
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
				if((command->getFieldValue("timestamps") == "all" ||
					command->getFieldValue("timestamps") == "spike" ||
					command->getFieldValue("all") == "1") &&
					pServerEventBuffer[MAPEvent].Type == PL_SingleWFType && 
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
					if(command->getFieldValue("wave") == "spike" ||
						command->getFieldValue("all") == "1")
					{
						writer.writeStartElement("wave");
						for(int i=0; i<pServerEventBuffer[MAPEvent].NumberOfDataWords; i++)
						{
							writer.writeCharacters(QString("%1 ").arg(pServerEventBuffer[MAPEvent].WaveForm[i]));
						}
						writer.writeEndElement();
					}

					writer.writeEndElement(); //end event
				}
				// eventcodes
				if((command->getFieldValue("timestamps") == "all" ||
					command->getFieldValue("timestamps") == "events" ||
					command->getFieldValue("all") == "1") &&
					pServerEventBuffer[MAPEvent].Type == PL_ExtEventType)
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
	}

	
	writer.writeEndDocument();
	
	response->setContent(xmlData.toUtf8());

	return response;
}
