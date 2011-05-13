#include "FrameDataStore.h"

namespace Picto {


FrameUnitDataStore::FrameUnitDataStore()
{
}
FrameUnitDataStore::FrameUnitDataStore(int frameNum, double timestamp, QString statename)
{
	frameNumber = frameNum;
	time = timestamp; 
	stateName = statename;
}

/*! \brief Turns the FrameDataStore into an XML fragment
 *
 *	The XML will look like this:
 *	<FrameUnitDataStore time=1.234 state=somestate>1</FrameUnitDataStore>
 *	
 */
bool FrameUnitDataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("FrameUnitDataStore");
	xmlStreamWriter->writeAttribute("time",QString("%1").arg(time));
	xmlStreamWriter->writeAttribute("state",QString("%1").arg(stateName));
	xmlStreamWriter->writeTextElement("framenumber",QString::number(frameNumber));
	DataStore::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a FrameDataStore object.  Note that this deletes any existing data.
bool FrameUnitDataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "FrameUnitDataStore")
	{
		addError("FrameUnitDataStore","Incorrect tag, expected <FrameUnitDataStore>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "FrameUnitDataStore") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "FrameUnitDataStore")
		{
			if(xmlStreamReader->attributes().hasAttribute("time"))
			{
				time = xmlStreamReader->attributes().value("time").toString().toDouble();
			}
			else
			{
				addError("FrameUnitDataStore","Frame missing time attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("state"))
			{
				stateName = xmlStreamReader->attributes().value("state").toString();
			}
			else
			{
				addError("FrameUnitDataStore","Frame missing state attribute",xmlStreamReader);
				return false;
			}
		}
		else if(name == "framenumber")
		{
			bool ok;
			frameNumber = xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
			{
				addError("FrameUnitDataStore","Frame number not an integer",xmlStreamReader);
				return false;
			}
		}
		else
		{
			DataStore::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}
	return true;
}


}; //namespace Picto
