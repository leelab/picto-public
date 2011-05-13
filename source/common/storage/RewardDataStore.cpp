#include "RewardDataStore.h"

namespace Picto {

RewardDataStore::RewardDataStore() :
		durationMs_(-1),
		channel_(-1),
		time_(-1)
{
}

RewardDataStore::RewardDataStore(int durationMs, int channel, double time) :
		durationMs_(durationMs),
		channel_(channel),
		time_(time)
{
}



/*! \brief Turns the RewardDataStore into an XML fragment
 *
 *	The XML will look like this:
 *	<RewardDataStore timestamp = 123.234>
 *		<Channel> 1</Channel>
 *		<Duration>25</Duration>
 *	</RewardDataStore>
 *	
 */
bool RewardDataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("RewardDataStore");
	xmlStreamWriter->writeAttribute("timestamp",QString("%1").arg(time_));

	xmlStreamWriter->writeTextElement("Channel",QString::number(channel_));
	xmlStreamWriter->writeTextElement("Duration",QString::number(durationMs_));
	DataStore::serializeDataID(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //RewardDataStore

	return true;
}
//! Converts XML into a RewardDataStore object.  Note that this deletes any existing data.
bool RewardDataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Clear exiting data
	time_ = -1;
	channel_ = -1;
	durationMs_ = -1;

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "RewardDataStore")
	{
		addError("RewardDataStore","Incorrect tag, expected <RewardDataStore>",xmlStreamReader);
		return false;
	}

	//read the timestamp
	bool ok;
	time_ = xmlStreamReader->attributes().value("timestamp").toString().toDouble(&ok);
	if(!ok || time_ < 0)
	{
		addError("RewardDataStore","<RewardDataStore> tag missing timestamp attribute",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "RewardDataStore") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Channel")
		{
			channel_ = xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
			{
				addError("RewardDataStore","<Channel> value not an integer.",xmlStreamReader);
				return false;
			}
		}
		else if(name == "Duration")
		{
			durationMs_ = xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
			{
				addError("RewardDataStore","<Duration> value not an integer.",xmlStreamReader);
				return false;
			}
			
		}
		else
		{
			DataStore::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("RewardDataStore", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	if( channel_ < 0)
	{
		addError("RewardDataStore", "<Channel> value not found or invalid", xmlStreamReader);
		return false;
	}

	if( durationMs_ < 0)
	{
		addError("RewardDataStore", "<Duration> value not found or invalid", xmlStreamReader);
		return false;
	}

	if( time_ < 0)
	{
		addError("RewardDataStore", "timestamp attribute not found or invalid", xmlStreamReader);
		return false;
	}


	return true;
}


}; //namespace Picto
