#include "Experiment.h"

#include "../engine/PictoEngine.h"

namespace Picto {

Experiment::Experiment()
:latestSyntaxVersion_("0.0.1")
{
	
	AddDefinableProperty("SyntaxVersion","");
	//DefinePlaceholderTag("Tasks");
	AddDefinableObjectFactory("Task",QSharedPointer<AssetFactory>(new AssetFactory(1,-1,AssetFactory::NewAssetFnPtr(Picto::Task::Create))));
}

QSharedPointer<Experiment> Experiment::Create()
{
	QSharedPointer<Experiment> newExperiment(new Experiment());
	newExperiment->setSelfPtr(newExperiment);
	return newExperiment;
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
		taskList.append(task->getName());
	}
	return taskList;
}

QSharedPointer<Task> Experiment::getTaskByName(QString taskName)
{
	QSharedPointer<Task> returnVal;
	if(tasks_.isEmpty())
		return returnVal;
	//search through tasks_ for a matching task!
	//note that the taskname here may have had all of it's whitespace 
	//removed, so we need to check that possibility
	foreach(QSharedPointer<Task> task, tasks_)
	{
		if(task->getName() == taskName ||
		   task->getName().simplified().remove(' ') == taskName)
		{	
			returnVal = task;
			break;
		}
	}
	return returnVal;
}

bool Experiment::runTask(QString taskName, QSharedPointer<Engine::PictoEngine> engine)
{
	if(tasks_.isEmpty())
		return false;

	//search through tasks_ for a matching task and run it!
	//note that the taskname here may have had all of it's whitespace 
	//removed, so we need to check that possibility
	QSharedPointer<Task> task = getTaskByName(taskName);
	if(!task)
		return false;
	engine->clearEngineCommand();
	engine->startAllSignalChannels();
	bool success = task->run(engine);
	engine->stopAllSignalChannels();

	return success;
	//foreach(QSharedPointer<Task> task, tasks_)
	//{
	//	if(task->getName() == taskName ||
	//	   task->getName().simplified().remove(' ') == taskName)
	//	{	
	//		engine->clearEngineCommand();
	//		engine->startAllSignalChannels();
	//		bool success = task->run(engine);
	//		engine->stopAllSignalChannels();

	//		return success;
	//	}
	//}
	//return false;
}


//! Jumps to the specified task and state
bool Experiment::jumpToState(QStringList path, QString state)
{
	QString taskName = path.takeFirst();
	foreach(QSharedPointer<Task> task, tasks_)
	{
		if(task->getName() == taskName ||
		   task->getName().simplified().remove(' ') == taskName)
		{	
			task->jumpToState(path,state);
			return true;
		}
	}
	return false;
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

//bool Experiment::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("Experiment");
//	xmlStreamWriter->writeTextElement("Name", propertyContainer_->getPropertyValue("Name").toString());
//
//	xmlStreamWriter->writeStartElement("Tasks");
//	foreach(QSharedPointer<Task> task, tasks_)
//	{
//		task->toXml(xmlStreamWriter);
//	}
//	xmlStreamWriter->writeEndElement(); //Tasks
//
//	xmlStreamWriter->writeEndElement(); //Experiment
//	
//	
//	return true;
//}
//
////! Turns an XML fragment into an Experiment
//bool Experiment::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->getName() != "Experiment")
//	{
//		addError("Experiment","Incorrect tag, expected <Experiment>",xmlStreamReader);
//		return false;
//	}
//	
//	//clear out the existing experiment
//	tasks_.clear();
//	
//
//	xmlStreamReader->readNext();
//	
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->getName().toString() == "Experiment") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->getName().toString();
//		if(name == "Name")
//		{
//			setName(xmlStreamReader->readElementText());
//		}
//		else if(name == "Tasks")
//		{
//			//do nothing
//		}
//		else if(name == "Task")
//		{
//			QSharedPointer<Task> newTask(new Task);
//			if(!newTask->fromXml(xmlStreamReader))
//			{
//				addError("Experiment", "Task failed to deserialize correctly", xmlStreamReader);
//				return false;
//			}
//			addTask(newTask);
//		}
//		else
//		{
//			addError("Experiment", "Unexpected tag", xmlStreamReader);
//			return false;
//		}		
//
//		xmlStreamReader->readNext();
//	}
//
//	//Normally we return an error if we're "atEnd", but since Experiment is a top-level
//	//XML fragment, we probably are at the end.
//
//	return true;
//}

void Experiment::postSerialize()
{
	UIEnabled::postSerialize();
	QString experimentSyntaxVer = propertyContainer_->getPropertyValue("SyntaxVersion").toString();
	if(experimentSyntaxVer == "")
		propertyContainer_->setPropertyValue("SyntaxVersion",latestSyntaxVersion_);
	
	//clear out the existing experiment
	tasks_.clear();
	QList<QSharedPointer<Asset>> newTasks = getGeneratedChildren("Task");
	foreach(QSharedPointer<Asset> newTask,newTasks)
	{
		addTask(newTask.staticCast<Task>());
	}
}

bool Experiment::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
