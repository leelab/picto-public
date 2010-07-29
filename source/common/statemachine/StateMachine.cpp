#include "StateMachine.h"

#include "State.h"
#include "Result.h"
#include "FlowElement.h"
#include "ScriptElement.h"

#include "../engine/PictoEngine.h"
#include "../timing/Timestamper.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../network/CommandChannel.h"
#include "../timing/Timestamper.h"
#include "../storage/StateDataStore.h"

namespace Picto {

int StateMachine::trialEventCode_;
int StateMachine::trialNum_;

StateMachine::StateMachine() : 
	scriptingInit_(false),
	ignoreInitialElement_(false)

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

/*!	\brief Sets the machine in a specific state
 *	
 *	Calling this function will place the state machine into the passed in state.
 *	Then, when run() or runAsSlave() is called, the machine will start in that
 *	state, rather than in the initial state.  This is used when we are joining 
 *  state machines that are already running.
 */
bool StateMachine::jumpToState(QStringList path, QString state)
{
	if(!path.isEmpty())
	{
		QString nextMachine = path.takeFirst();
		if(elements_.contains(nextMachine))
		{
			currElement_ = elements_[nextMachine];
			if(!currElement_.dynamicCast<StateMachine>()->jumpToState(path,state))
				return false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if(elements_.contains(state))
			currElement_ = elements_[state];
		else
			return false;
	}
	ignoreInitialElement_ = true;
	return true;
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
	QString name = propertyContainer_.getPropertyValue("Name").toString();

	//Confirm that any contained StateMachines are of the correct level
	foreach(QSharedPointer<StateMachineElement> element, elements_)
	{
		if(element->type() == "StateMachine")
		{
			StateMachineLevel::StateMachineLevel containedLevel;
			containedLevel = element.staticCast<StateMachine>()->getLevel();

			StateMachineLevel::StateMachineLevel thisLevel;
			thisLevel = getLevel();

			QString errMsg = QString("StateMachine: %1 contains a statemachine of incorrect level.  "
				"E.g. A statemachine of level \"Trial\" may not contain a statemachine of level \"Task\"")
				.arg(name);
			if(thisLevel == StateMachineLevel::Stage)
			{
				if(containedLevel > thisLevel)
				{
					addError("ValidateStateMachine", errMsg);
					return false;
				}
			}
			else
			{
				if(containedLevel >= thisLevel)
				{
					addError("ValidateStateMachine", errMsg);
					return false;
				}
			}
		}
		
	}

	//Confirm that all transitions are legal
	foreach(QSharedPointer<Transition> tran, transitions_)
	{
		QString source = tran->getSource();
		if(!elements_.contains(source))
		{
			QString errMsg = QString("State Machine: %1 has an illegal transition.  "
				"Source: %2 is not a element.").arg(name).arg(source);
			addError("ValidateStateMachine", errMsg);
			return false;
		}
		
		QString dest = tran->getDestination();
		if(!elements_.contains(dest))
		{
			QString errMsg = QString("State Machine: %1 has an illegal transition.  "
				"Destination: %2 is not a element.").arg(name).arg(dest);
			addError("ValidateStateMachine", errMsg);
			return false;
		}

		QString sourceResult = tran->getSourceResult();
		QSharedPointer<StateMachineElement> sourceElement;
		sourceElement = elements_.value(source);
		if(!sourceElement->getResultList().contains(sourceResult))
		{
			QString errMsg = QString("State Machine: %1 has an illegal transition.  "
				"Result: %2 is not a result generated by source: %3").arg(name)
				.arg(sourceResult).arg(sourceElement->getName());
			addError("ValidateStateMachine", errMsg);
			return false;
		}
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
				QString errMsg = QString("State Machine: %1 Element %2 has an "
					"unconnected result of %3").arg(name).arg(elementName).arg(unconnectedResult);
				addError("ValidateStateMachine", errMsg);
				return false;
			}
		}			
	}

