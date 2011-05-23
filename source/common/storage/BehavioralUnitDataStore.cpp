#include "BehavioralUnitDataStore.h"

namespace Picto {

BehavioralUnitDataStore::BehavioralUnitDataStore()
{
}

BehavioralUnitDataStore::BehavioralUnitDataStore(double X, double Y, double T)
{
	x = X; 
	y = Y; 
	t = T;
};

/*! \brief Turns the BehavioralUnitDataStore into an XML fragment
 *
 *	The XML will look like this:
 *	<BehavioralUnitDataStore time=123.4324 x=450 y=394/>
 */
bool BehavioralUnitDataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("BehavioralUnitDataStore");
	xmlStreamWriter->writeAttribute("time",QString("%1").arg(t));
	xmlStreamWriter->writeAttribute("x",QString("%1").arg(x));
	xmlStreamWriter->writeAttribute("y",QString("%1").arg(y));
	DataStore::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a BehavioralUnitDataStore object.  Note that this deletes any existing data.
bool BehavioralUnitDataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "BehavioralUnitDataStore")
	{
		addError("BehavioralDataStore","Incorrect tag, expected <BehavioralDataStore>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "BehavioralUnitDataStore") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "BehavioralUnitDataStore")
		{
			if(xmlStreamReader->attributes().hasAttribute("x"))
			{
				x = xmlStreamReader->attributes().value("x").toString().toDouble();
			}
			else
			{
				addError("BehavioralUnitDataStore","Data missing x attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("y"))
			{
				y = xmlStreamReader->attributes().value("y").toString().toDouble();
			}
			else
			{
				addError("BehavioralUnitDataStore","Data missing y attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("time"))
			{
				t = xmlStreamReader->attributes().value("time").toString().toDouble();
			}
			else
			{
				addError("BehavioralUnitDataStore","Data missing time attribute",xmlStreamReader);
				return false;
			}
		}
		else
		{
			DataStore::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}
	return true;
}

bool BehavioralUnitDataStore::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
