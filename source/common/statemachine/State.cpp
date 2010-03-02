#include "State.h"
#include "../controlelements/ControlElementFactory.h"

namespace Picto {

State::State()
: scene_(QSharedPointer<Scene>(new Scene))
{
	propertyContainer_.setPropertyValue("Type","State");

	//at some point, these should actually do something, but for the moment
	//we'll leave them as -1 to show that they aren't being used
	revision_ = -1;
	engineNeeded_ = -1;

}

QString State::run()
{
	//Start up all of the control elements
	foreach(QSharedPointer<ControlElement> control, controlElements_)
	{
		control->start();
	}

	while(true)
	{
		//Draw the scene
		scene_->render();

		//check all of the control elements
		foreach(QSharedPointer<ControlElement> control, controlElements_)
		{
			if(control->isDone())
				return control->getResult();
		}

		//If there are no control elements, then we just return with an empty string
		if(controlElements_.isEmpty())
			return "";
	}
}

void State::addControlElement(QSharedPointer<ControlElement> controlElement)
{
	//Add the control element to our map
	controlElements_.insert(controlElement->getName(),controlElement);

	//Add the controlElement's results to our list of possible results
	QStringList controlElementResults = controlElement->getResultList();
	results_ = results_ +controlElementResults;
}

void State::removeControlElement(QString controlElementName)
{
	if(!controlElements_.contains(controlElementName))
		return;

	//remove the results affiliated with this control element
	foreach(QString result, controlElements_[controlElementName]->getResultList())
	{
		results_.removeOne(result);
	}

	//remove the element
	controlElements_.remove(controlElementName);


}



/*!	\Brief Turns a state object into an XML framgent.
 *
 *	The XML fragment for a state will look like this:
 * 
 *	<StateMachineElement type="State">
 *		<Revision>1</Revision>
 *		<EngineNeeded>1</EngineNeeded>
 *		<Name>Fixation</Name>
 *		<ControlElements>
 *			<ControlElement type="TargetController">
 *				...
 *			</ControlElement>
 *		</ControlElements>
 *		<Scene>
 *			...
 *		</Scene>
 *		<Results>
 *			<!-- We'll list all possible results from the ControlElements here -->
 *			<Result>
 *				<Name>..</Name>
 *			</Result>
 *		</Results>
 *	</StateMachineElement>
 */
bool State::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("StateMachineElement");
	xmlStreamWriter->writeAttribute("type","State");
	
	xmlStreamWriter->writeTextElement("Revision",QString("%1").arg(revision_));
	xmlStreamWriter->writeTextElement("EngineNeeded",QString("%1").arg(engineNeeded_));
	xmlStreamWriter->writeTextElement("Name", propertyContainer_.getPropertyValue("Name").toString());

	xmlStreamWriter->writeStartElement("ControlElements");
	foreach(QSharedPointer<ControlElement> c, controlElements_)
	{
		c->serializeAsXml(xmlStreamWriter);
	}
	xmlStreamWriter->writeEndElement();

	scene_->serializeAsXml(xmlStreamWriter);

	serializeResults(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //State
	return true;
}

/*!	\Brief Turns a an XML fragment into a State object */
bool State::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateMachineElement")
	{
		addError("FlowElement","Incorrect tag, expected <StateMachineElement>",xmlStreamReader);
		return false;
	}
	if(xmlStreamReader->attributes().value("type").toString() != type())
	{
		addError("FlowElement","Incorrect type of StateMachineElement, expected State",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateMachineElement") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Revision")
		{
			bool ok;
			revision_= xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
			{
				addError("State","Revision value not an integer", xmlStreamReader);
				return false;
			}
		}
		else if(name == "EngineNeeded")
		{
			bool ok;
			engineNeeded_= xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
			{
				addError("State","EngineNeeded value not an integer", xmlStreamReader);
				return false;
			}
		}
		else if(name == "Name")
		{
			propertyContainer_.setPropertyValue("Name",QVariant(xmlStreamReader->readElementText()));
		}
		else if(name == "ControlElements")
		{
			//do nothing
		}
		else if(name == "ControlElement")
		{
			ControlElementFactory factory;
			QString controllerType = xmlStreamReader->attributes().value("type").toString();
			QSharedPointer<ControlElement> newController = factory.generateControlElement(controllerType);
			if(!newController->deserializeFromXml(xmlStreamReader))
			{
				addError("State", "Failed to deserialize <ControlElement>", xmlStreamReader);
				return false;
			}
			addControlElement(newController);
		}
		else if(name == "Scene")
		{
			QSharedPointer<Scene> newScene(new Scene);
			if(!newScene->deserializeFromXml(xmlStreamReader))
			{
				addError("State", "Failed to deserialize <Scene>", xmlStreamReader);
				return false;
			}
			scene_ = newScene;
		}
		else if(name == "Results")
		{
			if(!deserializeResults(xmlStreamReader))
			{
				addError("State", "Failed to deserialize <Results>", xmlStreamReader);
				return false;
			}
		}

		else
		{
			addError("State", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("State", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	return true;
}


}; //namespace Picto
