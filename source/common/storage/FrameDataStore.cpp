#include "FrameDataStore.h"

namespace Picto {

FrameDataStore::FrameDataStore()
{
}

void FrameDataStore::addFrame(int frameNumber, double time, QString stateName)
{
	FrameUnitDataStore data(frameNumber, time, stateName);
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

	foreach(FrameUnitDataStore data, data_)
	{
		data.serializeAsXml(xmlStreamWriter);
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
		if(name == "FrameUnitDataStore")
		{
			FrameUnitDataStore data;
			data.deserializeFromXml(xmlStreamReader);
			data_.append(data);
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
