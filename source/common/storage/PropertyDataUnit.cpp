#include "PropertyDataUnit.h"
#include "../memleakdetect.h"

namespace Picto {

PropertyDataUnit::PropertyDataUnit()
{
}

PropertyDataUnit::PropertyDataUnit(int index, bool initValue, QString value)
{
	index_ = index; 
	initValue_ = initValue;
	value_ = value; 
}
/*! \brief Turns the PropertyDataUnit into an XML fragment
 *
 *	The XML will look like this:
 *	<PropertyDataUnit time=123.4324 x=450 y=394/>
 */
bool PropertyDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("PDU");
	xmlStreamWriter->writeAttribute("f",QString::number(actionFrame_));
	xmlStreamWriter->writeAttribute("i",QString("%1").arg(index_));
	xmlStreamWriter->writeAttribute("r",QString(initValue_?"t":"f"));
	xmlStreamWriter->writeAttribute("v",QString("%1").arg(value_));
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a PropertyDataUnit object.  Note that this deletes any existing data.
bool PropertyDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "PDU")
	{
		addError("PropertyDataUnit","Incorrect tag, expected <PDU>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "PDU") && !xmlStreamReader->atEnd())
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
			if(xmlStreamReader->attributes().hasAttribute("f"))
			{
				actionFrame_ = xmlStreamReader->attributes().value("f").toString().toLongLong();
			}
			else
			{
				addError("PropertyDataUnit","Data missing f (frame) attribute",xmlStreamReader);
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

			if(xmlStreamReader->attributes().hasAttribute("r"))
			{
				initValue_ = xmlStreamReader->attributes().value("r").toString() == "t";
			}
			else
			{
				addError("PropertyDataUnit","Data missing r (is init value) attribute",xmlStreamReader);
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
		}
		else
		{
			DataUnit::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}
	return true;
}

void PropertyDataUnit::postDeserialize()
{

}

bool PropertyDataUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
