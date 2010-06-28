#include "FrameDataStore.h"

namespace Picto {

FrameDataStore::FrameDataStore()
{
}

void FrameDataStore::addFrame(int frameNumber, double time, QString stateName)
{
	FrameData data = {frameNumber, time, stateName};
	data_.append(data);
}



/*! \brief Turns the FrameDataStore into an XML fragment
 *
 *	The XML will look like this:
 *	<FrameDataStore>
 *		<Frame timestamp=1.234 state=somestate>1</Frame>
 *		<Frame timestamp=1.434 state=somestate>2</Frame>
 *		...
 *	</FrameDataStore>
 *	
 */
bool FrameDataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("FrameDataStore");

	foreach(FrameData data, data_)
	{
		xmlStreamWriter->writeStartElement("Frame");
		xmlStreamWriter->writeAttribute("time",QString("%1").arg(data.time));
		xmlStreamWriter->writeAttribute("state",QString("%1").arg(data.stateName));
		xmlStreamWriter->writeCharacters(QString::number(data.frameNumber));
		xmlStreamWriter->writeEndElement();
	}

	xmlStreamWriter->writeEndElement(); //BehavioralDataStore

	return true;
}
//! Converts XML into a FrameDataStore object.  Note that this deletes any existing data.
bool FrameDataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	clearFrames();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "FrameDataStore")
	{
		addError("FrameDataStore","Incorrect tag, expected <FrameDataStore>",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "FrameDataStore") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Frame")
		{
			double time;
			int frameNumber;
			QString state;

			if(xmlStreamReader->attributes().hasAttribute("time"))
			{
				time = xmlStreamReader->attributes().value("time").toString().toDouble();
			}
			else
			{
				addError("FrameDataStore","Frame missing time attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("state"))
			{
				state = xmlStreamReader->attributes().value("state").toString();
			}
			else
			{
				addError("FrameDataStore","Frame missing state attribute",xmlStreamReader);
				return false;
			}

			bool ok;
			frameNumber = xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
			{
				addError("FrameDataStore","Frame number not an integer",xmlStreamReader);
				return false;
			}

			addFrame(frameNumber, time, state);
		}
		else
		{
			addError("FrameDataStore", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("FrameDataStore", "Unexpected end of document", xmlStreamReader);
		return false;
	}
	return true;
}


}; //namespace Picto
