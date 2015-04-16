#include "DataSourceInfo.h"
#include "../memleakdetect.h"

namespace Picto {

DataSourceInfo::DataSourceInfo()
{
}

/*! \brief Builds a new DataSourceInfo object.
 *	@param name The of the DataSource (ie. "lfp" or "spikes")
 *	@param tableName The name of the table to be associated with data source
 *	@param resolution time between each sample in the source (for spikes this is in reference to the waveform)
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
		addError("Incorrect tag, expected <DataSourceInfo>");
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("name"))
	{
		name_ = xmlStreamReader->attributes().value("name").toString();
	}
	else
	{
		addError("Data missing name attribute");
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("tableName"))
	{
		tableName_ = xmlStreamReader->attributes().value("tableName").toString();
	}
	else
	{
		addError("Data missing tableName attribute");
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("resolution"))
	{
		resolution_ = xmlStreamReader->attributes().value("resolution").toString().toDouble();
	}
	else
	{
		addError("Data missing resolution attribute");
		return false;
	}


	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "DataSourceInfo") && !xmlStreamReader->atEnd())
	{
		xmlStreamReader->readNext();
	}
	return true;
}

}; //namespace Picto
