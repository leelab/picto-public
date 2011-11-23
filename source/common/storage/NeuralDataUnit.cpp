#include "NeuralDataUnit.h"

namespace Picto {

NeuralDataUnit::NeuralDataUnit()
{
	setTimestamp(0.0);
	setChannel(0);
	setUnit(0);
	setWaveformFromString("");
}

/*! \brief Will serialize out spike data. Not Yet Implemented.
 *	Returns true if successful.
 */
bool NeuralDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->setAutoFormatting(true);

	xmlStreamWriter->writeStartElement("NDU");
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeTextElement("t",QString("%1").arg(getTimestamp(),0,'f',6));
	xmlStreamWriter->writeTextElement("c",QString("%1").arg(getChannel()));
	xmlStreamWriter->writeTextElement("u",QString("%1").arg(getUnit()));
	xmlStreamWriter->writeTextElement("w",getWaveformAsString());		
	xmlStreamWriter->writeEndElement();
	return true;
}
/*!	\brief Deserializes Neural data from XML into this object.
 *	Returns true if successful.
 */
bool NeuralDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
 	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "NDU")
	{
		addError("NeuralDataUnit","Incorrect tag, expected <NDU>",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "NDU") && !xmlStreamReader->atEnd())
	{	
		if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "t")
		{
			setTimestamp(xmlStreamReader->readElementText().toDouble());
		}
		else if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "c")
		{
			setChannel(xmlStreamReader->readElementText().toInt());
		}
		else if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "u")
		{
			setUnit(xmlStreamReader->readElementText().toInt());
		}
		else if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "w")
		{
			setWaveformFromString(xmlStreamReader->readElementText());
		}
		else if(xmlStreamReader->isStartElement())
		{
			DataUnit::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	
		if(xmlStreamReader->hasError())
		{
			addError("NeuralDataUnit", "xmlStreamReader.errorString()", xmlStreamReader);
			return false;
		}
	}
	return true;
}

void NeuralDataUnit::postDeserialize()
{

}

bool NeuralDataUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}

//! \brief Waveform should be formatted as "X X X X " where each X is an int
void NeuralDataUnit::setWaveformFromString(QString waveform)
{
	waveform_ = QSharedPointer<QList<int>>(new QList<int>);
	QStringList vals = waveform.split(" ",QString::SkipEmptyParts);
	foreach(QString val, vals)
	{
		waveform_->push_back(val.toInt());
	}
}

QString NeuralDataUnit::getWaveformAsString()
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
