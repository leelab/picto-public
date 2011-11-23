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
		time_(QString("%1").arg(time,0,'f',6))
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
	xmlStreamWriter->writeStartElement("RDU");
	xmlStreamWriter->writeAttribute("t",time_);
	xmlStreamWriter->writeAttribute("c",QString::number(channel_));
	xmlStreamWriter->writeAttribute("d",QString::number(durationMs_));
	DataUnit::serializeDataID(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //RewardDataUnit

	return true;
}
//! Converts XML into a RewardDataUnit object.  Note that this deletes any existing data.
bool RewardDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	bool ok;
	//Clear exiting data
	time_ = "-1.0";
	channel_ = -1;
	durationMs_ = -1;

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "RDU")
	{
		addError("RewardDataUnit","Incorrect tag, expected <RDU>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "RDU") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "RDU")
		{
			//read the timestamp
			if(xmlStreamReader->attributes().hasAttribute("t"))
			{
				time_ = xmlStreamReader->attributes().value("t").toString();
				if(time_.toDouble() < 0)
				{
					addError("RewardDataUnit","<RewardDataUnit> tag missing t (timestamp) attribute",xmlStreamReader);
					return false;
				}
			}

			QString name = xmlStreamReader->name().toString();
			if(xmlStreamReader->attributes().hasAttribute("c"))
			{
				channel_ = xmlStreamReader->attributes().value("c").toString().toInt(&ok);
				if(!ok)
				{
					addError("RewardDataUnit","<c> (Channel) value not an integer.",xmlStreamReader);
					return false;
				}
			}
			
			if(xmlStreamReader->attributes().hasAttribute("d"))
			{
				durationMs_ = xmlStreamReader->attributes().value("d").toString().toInt(&ok);
				if(!ok)
				{
					addError("RewardDataUnit","<d> (duration) value not an integer.",xmlStreamReader);
					return false;
				}
				
			}
		}
		else
		{
			DataUnit::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}

	if( channel_ < 0)
	{
		addError("RewardDataUnit", "<c> (Channel) value not found or invalid", xmlStreamReader);
		return false;
	}

	if( durationMs_ < 0)
	{
		addError("RewardDataUnit", "<d> (duration) value not found or invalid", xmlStreamReader);
		return false;
	}

	if( time_.toDouble() < 0)
	{
		addError("RewardDataUnit", "t (timestamp) attribute not found or invalid", xmlStreamReader);
		return false;
	}


	return true;
}

void RewardDataUnit::postDeserialize()
{

}

bool RewardDataUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
