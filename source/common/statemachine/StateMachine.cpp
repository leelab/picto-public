#include "StateMachine.h"

#include "State.h"
#include "Result.h"
#include "FlowElement.h"
#include "ScriptElement.h"

#include "../engine/PictoEngine.h"
#include "../timing/Timestamper.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"

namespace Picto {

int StateMachine::trialEventCode_;
int StateMachine::trialNum_;

StateMachine::StateMachine()
: scriptingInit_(false)
{
	propertyContainer_.setPropertyValue("Type", "StateMachine");
	propertyContainer_.addProperty(Property(QVariant::String,"Initial Element",""));

	Property levelsEnumProp(QtVariantPropertyManager::enumTypeId(), "Level",0);
	//Note that this is the same order as the enum to allow the enum to be
	//used as an index
	levelEnumStrs_<<"Stage"<<"Trial"<<"Task"<<"Experiment"; 
	levelsEnumProp.addAttribute("enumNames",levelEnumStrs_);
	propertyContainer_.addProperty(levelsEnumProp);

	trialEventCode_ = 0;
}

void StateMachine::setLevel(StateMachineLevel::StateMachineLevel level)
{
	propertyContainer_.setPropertyValue("Level", QVariant(level));
}

StateMachineLevel::StateMachineLevel StateMachine::getLevel()
{
	return (StateMachineLevel::StateMachineLevel) propertyContainer_.getPropertyValue("Level").toInt();
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
/*! \brief Confirms that the state machine is legal
 *
 *	This function will mostly be used in the deserialization function to
 *	confirm that the passed in XML is legal.  However it is possible that
 *	it could be used elsewhere...
 *
 *	The validation process consists of:
 *		- check that the contained state machines are of a legal level
 *		- check each transition to confirm that the source, 
 *		  sourceResult, and destination all exist.
 *		- check that every element has all its results connected to
 *		  a transition
 *		- check that the initial element is a real element
 *		- validate all contained state machines
 *		
 */

bool StateMachine::validateStateMachine()
{
	//Confirm that any contained StateMachines are of the correct level
	foreach(QSharedPointer<StateMachineElement> element, elements_)
	{
		if(element->type() == "StateMachine")
		{
			StateMachineLevel::StateMachineLevel containedLevel;
			containedLevel = element.staticCast<StateMachine>()->getLevel();

			StateMachineLevel::StateMachineLevel thisLevel;
			thisLevel = getLevel();

			if(thisLevel == StateMachineLevel::Stage)
			{
				if(containedLevel > thisLevel)
					return false;
			}
			else
			{
				if(containedLevel >= thisLevel)
					return false;
			}
		}
		
	}

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
			{
				QString elementName = element->getName();
				QString unconnectedResult = result;
				return false;
			}
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
			if(!element.staticCast<StateMachine>()->validateStateMachine())
				return false;
		}
	}

	//If we made it this far, all the transitions are "legal"
	return true;
}

QString StateMachine::run()
{
	if(!scriptingInit_)
	{
		if(!initScripting(qsEngine_))
		{
			//! \TODO Make some sort of intelligent error reporting...
			return "scriptingError";
		}
	}

	//Generate the start trial event
	if(getLevel() == StateMachineLevel::Trial)
	{
		//The Picto event is generated first in an attempt to closely align the
		//times.  Generating a Picto event takes almost no time, while generating
		//an event code, takes 250 us.  I am assuming that the event gets recorded as soon
		//as the Plexon/TDT notices that the event strobe line has gone high, so generating
		//the Picto event first will place the events in closer proximity.
		trialEventCode_++;
		trialNum_++;
		sendStartTrialToServer();
		Engine::PictoEngine::generateEvent(trialEventCode_);
	}
	//Reset trialNum_ if we just entered a new Task
	if(getLevel() == StateMachineLevel::Task)
	{
		trialNum_ = 0;
	}


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
		{
			if(getLevel() == StateMachineLevel::Trial)
				sendEndTrialToServer();
			Engine::PictoEngine::generateEvent(trialEventCode_);
			return currElement->run();
		}
	}

}


/*!	\brief Sets up the script engines for this state machine
 *
 *	To make script execution more efficient, we attempt to preload everything
 *	Each state machine maintains its own script engine, which is used by all of
 *	the contained ScriptElements.  The initialization process consists of the 
 *	following:
 *		1. Bind all of the parameters to the scripting engine
 *		2. Call initScripting on all contained elements.  This has the following effects:
 *			a. Creates functions in our local engine for all ScriptElements
 *			b. Sets up a local engine within all States
 *			c. Calls this function on all contained StateMachines
 */
