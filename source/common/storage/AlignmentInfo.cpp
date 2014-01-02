#include "AlignmentInfo.h"
#include "../memleakdetect.h"

namespace Picto {

AlignmentInfo::AlignmentInfo()
{
}

/*! \brief Builds a AlignmentInfo object.
 *	\details There is a main timestream and a secondary time stream.  This object represents the timing of
 *	the secondary time stream.  offsetTime is the offset of the secondary timestream from the first (ie. It might
 *	have said 15 seconds when the primary said 0 seconds).  temporalFactor is the factor by which the primary
 *	time stream needs to be multipled in order to align it with the secondary time stream (believe it or not
 *	this is sometimes an issue, particularly with the Pictobox Nidaq card, although we handle that differently since
 *	it doesn't except event codes).
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
		addError("Incorrect tag, expected <AlignmentInfo>");
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("OffsetTime"))
	{
		offsetTime_ = xmlStreamReader->attributes().value("OffsetTime").toString().toDouble();
	}
	else
	{
		addError("Data missing OffsetTime attribute");
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("TemporalFactor"))
	{
		temporalFactor_ = xmlStreamReader->attributes().value("TemporalFactor").toString().toDouble();
	}
	else
	{
		addError("Data missing TemporalFactor attribute");
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
