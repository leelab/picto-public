#include <QString>
#include <QByteArray>
#include <QXmlStreamReader>

#include <QTextStream>

#include "ENGEVENTCommandHandler.h"


ENGEVENTCommandHandler::ENGEVENTCommandHandler()
{
}

//The command handler for engine events takes information from the engine, 
//and updates the front panel software.  Generally speaking, incoming events
//will result in some sort of change on the status display (assuming that the
//status display is on).  Given the speed at which events are generated, I am
//erring on the side of caution when it comes to processing these events.  For
//example, a trial start event does not change the display other than simply
//resetting the trial counter (otherwise, the display would be a constant
//stream of trialstart/reward/trialend...).

//Most of the panel info functions are self explanatory, but the "lastEvent"
//deserves some explanation. LastEvent is the string that will be displayed on 
//the second line of the status display.  For the moment, that string will
//rarely be changed (again trying to keep the display clean).  With real
//experiments, it is likely that the users will use it to display status info.
QSharedPointer<Picto::ProtocolResponse> ENGEVENTCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QTextStream out(stdout);

	QByteArray xmlData = command->getContent();
	QXmlStreamReader reader(xmlData);


	//We can't rely on atEnd here, because the reader will return 
	//PrematureEndOfDocumentError (since this isn't a full document)
	while(!reader.atEnd() && reader.error()==0)
	{
		reader.readNext();
		if(reader.isStartElement() && reader.name() == "event")
		{
			QString eventType = reader.attributes().value("type").toString();
			eventType = eventType.toLower();

			//NOTE: all event types are in lower case.
			if(eventType=="experimentstart")
			{
				panelInfo->setLastEvent("Experiment start");
			}
			else if(eventType=="experimentend")
			{
				panelInfo->setLastEvent("Experiment end");
			}
			else if(eventType=="blockstart")
			{
				int block = reader.attributes().value("block").toString().toInt();
				panelInfo->setBlock(block);
			}
			else if(eventType=="blockend")
			{
				int block = reader.attributes().value("block").toString().toInt();
				panelInfo->setBlock(block);
			}
			else if(eventType=="trialstart")
			{
				int trial = reader.attributes().value("trial").toString().toInt();
				panelInfo->setTrial(trial);
			}
			else if(eventType=="trialend")
			{
				int trial = reader.attributes().value("trial").toString().toInt();
				panelInfo->setTrial(trial);

				//clear the "last event" at the end of each trial
				panelInfo->setLastEvent("");
			}
			else if(eventType=="statuschange")
			{
				QString status = reader.attributes().value("status").toString();
				status = status.toLower();
				if(status == "connected")
					panelInfo->setSystemStatus(PanelInfo::connected);
				else if(status == "disconnected")
					panelInfo->setSystemStatus(PanelInfo::disconnected);
				else if(status == "running")
				{
					panelInfo->setSystemStatus(PanelInfo::running);

					//go into status display mode
					panelInfo->setDispMode(PanelInfo::StatusMode);
				}
			}
			else if(eventType=="other")
			{
				panelInfo->setLastEvent(reader.readElementText());
			}
			else
			{
				//bad event type, do nothing
			}
		}
		else
		{
			//do nothing
		}
	}

	//return a null pointer (since ENGEVENTs are never responded to)
	return QSharedPointer<Picto::ProtocolResponse>();
}
