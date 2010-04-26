#include "BehavioralDataStore.h"

namespace Picto {

BehavioralDataStore::BehavioralDataStore()
{
}

//! Adds a simple (x,y,t) data point
void BehavioralDataStore::addData(double x, double y, double t)
{
	BehavioralDataPoint newPoint = {x,y,t};
	data_.append(newPoint);
}

/*!	\brief Adds the map returned from a behavioral data signal channel
 *
 *	The format of the input to this function seems a bit odd, but it is
 *	exactly the same Map used by SignalChannel::getValues, allowing
 *	us to add the data directly to our data store.
 */
void BehavioralDataStore::addData(QMap<QString, QList<double>> signalChannelData)
{
	//First, check that the signal channel data has the expected subchannels
	Q_ASSERT(signalChannelData.contains("xpos"));
	Q_ASSERT(signalChannelData.contains("ypos"));
	Q_ASSERT(signalChannelData.contains("time"));

	//Next, run through the three lists generating data points and adding them
	//to our list
	QList<double> xposList = signalChannelData.value("xpos");
	QList<double> yposList = signalChannelData.value("ypos");
	QList<double> timeList = signalChannelData.value("time");

	QList<double>::const_iterator xpos = signalChannelData.value("xpos").begin();
	QList<double>::const_iterator ypos = signalChannelData.value("ypos").begin();
	QList<double>::const_iterator time = signalChannelData.value("time").begin();

	while(xpos != signalChannelData.value("xpos").end() &&
		  ypos != signalChannelData.value("ypos").end() &&
		  time != signalChannelData.value("time").end())
	{
		BehavioralDataPoint newPoint = {*xpos,*ypos,*time};
		data_.append(newPoint);

		xpos++;
		ypos++;
		time++;
	}
}


/*! \brief Turns the BehavioralDataStore into an XML fragment
 *
 *	The XML will look like this:
 *	<BehavioralDataStore id=143>
 *		<Data timestamp=123.4324 x=450 y=394/>
 *		<Data timestamp=123.4334 x=457 y=386/>
 *		...
 *	</BehavioralDataStore>
 */
bool BehavioralDataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("BehavioralDataStore");

	foreach(BehavioralDataPoint dataPoint, data_)
	{
		xmlStreamWriter->writeStartElement("Data");
		xmlStreamWriter->writeAttribute("time",QString("%1").arg(dataPoint.t));
		xmlStreamWriter->writeAttribute("x",QString("%1").arg(dataPoint.x));
		xmlStreamWriter->writeAttribute("y",QString("%1").arg(dataPoint.y));
		xmlStreamWriter->writeEndElement();
	}

	xmlStreamWriter->writeEndElement(); //BehavioralDataStore

	return true;
}
//! Converts XML into a BehavioralDataStore object.  Note that this deletes any existing data.
bool BehavioralDataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	emptyData();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "BehavioralDataStore")
	{
		addError("BehavioralDataStore","Incorrect tag, expected <BehavioralDataStore>",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "BehavioralDataStore") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Data")
		{
			double x,y,t;

			if(xmlStreamReader->attributes().hasAttribute("x"))
			{
				x = xmlStreamReader->attributes().value("x").toString().toDouble();
			}
			else
			{
				addError("BehavioralDataStore","Data missing x attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("y"))
			{
				y = xmlStreamReader->attributes().value("y").toString().toDouble();
			}
			else
			{
				addError("BehavioralDataStore","Data missing y attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("time"))
			{
				t = xmlStreamReader->attributes().value("time").toString().toDouble();
			}
			else
			{
				addError("BehavioralDataStore","Data missing time attribute",xmlStreamReader);
				return false;
			}

			addData(x,y,t);
		}
		else
		{
			addError("BehavioralDataStore", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("BehavioralDataStore", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	if(xmlStreamReader->atEnd())
	{
		addError("BehavioralDataStore", "Unexpected end of document", xmlStreamReader);
		return false;
	}
	return true;
}


}; //namespace Picto
