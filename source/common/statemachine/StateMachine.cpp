#include "StateMachine.h"

#include "State.h"
#include "Result.h"
#include "FlowElement.h"

namespace Picto {

StateMachine::StateMachine()
{
	propertyContainer_.setPropertyValue("Type", "StateMachine");
	propertyContainer_.addProperty(Property(QVariant::String,"Level",""));
	propertyContainer_.addProperty(Property(QVariant::String,"Initial Element",""));

}

void StateMachine::setLevel(QString level)
{
	propertyContainer_.setPropertyValue("Level", QVariant(level));
}

QString StateMachine::getLevel()
{
	QString level = propertyContainer_.getPropertyValue("Level").toString();
	return level;
}

//! \brief Adds a transition to this state machine
void StateMachine::addTransition(QSharedPointer<Transition> transition)
{
	transitions_.insert(transition->getSource(), transition);
}

//! \brief adds an element to this state machine
void StateMachine::addElement(QSharedPointer<StateMachineElement> element)
{
	elements_[element->getName()] = element;

	//if the element is a result, we need to add to our result list
	if(element->type() == "Result")
	{
		addResult(element.staticCast<Result>());
	}

	//Add all of our parameters to the element
	element->addParameters(parameterContainer_);
}

void StateMachine::addParameter(QSharedPointer<Parameter> parameter)
{
	parameterContainer_.addParameter(parameter);
	localParameterContainer_.addParameter(parameter);
}


bool StateMachine::setInitialElement(QString elementName)
{
	if(elements_.contains(elementName))
	{
		propertyContainer_.setPropertyValue("Initial Element", elementName);
		return true;
	}
	else
	{
		return false;
	}
}
/*! \brief Confirms that the transitions are legal
 *
 *	This function will mostly be used in the deserialization function to
 *	confirm that the passed in XML is legal.  However it is possible that
 *	it could be used elsewhere...
 *
 *	The validation process consists of checking each transition to confirm
 *	that the source, sourceResult, and destination all exist.
 */

bool StateMachine::validateTransitions()
{
	//Confirm that all transitions are legal
	foreach(QSharedPointer<Transition> tran, transitions_)
	{
		QString source = tran->getSource();
		if(!elements_.contains(source))
			return false;
		
		QString dest = tran->getDestination();
		if(!elements_.contains(dest))
			return false;

		QString sourceResult = tran->getSourceResult();
		QSharedPointer<StateMachineElement> sourceElement;
		sourceElement = elements_.value(source);
		if(!sourceElement->getResultList().contains(sourceResult))
			return false;
	}

	//Confrim that every element has all of its transitions connected
	foreach(QSharedPointer<StateMachineElement> element, elements_)
	{
		QStringList results = element->getResultList();
		foreach(QString result, results)
		{
			bool found = false;
			foreach(QSharedPointer<Transition> tran, transitions_.values(element->getName()))
			{
				if(tran->getSourceResult() == result)
				{
					found = true;
					continue;
				}
			}
			if(!found)
				return false;
		}			
	}

	//Confirm that the initial element is a real element
	QString initialElement = propertyContainer_.getPropertyValue("Initial Element").toString();
	if(!elements_.contains(initialElement))
		return false;

	//Do the same for all state machines contained within this state machine
	foreach(QSharedPointer<StateMachineElement> element, elements_)
	{
		if(element->type() == "StateMachine")
		{
			if(!element.staticCast<StateMachine>()->validateTransitions())
				return false;
		}
	}

	//If we made it this far, all the transitions are "legal"
	return true;
}

QString StateMachine::run()
{
	//! \TODO If we didn't mind slowing down, we could call ValidateTransitions here
	//Maybe we should have a flag that requires us to validate the first time we run?

	QString currElementName;
	QString nextElementName;
	QSharedPointer<StateMachineElement> currElement;

	currElementName = propertyContainer_.getPropertyValue("Initial Element").toString();
	currElement = elements_.value(currElementName);
	
	while(true)
	{
		QString result = currElement->run();

		//Find the transition from our current source with a SourceResult string that matches the result
		//Yeah, this is kind of ugly...
		nextElementName = "";
		foreach(QSharedPointer<Transition> tran, transitions_.values(currElementName))
		{
			if(tran->getSourceResult() == result)
			{
				nextElementName = tran->getDestination();
				break;
			}
		}
		//! \TODO come up with a more elegant error handling scheme...
		Q_ASSERT(elements_.contains(nextElementName));

		currElement = elements_[nextElementName];
		currElementName = nextElementName;

		if(currElement->type() == "Result")
			return currElement->run();
	}

}

/*!	\brief Turns a StateMachine into an XML fragment
 *
 *	The XML for a StateMachine looks like this:
 *	<StateMachine>
 *		<Name>My State Machine</Name>
 *		<Level>Stage</Level>
 *		<InitialElement>The name of an element</InitialElement>
 *		<StateMachineElements>
 *			<StateMachineElement type="State">
 *				...
 *			</StateMachineElement>
 *			<StateMachineElement type="Result">
 *				...
 *			</StateMachineElement>
 *		</StateMachineElements>
 *		<Parameters>
 *			..
 *		</Parameters>
 *		<Transitions>
 *			<Transition>
 *				...
 *			</Transition>
 *		</Transitions>
 *		<Results>
 *			<Result>
 *				<Name>Succes</Name>
 *			<Result>
 *		</Results>
 *	</StateMachine>
 */
bool StateMachine::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("StateMachine");
	xmlStreamWriter->writeTextElement("Name",propertyContainer_.getPropertyValue("Name").toString());
	xmlStreamWriter->writeTextElement("Level",propertyContainer_.getPropertyValue("Level").toString());
	xmlStreamWriter->writeTextElement("InitialElement",propertyContainer_.getPropertyValue("Initial Element").toString());

