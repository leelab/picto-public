#include "TaskRunDataUnit.h"
#include "../memleakdetect.h"

namespace Picto {


TaskRunDataUnit::TaskRunDataUnit()
{
}

/*! \brief Creates a TasnRunDataUnit object with the input startFrame DataId, run name, run notes, and
 *	flag indicating whether it will be saved.
 */
TaskRunDataUnit::TaskRunDataUnit(qulonglong startFrame, QString name, QString notes, bool saved)
{
	startFrame_ = startFrame;
	endFrame_ = 0;
	name_ = name;
	notes_ = notes;
	saved_ = saved;
}

/*! \brief Creates a TasnRunDataUnit object with the input startFrame DataId, endFrame DataId, run name, run notes, and
 *	flag indicating whether it will be saved.
 */
TaskRunDataUnit::TaskRunDataUnit(qulonglong startFrame, qulonglong endFrame, QString name, QString notes, bool saved)
{
	startFrame_ = startFrame;
	endFrame_ = endFrame;
	name_ = name;
	notes_ = notes;
	saved_ = saved;
}

/*! \brief Turns the TaskRunDataUnit into an XML fragment
 */
bool TaskRunDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("TaskRunDataUnit");
	xmlStreamWriter->writeAttribute("startframe",QString("%1").arg(startFrame_));
	xmlStreamWriter->writeAttribute("endframe",QString("%1").arg(endFrame_));
	xmlStreamWriter->writeAttribute("name",name_);
	xmlStreamWriter->writeAttribute("notes",notes_);
	xmlStreamWriter->writeAttribute("saved",QString("%1").arg(saved_));
	DataUnit::serializeDataID(xmlStreamWriter);
	xmlStreamWriter->writeEndElement();
	return true;
}
/*! Converts XML into a TaskRunDataUnit object.  Note that this deletes any existing data.
*/
bool TaskRunDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "TaskRunDataUnit")
	{
		addError("Incorrect tag, expected <TaskRunDataUnit>");
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
				addError("Frame missing startframe attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("endframe"))
			{
				endFrame_ = xmlStreamReader->attributes().value("endframe").toString().toLongLong();
			}
			else
			{
				addError("Frame missing endframe attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("name"))
			{
				name_ = xmlStreamReader->attributes().value("name").toString();
			}
			else
			{
				addError("Frame missing name (name) attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("notes"))
			{
				notes_ = xmlStreamReader->attributes().value("notes").toString();
			}
			else
			{
				addError("Frame missing notes attribute");
				return false;
			}

			if(xmlStreamReader->attributes().hasAttribute("saved"))
			{
				saved_ = xmlStreamReader->attributes().value("saved").toString().toInt();
			}
			else
			{
				addError("Frame missing saved attribute");
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

}; //namespace Picto
