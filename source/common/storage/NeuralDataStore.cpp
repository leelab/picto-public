#include "NeuralDataStore.h"

namespace Picto {

NeuralDataStore::NeuralDataStore()
{
	setTimestamp(0.0);
	setChannel(0);
	setUnit(0);
	setWaveform(0);
}

/*! \brief Will serialize out spike data. Not Yet Implemented.
 *	Returns true if successful.
 */
bool NeuralDataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	return false;
}
/*!	\brief Deserializes Neural data from XML into this object.
 *	Returns true if successful.
 */
bool NeuralDataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
 	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "event")
	{
		addError("NeuralDataStore","Incorrect tag, expected <spike>",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "event") && !xmlStreamReader->atEnd())
	{	
		if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "timestamp")
		{
			setTimestamp(xmlStreamReader->readElementText().toDouble());
		}
		else if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "channel")
		{
			setChannel(xmlStreamReader->readElementText().toInt());
		}
		else if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "unit")
		{
			setUnit(xmlStreamReader->readElementText().toInt());
		}
		else if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "wave")
		{
			setWaveform(xmlStreamReader->readElementText());
		}
		else
		{
		}
		xmlStreamReader->readNext();
	
		if(xmlStreamReader->hasError())
		{
			addError("NeuralDataStore", "xmlStreamReader.errorString()", xmlStreamReader);
			return false;
		}
	}
	return true;
}


}; //namespace Picto
