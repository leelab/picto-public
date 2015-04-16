#include "PropertyDataUnitPackage.h"
#include "../memleakdetect.h"

namespace Picto {

PropertyDataUnitPackage::PropertyDataUnitPackage()
{
}

/*! Adds a single Property change entry to this package.
 *	@param index The AssetId of the Property whose value changed.
 *	@param initValue True if the change was to an initValue, false if its a runValue.
 *	@param value The new Property value.
 */
void PropertyDataUnitPackage::addData(int index, bool initValue, QString value)
{
	QSharedPointer<PropertyDataUnit> newPoint(new PropertyDataUnit(index, initValue, value));
	data_.append(newPoint);
}

/*! \brief Sets the FrameId of the next frame to be presented after the all of the Property value changes in this package
 *	occured.
 */
void PropertyDataUnitPackage::setActionFrame(qulonglong frameId)
{
	for(QList<QSharedPointer<PropertyDataUnit>>::iterator it = data_.begin();it != data_.end();it++)
	{
		(*it)->setActionFrame(frameId);
	}
}

/*! \brief Turns the PropertyDataUnitPackage into an XML fragment
 */
bool PropertyDataUnitPackage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("PropertyDataUnitPackage");
	foreach(QSharedPointer<PropertyDataUnit> dataPoint, data_)
	{
		dataPoint->toXml(xmlStreamWriter);
	}

	xmlStreamWriter->writeEndElement(); //PropertyDataUnitPackage

	return true;
}

/*! \brief Converts XML into a PropertyDataUnitPackage object.  Note that this deletes any existing data.
 */
bool PropertyDataUnitPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	emptyData();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "PropertyDataUnitPackage")
	{
		addError("Incorrect tag, expected <PropertyDataUnitPackage>");
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "PropertyDataUnitPackage") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "PDU")
		{
			QSharedPointer<PropertyDataUnit> newPoint(new PropertyDataUnit());
			newPoint->fromXml(xmlStreamReader);
			data_.append(newPoint);
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
