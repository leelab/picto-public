#include "PropertyDataUnit.h"
#include "../memleakdetect.h"

namespace Picto {

PropertyDataUnit::PropertyDataUnit()
{
}

/*! \brief Constructs a PropertyDataUnit object.
 *	@param index The AssetId of the Property whose value changed.
 *	@param initValue True if the change was to an initValue, false if its a runValue.
 *	@param value The new Property value.
 */
PropertyDataUnit::PropertyDataUnit(int index, bool initValue, QString value)
{
	index_ = index; 
	initValue_ = initValue;
	value_ = value; 
}
/*! \brief Turns the PropertyDataUnit into an XML fragment
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
		addError("Incorrect tag, expected <PDU>");
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
				addError("Data missing f (frame) attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("i"))
			{
				index_ = xmlStreamReader->attributes().value("i").toString().toInt();
			}
			else
			{
				addError("Data missing i (index) attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("r"))
			{
				initValue_ = xmlStreamReader->attributes().value("r").toString() == "t";
			}
			else
			{
				addError("Data missing r (is init value) attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("v"))
			{
				value_ = xmlStreamReader->attributes().value("v").toString();
			}
			else
			{
				addError("Data missing v (value) attribute");
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
