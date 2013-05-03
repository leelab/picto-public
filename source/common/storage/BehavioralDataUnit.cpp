#include "BehavioralDataUnit.h"
#include "../memleakdetect.h"

namespace Picto {

BehavioralDataUnit::BehavioralDataUnit()
{
}

BehavioralDataUnit::BehavioralDataUnit(double X, double Y)
{
	x = X; 
	y = Y;
};

/*! \brief Turns the BehavioralDataUnit into an XML fragment
 *
 *	The XML will look like this:
 *	<BehavioralDataUnit time=123.4324 x=450 y=394/>
 */
bool BehavioralDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("BDU");
	xmlStreamWriter->writeAttribute("x",QString("%1").arg(x));
	xmlStreamWriter->writeAttribute("y",QString("%1").arg(y));
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a BehavioralDataUnit object.  Note that this deletes any existing data.
bool BehavioralDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "BDU")
	{
		addError("Incorrect tag, expected <BDU>");
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "BDU") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "BDU")
		{
			if(xmlStreamReader->attributes().hasAttribute("x"))
			{
				x = xmlStreamReader->attributes().value("x").toString().toDouble();
			}
			else
			{
				addError("Data missing x attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("y"))
			{
				y = xmlStreamReader->attributes().value("y").toString().toDouble();
			}
			else
			{
				addError("Data missing y attribute");
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

void BehavioralDataUnit::postDeserialize()
{

}

bool BehavioralDataUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
