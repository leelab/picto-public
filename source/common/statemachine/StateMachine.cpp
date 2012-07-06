#include "StateMachine.h"

#include "State.h"
#include "Reward.h"
//#include "FlowElement.h"
#include "ScriptElement.h"
#include "PausePoint.h"

#include "../engine/PictoEngine.h"
#include "../timing/Timestamper.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../network/CommandChannel.h"
#include "../timing/Timestamper.h"
#include "../storage/StateDataUnit.h"
#include "../storage/AlignmentDataUnit.h"
#include "../memleakdetect.h"

namespace Picto {

short StateMachine::trialEventCode_;
int StateMachine::trialNum_;

StateMachine::StateMachine() : 
	MachineContainer("Transition","StateMachineElement"),
	ignoreInitialElement_(false),
	dontRunElement_(false)
{
	
	AddDefinableProperty("Type","StateMachine");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/

	//AddDefinableProperty("InitialElement","");
	
	//Note that this is the same order as the enum to allow the enum to be
	//used as an index
	levelEnumStrs_<<"Stage"<<"Trial"<<"Task"<<"Experiment";
	AddDefinableProperty(QtVariantPropertyManager::enumTypeId(),"Level",0,"enumNames",levelEnumStrs_);

	//DefinePlaceholderTag("StateMachineElements");

	//QSharedPointer<AssetFactory> factory(new AssetFactory(0,-1));
	elementFactory_->addAssetType("Reward",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Reward::Create))));
	//elementFactory_->addAssetType("FlowElement",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(FlowElement::Create))));
	elementFactory_->addAssetType("ScriptElement",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ScriptElement::Create))));
	elementFactory_->addAssetType("State",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(State::Create))));
	elementFactory_->addAssetType("StateMachine",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(StateMachine::Create))));
	elementFactory_->addAssetType("PausePoint",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PausePoint::Create))));
	//AddDefinableObjectFactory("StateMachineElement",factory);

	//AddDefinableObjectFactory("Parameters",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ParameterContainer::Create))));

	//DefinePlaceholderTag("Transitions");

	//AddDefinableObjectFactory("Transition",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Transition::Create))));

	trialEventCode_ = 0;
}

void StateMachine::setLevel(StateMachineLevel::StateMachineLevel level)
{
	propertyContainer_->setPropertyValue("Level", QVariant(level));
}

StateMachineLevel::StateMachineLevel StateMachine::getLevel()
{
	return (StateMachineLevel::StateMachineLevel) propertyContainer_->getPropertyValue("Level").toInt();
}

QSharedPointer<Asset> StateMachine::Create()
{
	return QSharedPointer<Asset>(new StateMachine());
}


//bool StateMachine::setInitialElement(QString elementName)
//{
//	if(elements_.contains(elementName))
//	{
//		propertyContainer_->setPropertyValue("InitialElement", elementName);
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}

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
			QSharedPointer<StateMachineElement> childElement_ = elements_[nextMachine].staticCast<StateMachineElement>();
			if(!childElement_.dynamicCast<StateMachine>()->jumpToState(path,state))
				return false;
			currElement_ = childElement_;

		}
		else
		{
			Q_ASSERT_X(false,"StateMachine::jumpToState","Path entered into jumpToState was invalid");
			return false;
		}
	}
	else
	{
		if(elements_.contains(state))
		{
			currElement_ = elements_[state].staticCast<StateMachineElement>();
		}
		else if(results_.contains(state))
		{
			Q_ASSERT(false);
			dontRunElement_ = true;
		}
		else
		{
			Q_ASSERT_X(false,"StateMachine::jumpToState","Path entered into jumpToState was invalid");
			return false;
		}
	}
	ignoreInitialElement_ = true;
	return true;
}


/*	\brief The "run" function
 *
 *	Since runAsSlave(), and run() are so similar, they both just call into this private
 *	run function.  This saves us some serious code repetition.
 */