	//Confirm that the initial element is a real element
	QString initialElement = propertyContainer_.getPropertyValue("Initial Element").toString();
	if(!elements_.contains(initialElement))
	{
		QString errMsg = QString("InitialElement: %1 is not an element of state achine: %2")
			.arg(initialElement).arg(name);
		addError("ValidateStateMachine", errMsg);
		return false;
	}

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


/*	\brief The "run" function
 *
 *	Since runAsSlave(), and run() are so similar, they both just call into this private
 *	run function.  This saves us some serious code repetition.
 */
QString StateMachine::runPrivate(QSharedPointer<Engine::PictoEngine> engine, bool slave)
{
	path_.append(getName());

	if(!scriptingInit_)
	{
		if(!initScripting(qsEngine_))
		{
			//! \TODO Make some sort of intelligent error reporting...
			return "scriptingError";
		}
	}

	//Generate the start trial event
	if(getLevel() == StateMachineLevel::Trial && !slave)
	{
		//Generating an event on the neural recorder takes ~250 us, while generating a 
		//Picto event requires sending a command to the server and waiting for a response.
		//Because of this, we generate the nerual recorder event first.
		trialEventCode_++;
		trialNum_++;
		sendTrialEventToServer(engine);
		engine->generateEvent(trialEventCode_);
	}
	//Reset trialNum_ if we just entered a new Task
	if(getLevel() == StateMachineLevel::Task)
	{
		trialNum_ = 0;
	}


	QString currElementName;
	QString nextElementName;

	if(!ignoreInitialElement_)
	{
		currElementName = propertyContainer_.getPropertyValue("Initial Element").toString();
		currElement_ = elements_.value(currElementName);
	}
	else
	{
		currElementName = currElement_->getName();
		ignoreInitialElement_ = false;
	}

	while(true)
	{
		QString result;
		
		//If we're about to dive into another state machine we need to set it's path
		if(currElement_->type() == "StateMachine")
		{
			currElement_.dynamicCast<StateMachine>()->setPath(path_);
		}

		if(currElement_->type() == "Result")
		{
			if(getLevel() == StateMachineLevel::Trial)
			{
				engine->generateEvent(trialEventCode_);
				sendTrialEventToServer(engine);


				//Deal with all of the left-over commands
				if(!cleanupRegisteredCommands(engine))
					handleLostServer(engine);
			}
			if(slave)
			{
				//Since the result state takes a long time to run (due to issuing of rewards)
				//we're going to assume that we are in synch, and run it first.  Then we'll check
				//to make sure that the master engine is done (and that we didn't screw up)
				QString masterResult;
				QString slaveResult;
				slaveResult = currElement_->runAsSlave(engine);

				while(masterResult.isEmpty())
					masterResult = getMasterStateResult(engine);
				Q_ASSERT(masterResult == slaveResult);
				return masterResult;
			}
			else
				return currElement_->run(engine);
		}
		else
		{
			if(slave)
				result = currElement_->runAsSlave(engine);
			else
				result = currElement_->run(engine);

			if(result == "EngineAbort")
				return result;
		}

		//Find the transition from our current source with a SourceResult string that matches the result
		//Yeah, this is kind of ugly...
		nextElementName = "";
		foreach(QSharedPointer<Transition> tran, transitions_.values(currElementName))
		{
			if(tran->getSourceResult() == result)
			{
				nextElementName = tran->getDestination();
				sendStateDataToServer(tran, engine);
				break;
			}
		}
		//! \TODO come up with a more elegant error handling scheme...
		//QString errorMsg = "Unable to find element with name: "+nextElementName
		Q_ASSERT_X(elements_.contains(nextElementName), "StateMachine::Run",QString("Unable to find element with name: %1").arg(nextElementName).toAscii());

		currElement_ = elements_[nextElementName];
		currElementName = nextElementName;

	}

}
QString StateMachine::run(QSharedPointer<Engine::PictoEngine> engine)
{
	return runPrivate(engine, false);
}

QString StateMachine::runAsSlave(QSharedPointer<Engine::PictoEngine> engine)
{
	return runPrivate(engine, true);
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

/*!	\brief Sends a Trial event to PictoServer
 *
 *	At the begining and end of a Trial, we send a timestamped StartTrial event to 
 *	PictoServer. This is done nearly simultaneously to sending a StartTrial event 
 *	code to the neural recording device.  
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
void StateMachine::sendTrialEventToServer(QSharedPointer<Engine::PictoEngine> engine)
{
	QSharedPointer<CommandChannel> serverChannel = engine->getUpdateCommandChannel();
	if(serverChannel.isNull())
		return;

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

	xmlWriter.writeTextElement("EventCode",QString::number(trialEventCode_));
	xmlWriter.writeTextElement("TrialNum",QString::number(trialNum_));

	xmlWriter.writeEndElement(); //Trial

	//Add the content to the command
	QByteArray contentArr = content.toUtf8();
	command->setFieldValue("Content-Length",QString("%1").arg(contentArr.length()));
	//Check that the Content-Length field matches the actual content
	Q_ASSERT(0 == command->setContent(contentArr));

	//Send out the command
	QSharedPointer<ProtocolResponse> response;


	serverChannel->sendCommand(command);
	if(!serverChannel->waitForResponse(10000))
	{
		handleLostServer(engine);
	}
	else
	{
		response = serverChannel->getResponse();

		Q_ASSERT(!response.isNull());
		Q_ASSERT(response->getResponseType() == "OK");
	}
}

/*!	\brief Sends a StateDataStore to the server to let it know that we are transitioning
 *
 *	To keep master and slave engines in synch, we send StateDataStores to the server
 *	everytime there is a change in state.
 *
 *	This command is sent as a registered command, which means that we don't need to 
 *	worry about checking for a response, as that will be taken care of in either the
 *	State rendering loop, or the cleanup at the end of a trial.
 */
void StateMachine::sendStateDataToServer(QSharedPointer<Transition> transition, QSharedPointer<Engine::PictoEngine> engine)
{
	QSharedPointer<CommandChannel> dataChannel = engine->getDataCommandChannel();

	if(dataChannel.isNull())
		return;
	

	//send a PUTDATA command to the server with the state transition data
	QString dataCommandStr = "PUTDATA "+engine->getName()+" PICTO/1.0";
	QSharedPointer<Picto::ProtocolCommand> dataCommand(new Picto::ProtocolCommand(dataCommandStr));

	QByteArray stateDataXml;
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&stateDataXml));

