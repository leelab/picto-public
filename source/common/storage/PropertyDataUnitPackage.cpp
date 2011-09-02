#include "PropertyDataUnitPackage.h"

namespace Picto {

PropertyDataUnitPackage::PropertyDataUnitPackage()
{
}

//! Adds a simple (path,value,time) data point
void PropertyDataUnitPackage::addData(int index, QString path, QString value, double time)
{
	QSharedPointer<PropertyDataUnit> newPoint(new PropertyDataUnit(index, path,value,time));
	data_.append(newPoint);
}

void PropertyDataUnitPackage::addData(int index, QString path, QString value, QString time)
{
	QSharedPointer<PropertyDataUnit> newPoint(new PropertyDataUnit(index, path,value,time));
	data_.append(newPoint);
}

/*! \brief Turns the PropertyDataUnitPackage into an XML fragment
 *
 *	The XML will look like this:
 *	<PropertyDataUnitPackage>
 *		<PropertyDataUnit p="TopMachine::Block::TaskNum v="5" t=394.0330/>
 *		<PropertyDataUnit p="TopMachine::BlockNum v="2" t=397.4652/>
 *		...
 *	</PropertyDataUnitPackage>
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
//! Converts XML into a PropertyDataUnitPackage object.  Note that this deletes any existing data.
bool PropertyDataUnitPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	emptyData();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "PropertyDataUnitPackage")
	{
		addError("PropertyDataUnitPackage","Incorrect tag, expected <PropertyDataUnitPackage>",xmlStreamReader);
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
		if(name == "PropertyDataUnit")
		{
			QSharedPointer<PropertyDataUnit> newPoint(new PropertyDataUnit());
			newPoint->fromXml(xmlStreamReader);
			data_.append(newPoint);
		}
		else
		{
			addError("PropertyDataUnitPackage", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("PropertyDataUnitPackage", "Unexpected end of document", xmlStreamReader);
		return false;
	}
	return true;
}

void PropertyDataUnitPackage::postSerialize()
{

}

bool PropertyDataUnitPackage::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
