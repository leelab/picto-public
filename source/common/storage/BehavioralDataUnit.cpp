#include "BehavioralDataUnit.h"

namespace Picto {

BehavioralDataUnit::BehavioralDataUnit()
{
}

BehavioralDataUnit::BehavioralDataUnit(double X, double Y, double T)
{
	x = X; 
	y = Y; 
	t = T;
};

/*! \brief Turns the BehavioralDataUnit into an XML fragment
 *
 *	The XML will look like this:
 *	<BehavioralDataUnit time=123.4324 x=450 y=394/>
 */
bool BehavioralDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("BehavioralDataUnit");
	xmlStreamWriter->writeAttribute("time",QString("%1").arg(t,0,'e',6));
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
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "BehavioralDataUnit")
	{
		addError("BehavioralDataUnitPackage","Incorrect tag, expected <BehavioralDataUnitPackage>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "BehavioralDataUnit") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "BehavioralDataUnit")
		{
			if(xmlStreamReader->attributes().hasAttribute("x"))
			{
				x = xmlStreamReader->attributes().value("x").toString().toDouble();
			}
			else
			{
				addError("BehavioralDataUnit","Data missing x attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("y"))
			{
				y = xmlStreamReader->attributes().value("y").toString().toDouble();
			}
			else
			{
				addError("BehavioralDataUnit","Data missing y attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("time"))
			{
				t = xmlStreamReader->attributes().value("time").toString().toDouble();
			}
			else
			{
				addError("BehavioralDataUnit","Data missing time attribute",xmlStreamReader);
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

bool BehavioralDataUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