QString StateMachine::runPrivate(QSharedPointer<Engine::PictoEngine> engine, bool slave)
{
	if(!slave)
		resetScriptableValues();
	path_.append(getName());
	QString pathStr = path_.join("::");
	//qDebug(QString("Entering: %1").arg(path_.join("::")).toAscii());
	if(!initScripting(!slave && engine->operatorIsUser()))	//only debug when in the test viewer
	{
		//! \TODO Make some sort of intelligent error reporting...
		return "scriptingError";
	}

	//Figure out which scripts we will be running
	bool runEntryScript = !slave && !propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty();
	bool runExitScript = !slave && !propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty();

	QString entryScriptName = getName().simplified().remove(' ')+"Entry";
	QString exitScriptName = getName().simplified().remove(' ')+"Exit";

	//Reset trialNum_ if we just entered a new Task
	if(getLevel() == StateMachineLevel::Task)
	{
		trialNum_ = 0;
	}


	QString currElementName;
	QString nextElementName;

	if(!ignoreInitialElement_)
	{
		//currElementName = propertyContainer_->getPropertyValue("InitialElement").toString();
		currElementName = initTransition_->getDestination();
		currElement_ = elements_.value(currElementName).staticCast<StateMachineElement>();
		engine->addStateTransitionForServer(initTransition_);
	}
	else
	{
		currElementName = currElement_->getName();
		ignoreInitialElement_ = false;
	}

	QString result;
		//run the entry script
	if(runEntryScript)
		runScript(entryScriptName);
	while(true)
	{
		
		
		//If we're about to dive into another state machine we need to set it's path
		if(currElement_->type() == "StateMachine")
		{
			currElement_.dynamicCast<StateMachine>()->setPath(path_);
		}

		if((currElement_->type() == "Reward") && (getLevel() == StateMachineLevel::Trial) && !slave)
		{
			//if((getLevel() == StateMachineLevel::Trial) && !slave)
			//{
				QSharedPointer<CommandChannel> dataChan = engine->getDataCommandChannel();
				if(!dataChan.isNull())
					dataChan->processResponses(2000);
				//engine->generateEvent(trialEventCode_);
				//sendTrialEventToServer(engine);


				////Deal with all of the left-over commands
				//if(!cleanupRegisteredCommands(engine))
				//	handleLostServer(engine);
			//}
			//if(slave)
			//{
				//Since the result state takes a long time to run (due to issuing of rewards)
				//we're going to assume that we are in synch, and run it first.  Then we'll check
				//to make sure that the master engine is done (and that we didn't screw up)
				//QString masterResult;
				//QString slaveResult;
				//slaveResult = currElement_->runAsSlave(engine);
				//result = currElement_->runAsSlave(engine);

				//while(masterResult.isEmpty())
				//	masterResult = getMasterStateResult(engine);
				//Q_ASSERT(masterResult == slaveResult);
				//
				//result = masterResult;
				//break;
			//}
			//else
			//{
			//	result = currElement_->run(engine);
			//	//break;
			//}
		}
		//else
		//{
			if(slave)
			{
				if(dontRunElement_)
				{
					Q_ASSERT(false);
					while(result.isEmpty())
					{
						engine->updateCurrentStateFromServer();
						result = engine->getServerPathUpdate();
					}

					dontRunElement_ = false;
				}
				else
				{
					result = currElement_->runAsSlave(engine);
				}
			}
			else
				result = currElement_->run(engine);

			if(result == "EngineAbort")
				break;
		//}

		//Find the transition from our current source with a SourceResult string that matches the result
		//Yeah, this is kind of ugly...
		nextElementName = "";
		if(!slave)
		{
			bool foundTransition = false;
			foreach(QSharedPointer<Transition> tran, transitions_.values(currElementName))
			{
				if(tran->getSourceResult() == result)
				{
					nextElementName = tran->getDestination();
					engine->addStateTransitionForServer(tran);
					//sendStateDataToServer(tran, engine);
					foundTransition = true;
					break;
				}
			}
			Q_ASSERT(foundTransition);
		}
		else
		{	//We're a Slave
			//Get path to destination
			int lastSepIndex = result.lastIndexOf("::");
			QString destinationPath = result.left(lastSepIndex);
			QString destination = result.mid(lastSepIndex+2);
			qDebug(destination.toAscii());
			if(destinationPath == pathStr)
				nextElementName = destination;
			else if(destinationPath.startsWith(pathStr))
			{
				//The destination is in an element somewhere below this statemachine.
				//Call jumpToState to set this element and all elements in the path below 
				//to have the correct next element. (skipping the initialelement), 
				//then continue the loop.
				QStringList descendantPath = destinationPath.mid(pathStr.length()+2).split("::");
				nextElementName = descendantPath.first();
				bool rc = jumpToState(descendantPath,destination);
				//The jump to state function sets this elements "ignoreInitialElement"
				//value to true, which we don't want in this case because we aren't
				//leave this loop and nothing will set it back to false.  For that
				//reason we do it here.
				ignoreInitialElement_ = false;	
				Q_ASSERT_X(rc,"StateMachine::runPrivate",QString("Could not jump to \"%1\" from StateMachine at \"%2\"").arg(descendantPath.join("::")).arg(pathStr).toAscii());
				currElementName = nextElementName;
				continue;
			}
			else	
				//The path is not below this statemachine.  
				//Return the result to the level above and it will check if the path is below it.
				break;
			

		}
		// If we transitioned to a result, then we're done and should return that result
		if(results_.contains(nextElementName))
		{
			result = nextElementName;
			////If this asset's parent is a task then a result here means that we've already reached the last transition and the 
			////task is over.  Otherwise, we need to get the Master state result here because it will be the transition 
			////that tells us where we're going to go next in this StateMachine's parent.
			//if(slave && !getParentAsset()->inherits("Picto::Task"))
			//{
			//	Q_ASSERT(false);
			//	QString masterResult;
			//	//Make sure we're exiting with the next result (after the transition to our own result).
			//	while(masterResult.isEmpty())
			//		masterResult = getMasterStateResult(engine);
			//	//Q_ASSERT(masterResult == result);
			//	result = masterResult;
			//}
			break;
		}
		//! \TODO come up with a more elegant error handling scheme...
		//QString errorMsg = "Unable to find element with name: "+nextElementName
		Q_ASSERT_X(elements_.contains(nextElementName), "StateMachine::Run",QString("Unable to find element with name: %1").arg(nextElementName).toAscii());

		currElement_ = elements_[nextElementName].staticCast<StateMachineElement>();
		currElementName = nextElementName;

	}
	//run the exit script
	if(runExitScript)
		runScript(exitScriptName);
	
	//Since we're backing out of this state machine we need to remove it from the path
	//qDebug(QString("Exiting: %1").arg(path_.join("::")).toAscii());
	path_.takeLast();
	return result;
}
QString StateMachine::run(QSharedPointer<Engine::PictoEngine> engine)
{
	return runPrivate(engine, false);
}

