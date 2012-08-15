#include "RewardUnit.h"
#include "../../common/memleakdetect.h"

namespace Picto {


RewardUnit::RewardUnit()
:	channel_(-1),
	duration_(-1)
{
}

RewardUnit::RewardUnit(int channel, int duration)
:	channel_(channel),
	duration_(duration)
{
}

/*! \brief Turns the RewardUnitPackage into an XML fragment
 *
 *	
 */
bool RewardUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("RU");
	xmlStreamWriter->writeAttribute("c",QString("%1").arg(channel_));
	xmlStreamWriter->writeAttribute("d",QString("%1").arg(duration_));
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a RewardUnitPackage object.  Note that this deletes any existing data.
bool RewardUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "RU")
	{
		addError("RewardUnit","Incorrect tag, expected <RU>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "RU") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "RU")
		{
			if(xmlStreamReader->attributes().hasAttribute("c"))
			{
				channel_ = xmlStreamReader->attributes().value("c").toString().toInt();
			}
			else
			{
				addError("RewardUnit","Frame missing c (channel) attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("d"))
			{
				duration_ = xmlStreamReader->attributes().value("d").toString().toInt();
			}
			else
			{
				addError("RewardUnit","Frame missing d (duration) attribute",xmlStreamReader);
				return false;
			}
		}
		xmlStreamReader->readNext();
	}
	return true;
}

void RewardUnit::setDuration(int duration)
{
	duration_ = duration;
}

int RewardUnit::getDuration()
{
	return duration_;
}

int RewardUnit::getChannel()
{
	return channel_;
}

void RewardUnit::postDeserialize()
{

}

bool RewardUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