bool StateMachine::initScripting(QScriptEngine &qsEngine)
{
	//Since the StateMachine maintains its own engine, we can ignore the passed in engine
	Q_UNUSED(qsEngine);

	//bind the Parameters
	parameterContainer_.bindToScriptEngine(qsEngine_);

	//initialize scripting on all of the contained elements
	foreach(QSharedPointer<StateMachineElement> element, elements_)
	{
		if(!element->initScripting(qsEngine_))
			return false;
	}

	scriptingInit_ = true;
	return true;
}


/*!	\brief Sends a StartTrial event to PictoServer
 *
 *	At the begining of a Trial, we send a timestamped StartTrial event to PictoServer.
 *	This is done nearly simultaneously to sending a StartTrial event code to the
 *	neural recording device.  
 *
 *	The command used to do this is (the units of time are seconds)
 *		TRIAL /start PICTO/1.0
 *		Content-Length:???
 *		
 *		<Trial>
 *			<Time>8684354986.358943</Time>
 *			<EventCode>56</EventCode>
 *			<TrialNum>412</TrialNum> 	
 *		</Trial>
 */
void StateMachine::sendStartTrialToServer()
{
	//Create a TRIAL command
	QSharedPointer<ProtocolCommand> command(new ProtocolCommand("TRIAL /start PICTO/1.0"));

	//Create the content of the TRIAL command
	QString content;
	QXmlStreamWriter xmlWriter(&content);

	xmlWriter.writeStartElement("Trial");

	xmlWriter.writeStartElement("Time");
	Timestamper timestamper;
	xmlWriter.writeCharacters(QString("%1").arg(timestamper.stampSec(),0,'f',4));
	xmlWriter.writeEndElement();

	xmlWriter.writeTextElement("EventCode",QString("%1").arg(trialEventCode_));
	xmlWriter.writeTextElement("TrialNum",QString("%1").arg(trialNum_));

	xmlWriter.writeEndElement(); //Trial

	//Add the content to the command
	QByteArray contentArr = content.toUtf8();
	command->setFieldValue("Content-Length",QString("%1").arg(contentArr.length()));
	//Check that the Content-Length field matches the actual content
	Q_ASSERT(0 == command->setContent(contentArr));

	//Send out the command
	QSharedPointer<ProtocolResponse> response;
	response = Engine::PictoEngine::sendCommand(command, 1);
	Q_ASSERT(!response.isNull());
	Q_ASSERT(response->getResponseType() == "OK");
}

/*!	\brief Sends a EndTrial event to PictoServer
 *
 *	At the end of a Trial, we send a timestamped EndTrial event to PictoServer.
 *	This is done nearly simultaneously to sending a EndTrial event code to the
 *	neural recording device.  
 *
 *	The command used to do this is (the units of time are seconds)
 *		TRIAL /end PICTO/1.0
 *		Content-Length:???
 *		
 *		<Trial>
 *			<Time>8684354986.358943</Time>
 *			<EventCode>56</EventCode>
 *			<TrialNum>412</TrialNum> 	
 *		</Trial>
 */
void StateMachine::sendEndTrialToServer()
{
	//Create a TRIAL command
	QSharedPointer<ProtocolCommand> command(new ProtocolCommand("TRIAL /end PICTO/1.0"));

	//Create the content of the TRIAL command
	QString content;
	QXmlStreamWriter xmlWriter(&content);

	xmlWriter.writeStartElement("Trial");

	xmlWriter.writeStartElement("Time");
	Timestamper timestamper;
	xmlWriter.writeCharacters(QString("%1").arg(timestamper.stampSec(),0,'f',4));
	xmlWriter.writeEndElement();

	xmlWriter.writeTextElement("EventCode",QString("%1").arg(trialEventCode_));
	xmlWriter.writeTextElement("TrialNum",QString("%1").arg(trialNum_));

	xmlWriter.writeEndElement(); //Trial

	//Add the content to the command
	QByteArray contentArr = content.toUtf8();
	command->setFieldValue("Content-Length",QString("%1").arg(contentArr.length()));
	//Check that the Content-Length field matches the actual content
	Q_ASSERT(0 == command->setContent(contentArr));

	//Send out the command
	QSharedPointer<ProtocolResponse> response;
	response = Engine::PictoEngine::sendCommand(command, 1);
	Q_ASSERT(!response.isNull());
	Q_ASSERT(response->getResponseType() == "OK");
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
			int levelIndex = levelEnumStrs_.indexOf(xmlStreamReader->readElementText());

			if(levelIndex <0)
			{
				addError("StateMachine", "Unrecognized value inside Level tag", xmlStreamReader);
				return false;
			}

			propertyContainer_.setPropertyValue("Level",levelIndex);
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
					else if(elementType == "ScriptElement")
					{
						newElement = QSharedPointer<StateMachineElement>(new ScriptElement());
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