QString StateMachine::runAsSlave(QSharedPointer<Engine::PictoEngine> engine)
{
	return runPrivate(engine, true);
}

///*!	\brief Sets up the script engines for this state machine
// *
// *	To make script execution more efficient, we attempt to preload everything
// *	Each state machine maintains its own script engine, which is used by all of
// *	the contained ScriptElements.  The initialization process consists of the 
// *	following:
// *		1. Bind all of the parameters to the scripting engine
// *		2. Call initScripting on all contained elements.  This has the following effects:
// *			a. Creates functions in our local engine for all ScriptElements
// *			b. Sets up a local engine within all States
// *			c. Calls this function on all contained StateMachines
// */
//bool StateMachine::initScripting(QScriptEngine &qsEngine)
//{
//	//Since the StateMachine maintains its own engine, we can ignore the passed in engine
//	Q_UNUSED(qsEngine);
//
//	//bind the Scriptables
//	bindToScriptEngine(qsEngine_);
//
//	//initialize scripting on all of the contained elements
//	foreach(QSharedPointer<ResultContainer> element, elements_)
//	{
//		qDebug("Scripting Intialized for: " + element->getName().toAscii());
//		if(!element.staticCast<StateMachineElement>()->initScripting(qsEngine_))
//			return false;
//	}
//
//	scriptingInit_ = true;
//	return true;
//}

