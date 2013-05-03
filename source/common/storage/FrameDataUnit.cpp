#include "FrameDataUnitPackage.h"
#include "../memleakdetect.h"

namespace Picto {


FrameDataUnit::FrameDataUnit()
{
}

FrameDataUnit::FrameDataUnit(QString timestamp, int stateId)
{
	time = timestamp;
	stateId_ = stateId;
}

FrameDataUnit::FrameDataUnit(double timestamp, int stateId)
{
	time = QString("%1").arg(timestamp,0,'f',14);
	stateId_ = stateId;
}

/*! \brief Turns the FrameDataUnitPackage into an XML fragment
 *
 *	The XML will look like this:
 *	<FrameDataUnit time=1.234 state=somestate>1</FrameDataUnit>
 *	
 */
bool FrameDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("FDU");
	xmlStreamWriter->writeAttribute("t",time);
	xmlStreamWriter->writeAttribute("s",QString("%1").arg(stateId_));
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a FrameDataUnitPackage object.  Note that this deletes any existing data.
bool FrameDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "FDU")
	{
		addError("Incorrect tag, expected <FDU>");
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "FDU") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "FDU")
		{
			if(xmlStreamReader->attributes().hasAttribute("t"))
			{
				time = xmlStreamReader->attributes().value("t").toString();
			}
			else
			{
				addError("Frame missing t (time) attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("s"))
			{
				stateId_ = xmlStreamReader->attributes().value("s").toString().toInt();
			}
			else
			{
				addError("Frame missing s (state) attribute");
				return false;
			}
		}
		else
		{
			DataUnit::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}
	return true;
}

void FrameDataUnit::postDeserialize()
{

}

bool FrameDataUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
