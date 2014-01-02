#include "RewardDataUnit.h"
#include "../memleakdetect.h"

namespace Picto {

RewardDataUnit::RewardDataUnit() :
		durationMs_(-1),
		channel_(-1),
		time_(-1)
{
}

/*! \brief Constructs a RewardDataUnit object.
 *	@param durationMs The duration of the reward delivery in milliseconds.
 *	@param channel The reward channel on which the reward was supplied.
 *	@param time The time at which reward delivery began.
 */
RewardDataUnit::RewardDataUnit(int durationMs, int channel, double time) :
		durationMs_(durationMs),
		channel_(channel),
		time_(QString("%1").arg(time,0,'f',14))
{
}

/*! \brief Turns the RewardDataUnit into an XML fragment
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
/*! \brief Converts XML into a RewardDataUnit object.  Note that this deletes any existing data.*/
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
		addError("Incorrect tag, expected <RDU>");
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
					addError("<RewardDataUnit> tag missing t (timestamp) attribute");
					return false;
				}
			}

			QString name = xmlStreamReader->name().toString();
			if(xmlStreamReader->attributes().hasAttribute("c"))
			{
				channel_ = xmlStreamReader->attributes().value("c").toString().toInt(&ok);
				if(!ok)
				{
					addError("<c> (Channel) value not an integer.");
					return false;
				}
			}
			
			if(xmlStreamReader->attributes().hasAttribute("d"))
			{
				durationMs_ = xmlStreamReader->attributes().value("d").toString().toInt(&ok);
				if(!ok)
				{
					addError("<d> (duration) value not an integer.");
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
		addError("<c> (Channel) value not found or invalid");
		return false;
	}

	if( durationMs_ < 0)
	{
		addError("<d> (duration) value not found or invalid");
		return false;
	}

	if( time_.toDouble() < 0)
	{
		addError( "t (timestamp) attribute not found or invalid");
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