/*!	\brief Sends a Trial event to PictoServer
 *
 *	At the begining and end of a Trial, we send a timestamped StartTrial event to 
 *	PictoServer. This is done nearly simultaneously to sending a StartTrial event 
 *	code to the neural recording device.  
 *
 *	The command used to do this is (the units of time are seconds)
 *		PUTDATA engineName:running PICTO/1.0
 *		Content-Length:???
 *		
 *		<Data>
 *			<AlignmentDataUnit>
 *				<Time>8684354986.358943</Time>
 *				<EventCode>56</EventCode>
 *				<TrialNum>412</TrialNum> 	
 *			</AlignmentDataUnit>
 *		</Data>
 */
void StateMachine::sendTrialEventToServer(QSharedPointer<Engine::PictoEngine> engine)
{
	Timestamper timestamper;
	double timestamp = timestamper.stampSec();

	QSharedPointer<CommandChannel> dataChannel = engine->getDataCommandChannel();
	if(dataChannel.isNull())
		return;

	//Create an alignment command
	QString dataCommandStr = "PUTDATA " + engine->getName() + ":running PICTO/1.0";
	QSharedPointer<ProtocolCommand> command(new ProtocolCommand(dataCommandStr));

	//Create the content of the alignment command
	QByteArray alignDataXml;
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&alignDataXml));
	
	Picto::AlignmentDataUnit alignData;
	alignData.setAlignCode(trialEventCode_);
	alignData.setAlignNumber(trialNum_);
	alignData.setTimestamp(timestamp);

	xmlWriter->writeStartElement("Data");
	alignData.toXml(xmlWriter);
	xmlWriter->writeEndElement();


	command->setContent(alignDataXml);
	command->setFieldValue("Content-Length",QString::number(alignDataXml.length()));

	dataChannel->sendRegisteredCommand(command);
	//if(!dataChannel->waitForResponse(10000))
	//{
	//	handleLostServer(engine);
	//}
	//else
	//{

	//	response = dataChannel->getResponse();
	//	Q_ASSERT(!response.isNull());
	//	Q_ASSERT(response->getResponseType() == "OK");
	//	processStatusDirective(engine,response);
	//}
}

///*!	\brief Sends a StateDataUnit to the server to let it know that we are transitioning
// *
// *	To keep master and slave engines in synch, we send StateDataUnits to the server
// *	everytime there is a change in state.
// *
// *	This command is sent as a registered command, which means that we don't need to 
// *	worry about checking for a response, as that will be taken care of in either the
// *	State rendering loop, or the cleanup at the end of a trial.
// */
//void StateMachine::sendStateDataToServer(QSharedPointer<Transition> transition, QSharedPointer<Engine::PictoEngine> engine)
//{
//	QSharedPointer<CommandChannel> dataChannel = engine->getDataCommandChannel();
//
//	if(dataChannel.isNull())
//		return;
//	
//
//	//send a PUTDATA command to the server with the state transition data
//	QString status = "running";
//	int engCmd = engine->getEngineCommand();
//	switch(engCmd)
//	{
//	case Engine::PictoEngine::PlayEngine:
//		status = "running";
//		break;
//	case Engine::PictoEngine::PauseEngine:
//		status = "paused";
//		break;
//	case Engine::PictoEngine::StopEngine:
//		status = "stopped";
//		break;
//	}
//	QString dataCommandStr = "PUTDATA " + engine->getName() + ":" + status + " PICTO/1.0";
//	QSharedPointer<Picto::ProtocolCommand> dataCommand(new Picto::ProtocolCommand(dataCommandStr));
//
//	QByteArray stateDataXml;
//	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&stateDataXml));
//
//	Timestamper stamper;
//	double timestamp = stamper.stampSec();
//	QString qualifiedName = path_.join("::");
//
//	StateDataUnit stateData;
//	stateData.setTransition(transition,timestamp,qualifiedName);
//
//	xmlWriter->writeStartElement("Data");
//	stateData.toXml(xmlWriter);
//	xmlWriter->writeEndElement();
//
//
//	dataCommand->setContent(stateDataXml);
//	dataCommand->setFieldValue("Content-Length",QString::number(stateDataXml.length()));
//
//	dataChannel->sendRegisteredCommand(dataCommand);
//	dataChannel->processResponses(0);
//}

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

