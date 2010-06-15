#include "Experiment.h"

#include "../engine/PictoEngine.h"

namespace Picto {

Experiment::Experiment() :
	//formatID_("1.0.0.0"),
	propertyContainer_("Experiment")
{
	propertyContainer_.addProperty(Property(QVariant::String,"Name","Unnamed Experiment"));
}

//! returns the name of this experiment
QString Experiment::name()
{
	return propertyContainer_.getPropertyValue("Name").toString();
}

//! Sets the name of this experiment
void Experiment::setName(QString name)
{
	propertyContainer_.setPropertyValue("Name",name);
}

void Experiment::addTask(QSharedPointer<Task> task)
{
	tasks_.append(task);
}

QStringList Experiment::getTaskNames()
{
	QStringList taskList;
	foreach(QSharedPointer<Task> task, tasks_)
	{
		taskList.append(task->name());
	}
	return taskList;
}


bool Experiment::runTask(QString taskName, QSharedPointer<Engine::PictoEngine> engine)
{
	if(tasks_.isEmpty())
		return false;

	//search through tasks_ for a matching task and run it!
	//note that the taskname here may have had all of it's whitespace 
	//removed, so we need to check that possibility
	foreach(QSharedPointer<Task> task, tasks_)
	{
		///////////////////////
		QString debugginStr = task->name();
		///////////////////////


		if(task->name() == taskName ||
		   task->name().simplified().remove(' ') == taskName)
		{	
			engine->clearEngineCommand();
			engine->startAllSignalChannels();
			bool success = task->run(engine);
			engine->stopAllSignalChannels();

			return success;
		}
	}
	return false;
}

//! Clears the experiment
void Experiment::clear()
{
	tasks_.clear();	
	propertyContainer_.setPropertyValue("Name","Unnamed Experiment");
}

/*! \brief Turns this experiment into an XML fragment
 *
 *	The XML for an experiment looks like this:
 *	
 *	<Experiment>
 *		<Name>SomeExperiment</Name>
 *		<Tasks>
 *			<Task>
 *				...
 *			<Task>
 *		<Tasks>
 *	</Experiment>
 */

bool Experiment::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Experiment");
	xmlStreamWriter->writeTextElement("Name", propertyContainer_.getPropertyValue("Name").toString());

	xmlStreamWriter->writeStartElement("Tasks");
	foreach(QSharedPointer<Task> task, tasks_)
	{
		task->serializeAsXml(xmlStreamWriter);
	}
	xmlStreamWriter->writeEndElement(); //Tasks

	xmlStreamWriter->writeEndElement(); //Experiment
	
	
	return true;
}

//! Turns an XML fragment into an Experiment
bool Experiment::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Experiment")
	{
		addError("Experiment","Incorrect tag, expected <Experiment>",xmlStreamReader);
		return false;
	}
	
	//clear out the existing experiment
	tasks_.clear();
	

	xmlStreamReader->readNext();
	
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Experiment") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Name")
		{
			setName(xmlStreamReader->readElementText());
		}
		else if(name == "Tasks")
		{
			//do nothing
		}
		else if(name == "Task")
		{
			QSharedPointer<Task> newTask(new Task);
			if(!newTask->deserializeFromXml(xmlStreamReader))
			{
				addError("Experiment", "Task failed to deserialize correctly", xmlStreamReader);
				return false;
			}
			addTask(newTask);
		}
		else
		{
			addError("Experiment", "Unexpected tag", xmlStreamReader);
			return false;
		}		

		xmlStreamReader->readNext();
	}

	//Normally we return an error if we're "atEnd", but since Experiment is a top-level
	//XML fragment, we probably are at the end.

	return true;
}


}; //namespace Picto
