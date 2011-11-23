#include "BehavioralDataUnit.h"

namespace Picto {

BehavioralDataUnit::BehavioralDataUnit()
{
}

BehavioralDataUnit::BehavioralDataUnit(double X, double Y, double T)
{
	x = X; 
	y = Y; 
	t = QString("%1").arg(T,0,'f',6);;
};

BehavioralDataUnit::BehavioralDataUnit(double X, double Y, QString T)
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
	xmlStreamWriter->writeStartElement("BDU");
	xmlStreamWriter->writeAttribute("t",t);
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
		addError("BehavioralDataUnit","Incorrect tag, expected <BDU>",xmlStreamReader);
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
			if(xmlStreamReader->attributes().hasAttribute("t"))
			{
				t = xmlStreamReader->attributes().value("t").toString();
			}
			else
			{
				addError("BehavioralDataUnit","Data missing t (time) attribute",xmlStreamReader);
				return false;
			}

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