void StateMachine::postDeserialize()
{
	////Remove me when all experiments are updated to have init transitions!!!!!!!!!!!!!!!!!!!!!!
	//QSharedPointer<Transition> newTrans(new Transition("","",propertyContainer_->getPropertyValue("InitialElement").toString()));
	//if(addTransition(newTrans))
	//{
	//	newTrans->setExperimentConfig(getExperimentConfig());
	//	propertyContainer_->getProperty("InitialElement")->setDeleted();
	//}
	/////////////////////////////////////////////////////////////////////////////////////////////7

	MachineContainer::postDeserialize();
}

bool StateMachine::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!MachineContainer::validateObject(xmlStreamReader))
		return false;

	//	Validate State Machine
	//	The validation process consists of:
	//		- check that the contained state machines are of a legal level
	//		- check each transition to confirm that the source, 
	//		  sourceResult, and destination all exist.
	//		- check that every element has all its results connected to
	//		  a transition
	//		- check that the initial element is a real element
	//		- validate all contained state machines		
	QString myName = getName();

	//Confirm that any contained StateMachines are of the correct level
	QList<QSharedPointer<Asset>> newStateMachs = getGeneratedChildren("StateMachineElement");
	foreach(QSharedPointer<Asset> element, newStateMachs)
	{
		if(element.dynamicCast<StateMachine>().isNull())
			continue;
		StateMachineLevel::StateMachineLevel containedLevel;
		containedLevel = element.staticCast<StateMachine>()->getLevel();

		StateMachineLevel::StateMachineLevel thisLevel;
		thisLevel = getLevel();

		QString errMsg = QString("StateMachine: %1 contains a statemachine of incorrect level.  "
			"E.g. A statemachine of level \"Trial\" may not contain a statemachine of level \"Task\"")
			.arg(myName);
		if(thisLevel == StateMachineLevel::Stage)
		{
			if(containedLevel > thisLevel)
			{
				addError("StateMachine", errMsg,xmlStreamReader);
				return false;
			}
		}
		else
		{
			if(containedLevel >= thisLevel)
			{
				addError("StateMachine", errMsg,xmlStreamReader);
				return false;
			}
		}
	}

	////Confirm that the initial element is a real element
	//QString initialElement = propertyContainer_->getPropertyValue("InitialElement").toString();
	//if(!elements_.contains(initialElement))
	//{
	//	QString errMsg = QString("InitialElement: %1 is not an element of state machine: %2")
	//		.arg(initialElement).arg(myName);
	//	addError("StateMachine", errMsg,xmlStreamReader);
	//	return false;
	//}

	//Confirm that there is an initial state transition
	if(initTransition_.isNull())
	{
		QString errMsg = QString(
								"MachineContainer: %1 does not have an initial state transition").arg(getName());
		addError("MachineContainer", errMsg, xmlStreamReader);
		return false;
	}

	//If we made it this far, all the transitions are "legal"
	return true;
}

bool StateMachine::hasScripts()
{
	if(MachineContainer::hasScripts())
		return true;
	return false;
}


}; //namespace Picto
