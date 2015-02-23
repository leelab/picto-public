#include "InputDataUnitPackage.h"
#include "../memleakdetect.h"

namespace Picto {

InputDataUnitPackage::InputDataUnitPackage()
{
}

/*! \brief Input a comma separated list of value names for the input data.
 * \details This string is used to identify the number of input values, and 
 * is sent along with the data to describe individual data points significance.
 * (ex. "input0,input1,input2,input3,input4,input5,input6,input7")
 */
void InputDataUnitPackage::setDescriptor(QString descriptor)
{
	descriptor_ = descriptor;
}

/*! \brief Sets the signal channel from which this data was drawn.
 */
void InputDataUnitPackage::setChannel(QString channel)
{
	channel_ = channel;
}

//! Adds a simple (x,y) sample
void InputDataUnitPackage::addData(double input[8])
{
	QSharedPointer<InputDataUnit> newPoint(new InputDataUnit(input));
	data_.append(newPoint);
}

/*!	\brief Adds the map returned from a behavioral data signal channel
 *
 *	The format of the input to this function seems a bit odd, but it is
 *	exactly the same Map used by SignalChannel::getValues(), allowing
 *	us to add the data directly to our data store.
 *
 *	frameToSampleOffset is the offset time between when the previous frame's phosphor appeared and when the first samples
 *	in the input signalChannelData were read.
 */
void InputDataUnitPackage::addData(QMap<QString, QVector<double>> signalChannelData, double frameToSampleOffset)
{
	//First, check that the signal channel data has the expected subchannels
	/*
	Q_ASSERT(signalChannelData.contains("input0"));
	Q_ASSERT(signalChannelData.contains("input1"));
	Q_ASSERT(signalChannelData.contains("input2"));
	Q_ASSERT(signalChannelData.contains("input3"));
	Q_ASSERT(signalChannelData.contains("input4"));
	Q_ASSERT(signalChannelData.contains("input5"));
	Q_ASSERT(signalChannelData.contains("input6"));
	Q_ASSERT(signalChannelData.contains("input7"));
	*/

	setDescriptor("input0,input1,input2,input3,input4,input5,input6,input7");

	//Next, run through the lists generating data points and adding them
	//to our list
	setOffsetTime(frameToSampleOffset);

	double dataBuffer[8] = { 0.0 };
	bool inputPresent[8] = { false };
	
	QVector<double>::const_iterator inputElem[8];
	QVector<double>::const_iterator inputElemEnd[8];

	for (int i = 0; i < 8; i++)
	{
		if (signalChannelData.contains(QString("input%1").arg(i)))
		{
			inputElem[i] = signalChannelData.value(QString("input%1").arg(i)).begin();
			inputElemEnd[i] = signalChannelData.value(QString("input%1").arg(i)).end();
			inputPresent[i] = true;
		}
	}

	while ( [&]()->bool {
			for (int i = 0; i < 8; i++) {
				if (inputPresent[i] && (inputElem[i] == inputElemEnd[i]))
					return false;
			}
			return true;
		}())
	{
		for (int i = 0; i < 8; i++)
		{
			dataBuffer[i] = inputPresent[i] ? *inputElem[i] : 0;
		}

		QSharedPointer<InputDataUnit> newPoint(new InputDataUnit(dataBuffer));
		data_.append(newPoint);

		for (int i = 0; i < 8; i++)
		{
			inputPresent[i] ? inputElem[i]++ : 0;
		}
	}
}

/*! \brief Clears all data points apart from the ones at the latest time.
*/
void InputDataUnitPackage::clearAllButLastDataPoints()
{
	if(length() > 1)
	{
		data_.erase(data_.begin(),data_.end()-1);	
	}
}

/*! \brief Returns the list of BehavioralDataUnits as a ByteArray.
*/
QByteArray InputDataUnitPackage::getDataAsByteArray()
{
	int arraySize = data_.size() * 8;//"* 8" is for the 8 channels.  \\ At some point this should be configurable so that we can support variable-sized signal channels.
	float* data = new float[arraySize];
	int i=0;
	foreach(QSharedPointer<InputDataUnit> unit, data_)
	{
		for (int j = 0; j < 8; j++)
		{
			data[i++] = unit->input[j];
		}
	}
	//Note: We must create the byte array with the constructor (not setRawData of fromRawData)
	//So that we can then delete the pots array without problems.
	QByteArray returnVal(reinterpret_cast<const char*>(data),arraySize*sizeof(float));
	delete[] data;
	return returnVal;
}

/*! \brief Returns the DataId of the last BehavioralDataUnit in the list.*/
qulonglong InputDataUnitPackage::getDataIDOfLastUnit()
{
	if(!data_.size())
		return -1;
	return data_.last()->getDataID();
}

/*! \brief Turns the BehavioralDataUnitPackage into an XML fragment
 */
bool InputDataUnitPackage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("IDUP");
	xmlStreamWriter->writeAttribute("chan",getChannel());
	xmlStreamWriter->writeAttribute("res",QString::number(getResolution(),'f',14));
	xmlStreamWriter->writeAttribute("offtime",getOffsetTime());
	xmlStreamWriter->writeAttribute("fr",QString::number(actionFrame_));
	xmlStreamWriter->writeAttribute("desc",getDescriptor());

	foreach(QSharedPointer<InputDataUnit> dataPoint, data_)
	{
		dataPoint->toXml(xmlStreamWriter);
	}

	xmlStreamWriter->writeEndElement(); //BehavioralDataUnitPackage

	return true;
}
/*! Converts XML into a BehavioralDataUnitPackage object.  Note that this deletes any existing data.
*/
bool InputDataUnitPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	emptyData();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "IDUP")
	{
		addError("Incorrect tag, expected <IDUP>");
		return false;
	}
	if(xmlStreamReader->attributes().hasAttribute("chan"))
	{
		setChannel(xmlStreamReader->attributes().value("chan").toString());
	}
	else
	{
		addError("InputDataUnitPackage missing chan (Signal Channel) attribute");
		return false;
	}
	
	if(xmlStreamReader->attributes().hasAttribute("res"))
	{
		setResolution(xmlStreamReader->attributes().value("res").toString().toDouble());
	}
	else
	{
		addError("InputDataUnitPackage missing res (Resolution) attribute");
		return false;
	}
	
	if(xmlStreamReader->attributes().hasAttribute("offtime"))
	{
		setOffsetTime(xmlStreamReader->attributes().value("offtime").toString());
	}
	else
	{
		addError("InputDataUnitPackage missing offtime (offset time) attribute");
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("fr"))
	{
		setActionFrame(xmlStreamReader->attributes().value("fr").toString().toLongLong());
	}
	else
	{
		addError("InputDataUnitPackage missing fr (action frame) attribute");
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("desc"))
	{
		setDescriptor(xmlStreamReader->attributes().value("desc").toString());
	}
	else
	{
		addError("InputDataUnitPackage missing desc (Descriptor) attribute");
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "IDUP") && !xmlStreamReader->atEnd())
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
			QSharedPointer<InputDataUnit> newPoint(new InputDataUnit());
			newPoint->fromXml(xmlStreamReader);
			data_.append(newPoint);
		}
		else
		{
			addError("Unexpected tag");
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("Unexpected end of document");
		return false;
	}
	return true;
}

void InputDataUnitPackage::postDeserialize()
{

}

bool InputDataUnitPackage::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
