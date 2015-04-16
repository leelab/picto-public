#include "SignalChannelInfo.h"
#include "../memleakdetect.h"

namespace Picto {

SignalChannelInfo::SignalChannelInfo()
{
}

/*! \brief Builds a SignalChannelInfo object.
 *	@param name The signal channel name
 *	@param tableName The name of the table associated with the signal channel
 *	@param stateVariableId The State Variable ID associated with the signal channel
 *	@param resolution time between each group of subchannel samples (seconds)
 *	@param subchannels number of subchannels in this SignalChannel
 *	@param subchannelNames A comma seperated string describing the subchannels
 *	in order of appearance(i.e. "xPos,yPos")
 */
SignalChannelInfo::SignalChannelInfo(QString name,
									 QString tableName,
									 int stateVariableId,
									 double resolution,
									 int subchannels,
									 QString subchannelNames)
{
	name_ = name;
	tableName_ = tableName;
	stateVariableId_ = stateVariableId;
	resolution_ = resolution;
	subchannels_ = subchannels;
	subchannelNames_ = subchannelNames;
}


/*! \brief Turns the SignalChannelInfo into an XML fragment
 */
bool SignalChannelInfo::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("SignalChannelInfo");
	xmlStreamWriter->writeAttribute("name",name_);
	xmlStreamWriter->writeAttribute("tableName",tableName_);
	xmlStreamWriter->writeAttribute("stateVariableId",QString("%1").arg(stateVariableId_));
	xmlStreamWriter->writeAttribute("resolution",QString("%1").arg(resolution_));
	xmlStreamWriter->writeAttribute("numSubchannels",QString("%1").arg(subchannels_));
	xmlStreamWriter->writeAttribute("subChannelNames",subchannelNames_);
	xmlStreamWriter->writeEndElement();
	return true;
}
/*! Converts XML into a SignalChannelInfo object.  Note that this deletes any existing data.*/
bool SignalChannelInfo::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "SignalChannelInfo")
	{
		addError("Incorrect tag, expected <SignalChannelInfo>");
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

	if(xmlStreamReader->attributes().hasAttribute("stateVariableId"))
	{
		stateVariableId_ = xmlStreamReader->attributes().value("stateVariableId").toString().toInt();
	}
	else
	{
		addError("Data missing stateVariableId attribute");
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

	if(xmlStreamReader->attributes().hasAttribute("numSubchannels"))
	{
		subchannels_ = xmlStreamReader->attributes().value("numSubchannels").toString().toInt();
	}
	else
	{
		addError("Data missing numSubchannels attribute");
		return false;
	}

	if(xmlStreamReader->attributes().hasAttribute("subChannelNames"))
	{
		subchannelNames_ = xmlStreamReader->attributes().value("subChannelNames").toString();
	}
	else
	{
		addError("Data missing subChannelNames attribute");
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "SignalChannelInfo") && !xmlStreamReader->atEnd())
	{
		xmlStreamReader->readNext();
	}
	return true;
}

}; //namespace Picto
