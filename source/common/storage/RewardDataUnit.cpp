#include "RewardDataUnit.h"

namespace Picto {

RewardDataUnit::RewardDataUnit() :
		durationMs_(-1),
		channel_(-1),
		time_(-1)
{
}

RewardDataUnit::RewardDataUnit(int durationMs, int channel, double time) :
		durationMs_(durationMs),
		channel_(channel),
		time_(time)
{
}



/*! \brief Turns the RewardDataUnit into an XML fragment
 *
 *	The XML will look like this:
 *	<RewardDataUnit timestamp = 123.234>
 *		<Channel> 1</Channel>
 *		<Duration>25</Duration>
 *	</RewardDataUnit>
 *	
 */
bool RewardDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("RewardDataUnit");
	xmlStreamWriter->writeAttribute("timestamp",QString("%1").arg(time_,0,'e',6));

	xmlStreamWriter->writeTextElement("Channel",QString::number(channel_));
	xmlStreamWriter->writeTextElement("Duration",QString::number(durationMs_));
	DataUnit::serializeDataID(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //RewardDataUnit

	return true;
}
//! Converts XML into a RewardDataUnit object.  Note that this deletes any existing data.
bool RewardDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Clear exiting data
	time_ = -1;
	channel_ = -1;
	durationMs_ = -1;

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "RewardDataUnit")
	{
		addError("RewardDataUnit","Incorrect tag, expected <RewardDataUnit>",xmlStreamReader);
		return false;
	}

	//read the timestamp
	bool ok;
	time_ = xmlStreamReader->attributes().value("timestamp").toString().toDouble(&ok);
	if(!ok || time_ < 0)
	{
		addError("RewardDataUnit","<RewardDataUnit> tag missing timestamp attribute",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "RewardDataUnit") && !xmlStreamReader->atEnd())
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
				addError("RewardDataUnit","<Channel> value not an integer.",xmlStreamReader);
				return false;
			}
		}
		else if(name == "Duration")
		{
			durationMs_ = xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
			{
				addError("RewardDataUnit","<Duration> value not an integer.",xmlStreamReader);
				return false;
			}
			
		}
		else
		{
			DataUnit::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("RewardDataUnit", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	if( channel_ < 0)
	{
		addError("RewardDataUnit", "<Channel> value not found or invalid", xmlStreamReader);
		return false;
	}

	if( durationMs_ < 0)
	{
		addError("RewardDataUnit", "<Duration> value not found or invalid", xmlStreamReader);
		return false;
	}

	if( time_ < 0)
	{
		addError("RewardDataUnit", "timestamp attribute not found or invalid", xmlStreamReader);
		return false;
	}


	return true;
}

bool RewardDataUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
