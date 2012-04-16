#include "TaskRunDataUnit.h"
#include "../memleakdetect.h"

namespace Picto {


TaskRunDataUnit::TaskRunDataUnit()
{
}

TaskRunDataUnit::TaskRunDataUnit(qulonglong startFrame, QString name, QString notes)
{
	startFrame_ = startFrame;
	endFrame_ = 0;
	name_ = name;
	notes_ = notes;
}

TaskRunDataUnit::TaskRunDataUnit(qulonglong startFrame, qulonglong endFrame, QString name, QString notes)
{
	startFrame_ = startFrame;
	endFrame_ = endFrame;
	name_ = name;
	notes_ = notes;
}

/*! \brief Turns the TaskRunDataUnitPackage into an XML fragment
 *
 *	
 */
bool TaskRunDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("TaskRunDataUnit");
	xmlStreamWriter->writeAttribute("startframe",QString("%1").arg(startFrame_));
	xmlStreamWriter->writeAttribute("endframe",QString("%1").arg(endFrame_));
	xmlStreamWriter->writeAttribute("name",name_);
	xmlStreamWriter->writeAttribute("notes",notes_);
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a TaskRunDataUnitPackage object.  Note that this deletes any existing data.
bool TaskRunDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "TaskRunDataUnit")
	{
		addError("TaskRunDataUnit","Incorrect tag, expected <TaskRunDataUnit>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "TaskRunDataUnit") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "TaskRunDataUnit")
		{
			if(xmlStreamReader->attributes().hasAttribute("startframe"))
			{
				startFrame_ = xmlStreamReader->attributes().value("startframe").toString().toLongLong();
			}
			else
			{
				addError("TaskRunDataUnit","Frame missing startframe attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("endframe"))
			{
				endFrame_ = xmlStreamReader->attributes().value("endframe").toString().toLongLong();
			}
			else
			{
				addError("TaskRunDataUnit","Frame missing endframe attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("name"))
			{
				name_ = xmlStreamReader->attributes().value("name").toString();
			}
			else
			{
				addError("TaskRunDataUnit","Frame missing name (name) attribute",xmlStreamReader);
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("notes"))
			{
				notes_ = xmlStreamReader->attributes().value("notes").toString();
			}
			else
			{
				addError("TaskRunDataUnit","Frame missing notes attribute",xmlStreamReader);
				return false;
			}
		}
		else
		{
			DataUnit::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}
	return true;
}

void TaskRunDataUnit::postDeserialize()
{

}

bool TaskRunDataUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
