#include "FrameDataUnitPackage.h"
#include "../memleakdetect.h"

namespace Picto {

FrameDataUnitPackage::FrameDataUnitPackage()
{
}
/*! \brief Adds a new frame to this package with the input first phosphor time and the input AssetId of the state that was active when this frame was displayed.*/
void FrameDataUnitPackage::addFrame(double time, int stateId)
{
	QSharedPointer<FrameDataUnit> data(new FrameDataUnit(time, stateId));
	data_.append(data);
}

/*! \brief Adds a new frame to this package with the input first phosphor time (as a string) and the input AssetId of the state that was active when this frame was displayed.*/
void FrameDataUnitPackage::addFrame(QString time, int stateId)
{
	QSharedPointer<FrameDataUnit> data(new FrameDataUnit(time, stateId));
	data_.append(data);
}

/*! \brief Turns the FrameDataUnitPackage into an XML fragment
 *	
 */
bool FrameDataUnitPackage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("FrameDataUnitPackage");

	foreach(QSharedPointer<FrameDataUnit> data, data_)
	{
		data->toXml(xmlStreamWriter);
	}

	xmlStreamWriter->writeEndElement();

	return true;
}
//! Converts XML into a FrameDataUnitPackage object.  Note that this deletes any existing data.
bool FrameDataUnitPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	clearFrames();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "FrameDataUnitPackage")
	{
		addError("Incorrect tag, expected <FrameDataUnitPackage>");
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
			addError("Unexpected tag");
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("Unexpected end of document");
		return false;
	}
	return true;
}

}; //namespace Picto
