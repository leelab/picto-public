#include "AlignmentDataUnit.h"
#include "../memleakdetect.h"

namespace Picto {

AlignmentDataUnit::AlignmentDataUnit()
{
	setTimestamp(0.0);
	setAlignCode(0);
	setAlignNumber(-1);
}

/*! \brief Serializes out alignment event data.
 *	Returns true if successful.
 */
bool AlignmentDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->setAutoFormatting(true);

	xmlStreamWriter->writeStartElement("ADU");
	xmlStreamWriter->writeTextElement("t",QString("%1").arg(getTimestamp(),0,'f',14));
	xmlStreamWriter->writeTextElement("e",QString("%1").arg(getAlignCode()));
	xmlStreamWriter->writeTextElement("n",QString("%1").arg(getAlignNumber()));
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
/*!	\brief Deserializes alignment event data from XML into this object.
 *	Returns true if successful.
 */
bool AlignmentDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
 	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "ADU")
	{
		addError("Incorrect tag, expected <ADU>");
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "ADU") && !xmlStreamReader->atEnd())
	{	
		if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "t")
		{
			setTimestamp(xmlStreamReader->readElementText().toDouble());
		}
		else if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "e")
		{
			setAlignCode(xmlStreamReader->readElementText().toInt());
		}
		else if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "n")
		{
			setAlignNumber(xmlStreamReader->readElementText().toInt());
		}
		else if(xmlStreamReader->isStartElement())
		{
			DataUnit::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	
		if(xmlStreamReader->hasError())
		{
			addError(xmlStreamReader->errorString());
			return false;
		}
	}
	return true;
}

}; //namespace Picto
