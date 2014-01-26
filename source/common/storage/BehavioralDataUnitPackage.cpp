#include "BehavioralDataUnitPackage.h"
#include "../memleakdetect.h"

namespace Picto {

BehavioralDataUnitPackage::BehavioralDataUnitPackage()
{
}

/*! \brief Input a comma separated list of value names for the input data.
 * \details This string is used to identify the number of input values, and 
 * is sent along with the data to describe individual data points significance.
 * (ex. "xPos,yPos")
 */
void BehavioralDataUnitPackage::setDescriptor(QString descriptor)
{
	descriptor_ = descriptor;
}

/*! \brief Sets the signal channel from which this data was drawn.
 */
void BehavioralDataUnitPackage::setChannel(QString channel)
{
	channel_ = channel;
}

//! Adds a simple (x,y) sample
void BehavioralDataUnitPackage::addData(double x, double y)
{
	QSharedPointer<BehavioralDataUnit> newPoint(new BehavioralDataUnit(x,y));
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
void BehavioralDataUnitPackage::addData(QMap<QString, QVector<double>> signalChannelData, double frameToSampleOffset)
{
	//First, check that the signal channel data has the expected subchannels
	Q_ASSERT(signalChannelData.contains("x"));
	Q_ASSERT(signalChannelData.contains("y"));

	setDescriptor("x,y");

	//Next, run through the two lists generating data points and adding them
	//to our list
	setOffsetTime(frameToSampleOffset);

	QVector<double>::const_iterator xpos = signalChannelData.value("x").begin();
	QVector<double>::const_iterator ypos = signalChannelData.value("y").begin();

	while(xpos != signalChannelData.value("x").end() &&
		  ypos != signalChannelData.value("y").end())
	{
		QSharedPointer<BehavioralDataUnit> newPoint(new BehavioralDataUnit(*xpos,*ypos));
		data_.append(newPoint);

		xpos++;
		ypos++;
	}
}

/*! \brief Clears all data points apart from the ones at the latest time.
*/
void BehavioralDataUnitPackage::clearAllButLastDataPoints()
{
	if(length() > 1)
	{
		data_.erase(data_.begin(),data_.end()-1);	
	}
}

/*! \brief Returns the list of BehavioralDataUnits as a ByteArray.
*/
QByteArray BehavioralDataUnitPackage::getDataAsByteArray()
{
	int arraySize = data_.size() * 2;//"* 2" is for x,y.  \\ At some point this should be configurable so that we can support single signal channels.
	float* data = new float[arraySize];
	int i=0;
	foreach(QSharedPointer<BehavioralDataUnit> unit, data_)
	{
		data[i++] = unit->x;
		data[i++] = unit->y;
	}
	//Note: We must create the byte array with the constructor (not setRawData of fromRawData)
	//So that we can then delete the pots array without problems.
	QByteArray returnVal(reinterpret_cast<const char*>(data),arraySize*sizeof(float));
	delete[] data;
	return returnVal;
}

/*! \brief Returns the DataId of the last BehavioralDataUnit in the list.*/
qulonglong BehavioralDataUnitPackage::getDataIDOfLastUnit()
{
	if(!data_.size())
		return -1;
	return data_.last()->getDataID();
}

/*! \brief Turns the BehavioralDataUnitPackage into an XML fragment
 */
bool BehavioralDataUnitPackage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("BDUP");
	xmlStreamWriter->writeAttribute("chan",getChannel());
	xmlStreamWriter->writeAttribute("res",QString::number(getResolution(),'f',14));
	xmlStreamWriter->writeAttribute("offtime",getOffsetTime());
	xmlStreamWriter->writeAttribute("fr",QString::number(actionFrame_));
	xmlStreamWriter->writeAttribute("desc",getDescriptor());

	foreach(QSharedPointer<BehavioralDataUnit> dataPoint, data_)
	{
		dataPoint->toXml(xmlStreamWriter);
	}

	xmlStreamWriter->writeEndElement(); //BehavioralDataUnitPackage

	return true;
}
/*! Converts XML into a BehavioralDataUnitPackage object.  Note that this deletes any existing data.
*/
bool BehavioralDataUnitPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	emptyData();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "BDUP")
	{
		addError("Incorrect tag, expected <BDUP>");
		return false;
	}
	if(xmlStreamReader->attributes().hasAttribute("chan"))
	{
		setChannel(xmlStreamReader->attributes().value("chan").toString());
	}
	else
	{
		addError("BehavioralDataUnitPackage missing chan (Signal Channel) attribute");
		return false;
	}
	
	if(xmlStreamReader->attributes().hasAttribute("res"))
	{
		setResolution(xmlStreamReader->attributes().value("res").toString().toDouble());
	}
	else
	{
		addError("BehavioralDataUnitPackage missing res (Resolution) attribute");
		return false;
	}
	
	if(xmlStreamReader->attributes().hasAttribute("offtime"))
	{
		setOffsetTime(xmlStreamReader->attributes().value("offtime").toString());
	}
	else
	{
		addError("BehavioralDataUnitPackage missing offtime (offset time) attribute");
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("fr"))
	{
		setActionFrame(xmlStreamReader->attributes().value("fr").toString().toLongLong());
	}
	else
	{
		addError("BehavioralDataUnitPackage missing fr (action frame) attribute");
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("desc"))
	{
		setDescriptor(xmlStreamReader->attributes().value("desc").toString());
	}
	else
	{
		addError("BehavioralDataUnitPackage missing desc (Descriptor) attribute");
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "BDUP") && !xmlStreamReader->atEnd())
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
			QSharedPointer<BehavioralDataUnit> newPoint(new BehavioralDataUnit());
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

void BehavioralDataUnitPackage::postDeserialize()
{

}

bool BehavioralDataUnitPackage::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
