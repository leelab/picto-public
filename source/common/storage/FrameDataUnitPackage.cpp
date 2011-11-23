#include "FrameDataUnitPackage.h"

namespace Picto {

FrameDataUnitPackage::FrameDataUnitPackage()
{
}

void FrameDataUnitPackage::addFrame(double time, int stateId)
{
	QSharedPointer<FrameDataUnit> data(new FrameDataUnit(time, stateId));
	data_.append(data);
}

void FrameDataUnitPackage::addFrame(QString time, int stateId)
{
	QSharedPointer<FrameDataUnit> data(new FrameDataUnit(time, stateId));
	data_.append(data);
}



/*! \brief Turns the FrameDataUnitPackage into an XML fragment
 *
 *	The XML will look like this:
 *	<FrameDataUnitPackage>
 *		<Frame timestamp=1.234 state=somestate>1</Frame>
 *		<Frame timestamp=1.434 state=somestate>2</Frame>
 *		...
 *	</FrameDataUnitPackage>
 *	
 */
bool FrameDataUnitPackage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("FrameDataUnitPackage");

	foreach(QSharedPointer<FrameDataUnit> data, data_)
	{
		data->toXml(xmlStreamWriter);
	}

	xmlStreamWriter->writeEndElement(); //BehavioralDataUnitPackage

	return true;
}
//! Converts XML into a FrameDataUnitPackage object.  Note that this deletes any existing data.
bool FrameDataUnitPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	clearFrames();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "FrameDataUnitPackage")
	{
		addError("FrameDataUnitPackage","Incorrect tag, expected <FrameDataUnitPackage>",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "FrameDataUnitPackage") && !xmlStreamReader->atEnd())
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
			QSharedPointer<FrameDataUnit> data(new FrameDataUnit());
			data->fromXml(xmlStreamReader);
			data_.append(data);
		}
		else
		{
			addError("FrameDataUnitPackage", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("FrameDataUnitPackage", "Unexpected end of document", xmlStreamReader);
		return false;
	}
	return true;
}

void FrameDataUnitPackage::postDeserialize()
{

}

bool FrameDataUnitPackage::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
