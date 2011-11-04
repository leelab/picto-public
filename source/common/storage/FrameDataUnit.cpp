#include "FrameDataUnitPackage.h"

namespace Picto {


FrameDataUnit::FrameDataUnit()
{
}

FrameDataUnit::FrameDataUnit(int frameNum, QString timestamp, QString statename)
{
	frameNumber = frameNum;
	time = timestamp;
	stateName = statename;
}

FrameDataUnit::FrameDataUnit(int frameNum, double timestamp, QString statename)
{
	frameNumber = frameNum;
	time = QString("%1").arg(timestamp,0,'f',6);
	stateName = statename;
}

/*! \brief Turns the FrameDataUnitPackage into an XML fragment
 *
 *	The XML will look like this:
 *	<FrameDataUnit time=1.234 state=somestate>1</FrameDataUnit>
 *	
 */
bool FrameDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("FrameDataUnit");
	xmlStreamWriter->writeAttribute("time",time);
	xmlStreamWriter->writeAttribute("state",QString("%1").arg(stateName));
	xmlStreamWriter->writeTextElement("framenumber",QString::number(frameNumber));
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a FrameDataUnitPackage object.  Note that this deletes any existing data.
bool FrameDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "FrameDataUnit")
	{
		addError("FrameDataUnit","Incorrect tag, expected <FrameDataUnit>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "FrameDataUnit") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "FrameDataUnit")
		{
			if(xmlStreamReader->attributes().hasAttribute("time"))
			{
				time = xmlStreamReader->attributes().value("time").toString();
			}
			else
			{
				addError("FrameDataUnit","Frame missing time attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("state"))
			{
				stateName = xmlStreamReader->attributes().value("state").toString();
			}
			else
			{
				addError("FrameDataUnit","Frame missing state attribute",xmlStreamReader);
				return false;
			}
		}
		else if(name == "framenumber")
		{
			bool ok;
			frameNumber = xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
			{
				addError("FrameDataUnit","Frame number not an integer",xmlStreamReader);
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
