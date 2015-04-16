#include "RewardUnit.h"
#include "../../common/memleakdetect.h"

namespace Picto {


RewardUnit::RewardUnit()
:	channel_(-1),
	duration_(-1)
{
}

/*! \brief Constructs a RewardUnit object.
 *	@param duration The duration of the reward delivery in milliseconds.
 *	@param channel The reward channel for which this is the default duration.
 */
RewardUnit::RewardUnit(int channel, int duration)
:	channel_(channel),
	duration_(duration)
{
}

/*! \brief Turns the RewardUnit into an XML fragment
 */
bool RewardUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("RU");
	xmlStreamWriter->writeAttribute("c",QString("%1").arg(channel_));
	xmlStreamWriter->writeAttribute("d",QString("%1").arg(duration_));
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a RewardUnit object.  Note that this deletes any existing data.
bool RewardUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "RU")
	{
		addError("Incorrect tag, expected <RU>");
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
				addError("Frame missing c (channel) attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("d"))
			{
				duration_ = xmlStreamReader->attributes().value("d").toString().toInt();
			}
			else
			{
				addError("Frame missing d (duration) attribute");
				return false;
			}
		}
		xmlStreamReader->readNext();
	}
	return true;
}

/*! \brief Sets the input number of milliseconds as the default duration for this RewardUnit.*/
void RewardUnit::setDuration(int duration)
{
	duration_ = duration;
}

/*! \brief Gets the number of milliseconds of reward for this RewardUnit.*/
int RewardUnit::getDuration()
{
	return duration_;
}

/*! \brief Gets the channel for which this RewardUnit stores the default reward duration.*/
int RewardUnit::getChannel()
{
	return channel_;
}

}; //namespace Picto
