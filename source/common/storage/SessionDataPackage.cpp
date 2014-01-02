#include "SessionDataPackage.h"
#include "../memleakdetect.h"

using namespace Picto;

SessionDataPackage::SessionDataPackage()
{
}

/*! \brief Adds the input TaskRunDataUnit to this object's runs lookup table, indexed by the run's DataId.
*/
void SessionDataPackage::setTaskRun(QSharedPointer<TaskRunDataUnit> run)
{
	if(!run)
		return;
	runsMap_[run->getDataID()] = run;
	runsList_.clear();
}

/*! \brief Returns the TaskRunDataUnit with the input index.  This is an index from 0 to the number of contained runs minus 1
 *	where 0 is the first run in the session and total-1 is the last.
 *	\details If not such run exists, an empty TaskRunDataUnit pointer will be returned.
*/
QSharedPointer<TaskRunDataUnit> SessionDataPackage::getTaskRunByIndex(int index)
{
	if(index >= getNumRuns())
		return QSharedPointer<TaskRunDataUnit>();
	if(runsList_.isEmpty())
		runsList_ = runsMap_.values();
	return runsList_[index];

}

/*! \brief Returns the TaskRunDataUnit with the input DataId.
*	\details If not such run exists, an empty TaskRunDataUnit pointer will be returned.
*/
QSharedPointer<TaskRunDataUnit> SessionDataPackage::getTaskRunByRunId(qulonglong id)
{
	if(!runsMap_.contains(id))
		return QSharedPointer<TaskRunDataUnit>();
	return(runsMap_.value(id));
}

/*! \brief Returns a lookup table of TaskRunDataUnits indexed by their DataIds.*/
QMap<qulonglong,QSharedPointer<TaskRunDataUnit>> SessionDataPackage::getRunsMap()
{
	return QMap<qulonglong,QSharedPointer<TaskRunDataUnit>>(runsMap_);
}

/*! \brief Turns the SessionDataPackage into an XML fragment
 */
bool SessionDataPackage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("SessionDataPackage");

	foreach(QSharedPointer<TaskRunDataUnit> run,runsMap_)
	{
		run->toXml(xmlStreamWriter);
	}

	xmlStreamWriter->writeEndElement(); //SessionDataPackage

	return true;
}
/*! Converts XML into a SessionDataPackage object.  Note that this deletes any existing data.*/
bool SessionDataPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "SessionDataPackage")
	{
		addError("Incorrect tag, expected <SessionDataPackage>");
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "SessionDataPackage") && !xmlStreamReader->atEnd())
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
			QSharedPointer<TaskRunDataUnit> run = QSharedPointer<TaskRunDataUnit>(new TaskRunDataUnit());
			run->fromXml(xmlStreamReader);
			setTaskRun(run);
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

void SessionDataPackage::postDeserialize()
{

}

bool SessionDataPackage::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}
