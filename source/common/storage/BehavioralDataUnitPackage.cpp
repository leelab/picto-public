#include "BehavioralDataUnitPackage.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Contains all data from a single signal channel.
 * The name of the channel is passed in as input.
 */
BehavioralDataUnitPackage::BehavioralDataUnitPackage()
{
}

/*! \brief Sets the signal channel from which this data was drawn.
 */
void BehavioralDataUnitPackage::setChannel(QString channel)
{
	channel_ = channel;
}

//! Adds a simple (x,y,t) data point
void BehavioralDataUnitPackage::addData(double x, double y, double t)
{
	QSharedPointer<BehavioralDataUnit> newPoint(new BehavioralDataUnit(x,y,t));
	data_.append(newPoint);
}
void BehavioralDataUnitPackage::addData(double x, double y, QString t)
{
	QSharedPointer<BehavioralDataUnit> newPoint(new BehavioralDataUnit(x,y,t));
	data_.append(newPoint);
}

/*!	\brief Adds the map returned from a behavioral data signal channel
 *
 *	The format of the input to this function seems a bit odd, but it is
 *	exactly the same Map used by SignalChannel::getValues, allowing
 *	us to add the data directly to our data store.
 */
void BehavioralDataUnitPackage::addData(QMap<QString, QList<double>> signalChannelData)
{
	//First, check that the signal channel data has the expected subchannels
	Q_ASSERT(signalChannelData.contains("x"));
	Q_ASSERT(signalChannelData.contains("y"));
	Q_ASSERT(signalChannelData.contains("time"));

	//Next, run through the three lists generating data points and adding them
	//to our list
	QList<double> xposList = signalChannelData.value("x");
	QList<double> yposList = signalChannelData.value("y");
	QList<double> timeList = signalChannelData.value("time");

	QList<double>::const_iterator xpos = signalChannelData.value("x").begin();
	QList<double>::const_iterator ypos = signalChannelData.value("y").begin();
	QList<double>::const_iterator time = signalChannelData.value("time").begin();

	while(xpos != signalChannelData.value("x").end() &&
		  ypos != signalChannelData.value("y").end() &&
		  time != signalChannelData.value("time").end())
	{
		QSharedPointer<BehavioralDataUnit> newPoint(new BehavioralDataUnit(*xpos,*ypos,*time));
		data_.append(newPoint);

		xpos++;
		ypos++;
		time++;
	}
}


/*! \brief Turns the BehavioralDataUnitPackage into an XML fragment
 *
 *	The XML will look like this:
 *	<BehavioralDataUnitPackage>
 *		<BehavioralDataUnit time=123.4324 x=450 y=394/>
 *		<BehavioralDataUnit time=123.4334 x=457 y=386/>
 *		...
 *	</BehavioralDataUnitPackage>
 */
bool BehavioralDataUnitPackage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("BDUP");
	xmlStreamWriter->writeAttribute("chan",getChannel());

	foreach(QSharedPointer<BehavioralDataUnit> dataPoint, data_)
	{
		dataPoint->toXml(xmlStreamWriter);
	}

	xmlStreamWriter->writeEndElement(); //BehavioralDataUnitPackage

	return true;
}
//! Converts XML into a BehavioralDataUnitPackage object.  Note that this deletes any existing data.
bool BehavioralDataUnitPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	emptyData();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "BDUP")
	{
		addError("BehavioralDataUnitPackage","Incorrect tag, expected <BDUP>",xmlStreamReader);
		return false;
	}
	if(xmlStreamReader->attributes().hasAttribute("chan"))
	{
		setChannel(xmlStreamReader->attributes().value("chan").toString());
	}
	else
	{
		addError("BehavioralDataUnitPackage","LFPDataUnitPackage missing chan (Signal Channel) attribute",xmlStreamReader);
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
			addError("BehavioralDataUnitPackage", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("BehavioralDataUnitPackage", "Unexpected end of document", xmlStreamReader);
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
