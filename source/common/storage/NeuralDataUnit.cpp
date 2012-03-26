#include "NeuralDataUnit.h"
#include "../memleakdetect.h"

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
	xmlStreamWriter->writeTextElement("t",QString("%1").arg(getTimestamp(),0,'f',14));
	xmlStreamWriter->writeTextElement("f",QString("%1").arg(getFittedtime(),0,'f',14));
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
		else if(xmlStreamReader->isStartElement() && xmlStreamReader->name() == "f")
		{
			setFittedtime(xmlStreamReader->readElementText().toDouble());
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

void NeuralDataUnit::setWaveform(QSharedPointer<QVector<float>> waveform)
{
	waveform_.clear();
	for(int i=0;i<waveform->size();i++)
	{
		waveform_.push_back(QString("%1").arg((*waveform)[i]));
	}
}

//! \brief Waveform should be formatted as "X X X X " where each X is an int
void NeuralDataUnit::setWaveformFromString(QString waveform)
{
	waveform_.clear();
	waveform_ = waveform.split(" ",QString::SkipEmptyParts);
}

QString NeuralDataUnit::getWaveformAsString()
{
	return waveform_.join(" ");
}

QByteArray NeuralDataUnit::getWaveformAsByteArray()
{
	float* pots = new float[waveform_.size()];
	for(int i=0;i<waveform_.size();i++)
	{
		pots[i] = waveform_[i].toFloat();
	}
	//Note: We must create the byte array with the constructor (not setRawData of fromRawData)
	//So that we can then delete the pots array without problems.
	QByteArray returnVal(reinterpret_cast<const char*>(pots),waveform_.size()*sizeof(float));
	delete[] pots;
	return returnVal;
}

void NeuralDataUnit::setWaveformFromByteArray(QByteArray waveform)
{
	const float* pots = reinterpret_cast<const float*>(waveform.constData());
	waveform_.clear();
	for(unsigned int i=0;i<waveform.size()/sizeof(float);i++)
	{
		waveform_.append(QString("%1").arg(pots[i]));
	}
}

}; //namespace Picto