	Timestamper stamper;
	double timestamp = stamper.stampSec();
	QString qualifiedName = path_.join("::");

	StateDataStore stateData;
	stateData.setTransition(transition,timestamp,qualifiedName);

	xmlWriter->writeStartElement("Data");
	stateData.serializeAsXml(xmlWriter);
	xmlWriter->writeEndElement();


	dataCommand->setContent(stateDataXml);
	dataCommand->setFieldValue("Content-Length",QString::number(stateDataXml.length()));
	QUuid commandUuid = QUuid::createUuid();
	dataCommand->setFieldValue("Command-ID",commandUuid.toString());

	dataChannel->sendRegisteredCommand(dataCommand);
}

/*	\brief Called when we seem to have lost contact with the server
 *
 *	In the event that the server connection goes down, the commandChannels will
 *	automatically attempt to reconnect.  By the time we've gotten here, we can
 *	assume that the connection is completely lost (either the server died, or
 *	there's a network interruption.
 *
 *	We "handle" this by sending a stop command to the engine.
 *
 *	At some point, we may decide to use a more elegant error handling mechanism...
 */
void StateMachine::handleLostServer(QSharedPointer<Engine::PictoEngine> engine)
{
	engine->stop();
}


/*! \brief cleans up all of the sent commands.
 *	
 *	When registered commands are sent, we don't wait around for a response.  Therefore,
 *	at some point we need to make sure that all of the responses have arrived.  This 
 *	point is at the end of a trial (since that's the only time we have to spare).
 *	The function does the following:
 *		1. Waits for 2 seconds checking to see if all the responses are in.
 *		2. Resends any commands that never saw responses
 *		3. Waits for 2 seconds picking up all responses
 *		4. Returns true if all commands received responses (eventually) or
 *		   false if responses were dropped
 *
 *	NOTE: As of 5/26/2010 this function is incredibly important.  We are currently
 *	using a disk-backed database to store the session data, and the database is too 
 *	slow to keep up with the frame rate.  As a result, commands are piling up, so
 *	this clean-up function becomes essential.  Obviously the database issues
 *	will eventually get fixed.
 */
bool StateMachine::cleanupRegisteredCommands(QSharedPointer<Engine::PictoEngine> engine)
{
	QSharedPointer<CommandChannel> serverChan = engine->getDataCommandChannel();

	if(!serverChan)
		return true;

	if(serverChan->pendingResponses() == 0)
		return true;

	//int elapsedTimeMs = 0;
	QSharedPointer<ProtocolResponse> resp;

	//See if the missing responses are just slow getting to us...
	//This loop will keep reading responses as long as they are arriving at a 
	//rate of greater than 0.5Hz
	while(serverChan->waitForResponse(2000))
	{
		resp = serverChan->getResponse();
		Q_ASSERT(!resp.isNull());
		Q_ASSERT(resp->getResponseType() == "OK");

		processStatusDirective(engine, resp);

		if(serverChan->pendingResponses() == 0 && 
			serverChan->incomingResponsesWaiting() == 0)
		{
			return true;
		}
	}


	//resend all of the pending responses
	serverChan->resendPendingCommands();

	//Wait to see if the missing responses arrive
	//This loop will keep reading responses as long as they are arriving at a 
	//rate of greater than 0.5Hz
	while(serverChan->waitForResponse(2000))
	{
		resp = serverChan->getResponse();
		Q_ASSERT(!resp.isNull());
		Q_ASSERT(resp->getResponseType() == "OK");

		processStatusDirective(engine, resp);

		if(serverChan->pendingResponses() == 0 && 
			serverChan->incomingResponsesWaiting() == 0)
		{
			return true;
		}
	}


	//If we've made it this far, then we failed at cleaning up.
	return false;

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
