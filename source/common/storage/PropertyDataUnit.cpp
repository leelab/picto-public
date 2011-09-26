#include "PropertyDataUnit.h"

namespace Picto {

PropertyDataUnit::PropertyDataUnit()
{
}

PropertyDataUnit::PropertyDataUnit(int index, QString path, QString value, double time)
{
	index_ = index;
	path_ = path; 
	value_ = value; 
	time_ = QString("%1").arg(time,0,'f',6);
}

PropertyDataUnit::PropertyDataUnit(int index, QString path, QString value, QString time)
{
	index_ = index;
	path_ = path; 
	value_ = value; 
	time_ = time;
}

/*! \brief Turns the PropertyDataUnit into an XML fragment
 *
 *	The XML will look like this:
 *	<PropertyDataUnit time=123.4324 x=450 y=394/>
 */
bool PropertyDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("PropertyDataUnit");
	xmlStreamWriter->writeAttribute("t",time_);
	xmlStreamWriter->writeAttribute("i",QString("%1").arg(index_));
	xmlStreamWriter->writeAttribute("p",QString("%1").arg(path_));
	xmlStreamWriter->writeAttribute("v",QString("%1").arg(value_));
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a PropertyDataUnit object.  Note that this deletes any existing data.
bool PropertyDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "PropertyDataUnit")
	{
		addError("PropertyDataUnit","Incorrect tag, expected <PropertyDataUnit>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "PropertyDataUnit") && !xmlStreamReader->atEnd())
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
			if(xmlStreamReader->attributes().hasAttribute("p"))
			{
				path_ = xmlStreamReader->attributes().value("p").toString();
			}
			else
			{
				addError("PropertyDataUnit","Data missing p (path) attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("v"))
			{
				value_ = xmlStreamReader->attributes().value("v").toString();
			}
			else
			{
				addError("PropertyDataUnit","Data missing v (value) attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("i"))
			{
				index_ = xmlStreamReader->attributes().value("i").toString().toInt();
			}
			else
			{
				addError("PropertyDataUnit","Data missing i (index) attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("t"))
			{
				time_ = xmlStreamReader->attributes().value("t").toString();
			}
			else
			{
				addError("PropertyDataUnit","Data missing t (time) attribute",xmlStreamReader);
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

void PropertyDataUnit::postSerialize()
{

}

bool PropertyDataUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
