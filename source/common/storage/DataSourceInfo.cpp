#include "DataSourceInfo.h"
#include "../memleakdetect.h"

namespace Picto {

DataSourceInfo::DataSourceInfo()
{
}

/*! \brief Builds a new signal channel info object.
 *	@param name The signal channel name
 *	@param tableName The name of the table associated with the signal channel
 *	@param stateVariableId The State Variable ID associated with the signal channel
 *	@param resolution time between each group of subchannel samples (seconds)
 *	@param subchannels number of subchannels in this SignalChannel
 *	@param subchannelNames A comma seperated string describing the subchannels
 *	in order of appearance(i.e. "xPos,yPos")
 */
DataSourceInfo::DataSourceInfo(QString name,
									 QString tableName,
									 double resolution)
{
	name_ = name;
	tableName_ = tableName;
	resolution_ = resolution;
}


/*! \brief Turns the DataSourceInfo into an XML fragment
 */
bool DataSourceInfo::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("DataSourceInfo");
	xmlStreamWriter->writeAttribute("name",name_);
	xmlStreamWriter->writeAttribute("tableName",tableName_);
	xmlStreamWriter->writeAttribute("resolution",QString("%1").arg(resolution_));
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a DataSourceInfo object.  Note that this deletes any existing data.
bool DataSourceInfo::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "DataSourceInfo")
	{
		addError("DataSourceInfo","Incorrect tag, expected <DataSourceInfo>",xmlStreamReader);
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("name"))
	{
		name_ = xmlStreamReader->attributes().value("name").toString();
	}
	else
	{
		addError("DataSourceInfo","Data missing name attribute",xmlStreamReader);
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("tableName"))
	{
		tableName_ = xmlStreamReader->attributes().value("tableName").toString();
	}
	else
	{
		addError("DataSourceInfo","Data missing tableName attribute",xmlStreamReader);
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("resolution"))
	{
		resolution_ = xmlStreamReader->attributes().value("resolution").toString().toDouble();
	}
	else
	{
		addError("DataSourceInfo","Data missing resolution attribute",xmlStreamReader);
		return false;
	}


	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "DataSourceInfo") && !xmlStreamReader->atEnd())
	{
		xmlStreamReader->readNext();
	}
	return true;
}

void DataSourceInfo::postDeserialize()
{

}

bool DataSourceInfo::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
