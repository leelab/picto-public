#include "Task.h"

namespace Picto {

Task::Task() :
	propertyContainer_("Task")
{
	propertyContainer_.addProperty(Property(QVariant::String,"Name","Unnamed Task"));
}

//! returns the name of this task
QString Task::name()
{
	return propertyContainer_.getPropertyValue("Name").toString();
}

//! Sets the name of this task
void Task::setName(QString name)
{
	propertyContainer_.setPropertyValue("Name",name);
}

bool Task::run(QSharedPointer<Engine::PictoEngine> engine)
{
	if(stateMachine_.isNull())
	{
		return false;
	}
	else
	{
		if(engine->slaveMode())
		{
			StateMachine::resetSlaveElements();
			stateMachine_->runAsSlave(engine);
		}
		else
			stateMachine_->run(engine);
		return true;
	}
}

/*! \brief Turns this task into an XML fragment
 *
 *	The XML for a task looks like this:
 *	
 *	<Task>
 *		<Name>SomeExperiment</Name>
 *		<StateMachine>
 *			...
 *		</StateMachine>
 *	</Task>
 */
bool Task::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Task");
	xmlStreamWriter->writeTextElement("Name", propertyContainer_.getPropertyValue("Name").toString());

	xmlStreamWriter->writeStartElement("StateMachine");
	stateMachine_->serializeAsXml(xmlStreamWriter);
	xmlStreamWriter->writeEndElement(); //StateMachine

	xmlStreamWriter->writeEndElement(); //Task
	
	return true;
}

//! Turns XML into a Task
bool Task::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Task")
	{
		addError("Task","Incorrect tag, expected <Task>",xmlStreamReader);
		return false;
	}
	
	xmlStreamReader->readNext();
	
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Task") && !xmlStreamReader->atEnd())
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
		else if(name == "StateMachine")
		{
			stateMachine_ = QSharedPointer<StateMachine>(new StateMachine);
			if(!stateMachine_->deserializeFromXml(xmlStreamReader))
			{
				addError("Task","State Machine failed to deserialize", xmlStreamReader);
				return false;
			}
		}
		else
		{
			addError("Task", "Unexpected tag", xmlStreamReader);
			return false;
		}		

		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("Task", "Unexpected end of document", xmlStreamReader);
		return false;
	}
	if(stateMachine_->validateStateMachine())
	{
		return true;
	}
	else
	{
		addError("StateMachine","StateMachine failed validation",xmlStreamReader);
		return false;
	}
}

}; //namespace Picto
