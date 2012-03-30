#include "AlignmentInfo.h"
#include "../memleakdetect.h"

namespace Picto {

AlignmentInfo::AlignmentInfo()
{
}

/*! \brief Builds a new signal channel info object.
 *	@param sourceName The name of the source of the data that this information will align.
 *	@param offsetTime The name of the table associated with the signal channel
 *	@param temporalFactor The State Variable ID associated with the signal channel
 */
AlignmentInfo::AlignmentInfo(	double offsetTime,
								double temporalFactor)
{
	offsetTime_ = offsetTime;
	temporalFactor_ = temporalFactor;
}


/*! \brief Turns the AlignmentInfo into an XML fragment
 */
bool AlignmentInfo::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("AlignmentInfo");
	xmlStreamWriter->writeAttribute("OffsetTime",QString::number(offsetTime_,'e',14));
	xmlStreamWriter->writeAttribute("TemporalFactor",QString::number(temporalFactor_,'e',14));
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a AlignmentInfo object.  Note that this deletes any existing data.
bool AlignmentInfo::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "AlignmentInfo")
	{
		addError("AlignmentInfo","Incorrect tag, expected <AlignmentInfo>",xmlStreamReader);
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("OffsetTime"))
	{
		offsetTime_ = xmlStreamReader->attributes().value("OffsetTime").toString().toDouble();
	}
	else
	{
		addError("AlignmentInfo","Data missing OffsetTime attribute",xmlStreamReader);
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("TemporalFactor"))
	{
		temporalFactor_ = xmlStreamReader->attributes().value("TemporalFactor").toString().toDouble();
	}
	else
	{
		addError("AlignmentInfo","Data missing TemporalFactor attribute",xmlStreamReader);
		return false;
	}


	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "AlignmentInfo") && !xmlStreamReader->atEnd())
	{
		xmlStreamReader->readNext();
	}
	return true;
}

void AlignmentInfo::postDeserialize()
{

}

bool AlignmentInfo::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
