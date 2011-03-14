#include "AlignmentDataStore.h"

namespace Picto {

AlignmentDataStore::AlignmentDataStore()
{
	setTimestamp(0.0);
	setAlignCode(0);
	setAlignNumber(-1);
}

/*! \brief Serializes out alignment event data.
 *	Returns true if successful.
 */
bool AlignmentDataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->setAutoFormatting(true);

	xmlStreamWriter->writeStartElement("AlignmentDataStore");
	xmlStreamWriter->writeTextElement("Time",QString("%1").arg(getTimestamp(),0,'f',4));
	xmlStreamWriter->writeTextElement("EventCode",QString("%1").arg(getAlignCode()));
	xmlStreamWriter->writeTextElement("TrialNum",QString("%1").arg(getAlignNumber()));
	DataStore::serializeAsXml(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
/*!	\brief Deserializes alignment event data from XML into this object.
 *	Returns true if successful.
 */
bool AlignmentDataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
 	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "AlignmentDataStore")
	{
		addError("AlignmentDataStore","Incorrect tag, expected <AlignmentDataStore>",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "AlignmentDataStore") && !xmlStreamReader->atEnd())
	{	
		if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "Time")
		{
			setTimestamp(xmlStreamReader->readElementText().toDouble());
		}
		else if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "EventCode")
		{
			setAlignCode(xmlStreamReader->readElementText().toInt());
		}
		else if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "TrialNum")
		{
			setAlignNumber(xmlStreamReader->readElementText().toInt());
		}
		else if(xmlStreamReader->isStartElement())
		{
			DataStore::deserializeFromXml(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	
		if(xmlStreamReader->hasError())
		{
			addError("AlignmentDataStore", "xmlStreamReader.errorString()", xmlStreamReader);
			return false;
		}
	}
	return true;
}

}; //namespace Picto
