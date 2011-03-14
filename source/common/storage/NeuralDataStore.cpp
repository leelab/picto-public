#include "NeuralDataStore.h"

namespace Picto {

NeuralDataStore::NeuralDataStore()
{
	setTimestamp(0.0);
	setChannel(0);
	setUnit(0);
	setWaveformFromString("");
}

/*! \brief Will serialize out spike data. Not Yet Implemented.
 *	Returns true if successful.
 */
bool NeuralDataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->setAutoFormatting(true);

	xmlStreamWriter->writeStartElement("NeuralDataStore");
	DataStore::serializeAsXml(xmlStreamWriter);
	xmlStreamWriter->writeTextElement("timestamp",QString("%1").arg(getTimestamp(),0,'f',4));
	xmlStreamWriter->writeTextElement("channel",QString("%1").arg(getChannel()));
	xmlStreamWriter->writeTextElement("unit",QString("%1").arg(getUnit()));
	xmlStreamWriter->writeTextElement("wave",getWaveformAsString());		
	xmlStreamWriter->writeEndElement();
	return true;
}
/*!	\brief Deserializes Neural data from XML into this object.
 *	Returns true if successful.
 */
bool NeuralDataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
 	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "NeuralDataStore")
	{
		addError("NeuralDataStore","Incorrect tag, expected <NeuralDataStore>",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "NeuralDataStore") && !xmlStreamReader->atEnd())
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
			setWaveformFromString(xmlStreamReader->readElementText());
		}
		else if(xmlStreamReader->isStartElement())
		{
			DataStore::deserializeFromXml(xmlStreamReader);
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
//! \brief Waveform should be formatted as "X X X X " where each X is an int
void NeuralDataStore::setWaveformFromString(QString waveform)
{
	waveform_ = QSharedPointer<QList<int>>(new QList<int>);
	QStringList vals = waveform.split(" ",QString::SkipEmptyParts);
	foreach(QString val, vals)
	{
		waveform_->push_back(val.toInt());
	}
}

QString NeuralDataStore::getWaveformAsString()
{
	QString returnVal;
	QList<int>::Iterator it;
	for(it = waveform_->begin();it != waveform_->end(); it++)
	{
		returnVal += QString("%1 ").arg((*it));
	}
	return returnVal;
}

}; //namespace Picto
