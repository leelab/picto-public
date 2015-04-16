#include "InputDataUnit.h"
#include "../memleakdetect.h"

namespace Picto {

InputDataUnit::InputDataUnit()
{
}

/*! \brief Constucts an InputDataUnit object.
 *	\details input%i for i:0->7 are the components of the signal channel.
 */
InputDataUnit::InputDataUnit(double Input[8])
{
	memcpy(input, Input, sizeof(input));
};

/*! \brief Turns the InputDataUnit into an XML fragment
 */
bool InputDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("IDU");
	for (int i = 0; i < 8; i++)
	{
		xmlStreamWriter->writeAttribute(QString("input%1").arg(i), QString("%1").arg(input[i]));
	}
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a InputDataUnit object.  Note that this deletes any existing data.
bool InputDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "IDU")
	{
		addError("Incorrect tag, expected <IDU>");
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "IDU") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "IDU")
		{
			for (int i = 0; i < 8; i++)
			{
				if (xmlStreamReader->attributes().hasAttribute(QString("input%1").arg(i)))
				{
					input[i] = xmlStreamReader->attributes().value(QString("input%1").arg(i)).toString().toDouble();
				}
				else
				{
					addError(QString("Data missing input%1 attribute").arg(i));
					return false;
				}
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


}; //namespace Picto