	//StateMachineElements
	xmlStreamWriter->writeStartElement("StateMachineElements");
	foreach(QSharedPointer<StateMachineElement> element, elements_)
	{
		element->serializeAsXml(xmlStreamWriter);
	}
	xmlStreamWriter->writeEndElement(); //StateMachineElements

	//Parameters
	//We only need to output the local parameters (otherwise we'd end up with
	//a really long and confusing list)
	localParameterContainer_.serializeAsXml(xmlStreamWriter);

	//Transitions
	xmlStreamWriter->writeStartElement("Transitions");
	foreach(QSharedPointer<Transition> transition, transitions_)
	{
		transition->serializeAsXml(xmlStreamWriter);
	}
	xmlStreamWriter->writeEndElement(); //Transitions

	//Results
	//Technically this is redundant, since the results are all listed in the
	//STateMachineElements section, but these are included for readability
	serializeResults(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //StateMachine

	return true;
}

bool StateMachine::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateMachine")
	{
		addError("StateMachine","Incorrect tag, expected <StateMachine>",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateMachine") && !xmlStreamReader->atEnd())
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
			propertyContainer_.setPropertyValue("Name",xmlStreamReader->readElementText());
		}
		else if(name == "InitialElement")
		{
			propertyContainer_.setPropertyValue("Initial Element",xmlStreamReader->readElementText());
		}
		else if(name == "Level")
		{
			propertyContainer_.setPropertyValue("Level",xmlStreamReader->readElementText());
		}
		else if(name == "StateMachineElements")
		{
			xmlStreamReader->readNext();
			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateMachineElements") && !xmlStreamReader->atEnd())
			{
				if(!xmlStreamReader->isStartElement())
				{
					//Do nothing unless we're at a start element
					xmlStreamReader->readNext();
					continue;
				}

				name = xmlStreamReader->name().toString();
				if(name == "StateMachineElement")
				{
					//I could use a StateMachineElementFactory here, but that seems like overkill, since there
					//aren't that many StateMachineElement types
					QString elementType = xmlStreamReader->attributes().value("type").toString();
					QSharedPointer<StateMachineElement> newElement;
					if(elementType == "FlowElement")
					{
						newElement = QSharedPointer<StateMachineElement>(new FlowElement());
					}
					else if(elementType == "Result")
					{
						newElement = QSharedPointer<StateMachineElement>(new Result());
					}
					else if(elementType == "State")
					{
						newElement = QSharedPointer<StateMachineElement>(new State());
					}
					else
					{
						addError("StateMachine", "Unexpected type of StateMachineElement", xmlStreamReader);
						return false;
					}

					if(!newElement->deserializeFromXml(xmlStreamReader))
					{
						addError("StateMachine", "Failed to deserialize <StateMachineElement>", xmlStreamReader);
						return false;
					}
					newElement->addParameters(parameterContainer_);
					addElement(newElement);
				}
				else if(name == "StateMachine")
				{
					QSharedPointer<StateMachine> newMachine(new StateMachine);
					if(!newMachine->deserializeFromXml(xmlStreamReader))
					{
						addError("StateMachine", "Failed to deserialize <StateMachine> within <StateMachineElements>", xmlStreamReader);
						return false;
					}
					newMachine->addParameters(parameterContainer_);
					addElement(newMachine);
				}
				else
				{
					addError("StateMachine", "Unexpected tag encountered within <StateMachineElements>", xmlStreamReader);
					return false;
				}
				xmlStreamReader->readNext();
			}
			if(xmlStreamReader->atEnd())
			{
				addError("StateMachine", "Unexpected end of document encountered within <StateMachineElements>", xmlStreamReader);
				return false;
			}
		}
		else if(name == "Parameters")
		{
			if(!localParameterContainer_.deserializeFromXml(xmlStreamReader))
			{
				addError("StateMachine","Failed to deserialize <Parameters>",xmlStreamReader);
				return false;
			}
			
			//Now our local parameter container is filled up, so we want
			//to copy all of these values to the global container (which may already
			//contain parameters)
			addParameters(localParameterContainer_);
		}
		else if(name == "Transitions")
		{
			xmlStreamReader->readNext();
			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Transitions") && !xmlStreamReader->atEnd())
			{
				if(!xmlStreamReader->isStartElement())
				{
					//Do nothing unless we're at a start element
					xmlStreamReader->readNext();
					continue;
				}

				name = xmlStreamReader->name().toString();
				if(name == "Transition")
				{
					QSharedPointer<Transition> newTrans(new Transition());
					if(!newTrans->deserializeFromXml(xmlStreamReader))
					{
						addError("StateMachine", "Failed to deserialize <Transition>", xmlStreamReader);
						return false;
					}
					addTransition(newTrans);
				}
				else
				{
					addError("StateMachine", "unexpected tag within <Transitions>", xmlStreamReader);
					return false;
				}
				xmlStreamReader->readNext();

			}
			if(xmlStreamReader->atEnd())
			{
				addError("StateMachine", "Unexpected end of document encountered within <Transitions>", xmlStreamReader);
				return false;
			}
		}
		else if(name == "Results")
		{
			//The results were already generated when they were encountered in the 
			//StateMachineElement section (they're just included in the XML for readability)
			//So, we'll skip reading this section.
			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Results") && !xmlStreamReader->atEnd())
			{
				xmlStreamReader->readNext();
			}
			if(xmlStreamReader->atEnd())
			{
				addError("StateMachine", "Unexpected end of document encountered within <Results>", xmlStreamReader);
				return false;
			}
		}
		else
		{
			addError("StateMachine", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("StateMachine", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	return true;

}
}; //namespace Picto
