#include "StateMachine.h"

#include "State.h"
#include "Reward.h"
//#include "FlowElement.h"
#include "ScriptElement.h"
#include "SwitchElement.h"
#include "PausePoint.h"
#include "RequiredResult.h"
#include "../storage/ObsoleteAsset.h"

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
	AddDefinableProperty(PropertyContainer::enumTypeId(),"Level",0,"enumNames",levelEnumStrs_);

	//DefinePlaceholderTag("StateMachineElements");

	//QSharedPointer<AssetFactory> factory(new AssetFactory(0,-1));
	elementFactory_->addAssetType("Reward",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Reward::Create))));
	//elementFactory_->addAssetType("FlowElement",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(FlowElement::Create))));
	elementFactory_->addAssetType("ScriptElement",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
	elementFactory_->addAssetType("Switch",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SwitchElement::Create))));
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
 *	Then, when run() or slaveRun() is called, the machine will start in that
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

void StateMachine::upgradeVersion(QString deserializedVersion)
{
	MachineContainer::upgradeVersion(deserializedVersion);
	if(deserializedVersion < "0.0.3")
	{	// In design syntax version "0.0.3", we removed "ScriptElements" and replaced them with "SwitchElements"
		// "ScriptElement" were essentially "SwitchElements" that contained only two results, "true" and "false"
		// that were triggered with a boolean value.
		// To upgrade, we serialize "ScriptElements" into ObsoleteAssets and create new equivalent "SwitchElements".
		QList<QSharedPointer<Asset>> stateMachineElements = getGeneratedChildren("StateMachineElement");
		QList<QSharedPointer<ObsoleteAsset>> scriptElements;
		foreach(QSharedPointer<Asset> element, stateMachineElements)
		{
			if(element->inherits("Picto::ObsoleteAsset") 
				&& element.staticCast<ObsoleteAsset>()->getAttributeValue("type") == "ScriptElement")
			{
				scriptElements.append(element.staticCast<ObsoleteAsset>());
			}
		}

		//For each script element, create a switch element and script data into it.
		foreach(QSharedPointer<ObsoleteAsset> scriptElement,scriptElements)
		{
			QString error;
			QSharedPointer<Asset> switchElementAsset = createChildAsset("StateMachineElement","Switch",error);
			QSharedPointer<SwitchElement> switchElement = switchElementAsset.staticCast<SwitchElement>();
			QList<QSharedPointer<ObsoleteAsset>> obsAssetList;
			//Copy name to Switch Element
			obsAssetList = scriptElement->getObsoleteChildAsset("Name");
			switchElement->setName(obsAssetList.size()?obsAssetList.first()->getValue():"");
			//Copy Script to Switch Element
			obsAssetList = scriptElement->getObsoleteChildAsset("Script");
			switchElement->getPropertyContainer()->setPropertyValue("Script",obsAssetList.size()?obsAssetList.first()->getValue():"");
			//Get UI position info and set it to the switch element
			obsAssetList = scriptElement->getObsoleteChildAsset("UIInfo");	//Get UIINfo sub asset
			if(!obsAssetList.isEmpty())
			{
				obsAssetList = obsAssetList.first()->getObsoleteChildAsset("Pos");	//Get Pos sub tag
				if(!obsAssetList.isEmpty())
				{
					QStringList xy = obsAssetList.first()->getValue().split(",");	//Convery ?,? of tag value to string list
					if(xy.size() == 2)	//If string list has correct size, set it to the switch element position
						switchElement->setPos(QPoint(xy.first().toFloat(),xy.last().toFloat()));
				}
			}

			//Create true, false results in Switch Element
			QSharedPointer<Asset> result = switchElement->createChildAsset("Result","",error);
			result.staticCast<UIEnabled>()->setName("true");
			result = switchElement->createChildAsset("Result","",error);
			result.staticCast<UIEnabled>()->setName("false");\
		}
	}
}

/*	\brief The "run" function
 *
 *	Since slaveRun(), and run() are so similar, they both just call into this private
 *	run function.  This saves us some serious code repetition.
 */
QString StateMachine::runPrivate(QSharedPointer<Engine::PictoEngine> engine, bool slave)
{
	resetScriptableValues();
	path_.append(getName());
	QString pathStr = path_.join("::");
	//qDebug(QString("Entering: %1").arg(path_.join("::")).toLatin1());
	if(!initScripting(engine->operatorIsUser()))	//only debug when in the test viewer
	{
		//! \TODO Make some sort of intelligent error reporting...
		return "scriptingError";
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

	Q_ASSERT(currElement_);
	QString result;
	//run the entry script
	runEntryScript();
	while(true)
	{
		
		
		//If we're about to dive into another state machine we need to set it's path
		if(currElement_->type() == "StateMachine")
		{
			currElement_.dynamicCast<StateMachine>()->setPath(path_);
		}

		if((currElement_->type() == "Reward") && (getLevel() == StateMachineLevel::Trial))
		{
			QSharedPointer<CommandChannel> dataChan = engine->getDataCommandChannel();
			if(!dataChan.isNull())
				dataChan->processResponses(2000);
		}
		
		result = currElement_->run(engine);

		if(result == "EngineAbort")
			break;
		

		//Find the transition from our current source with a SourceResult string that matches the result
		//Yeah, this is kind of ugly...
		nextElementName = "";
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
		// If we transitioned to a result, then we're done and should return that result
		if(results_.contains(nextElementName))
		{
			result = nextElementName;

			//Run result script if there is one.
			results_.value(result)->runResultScript();	//Added in Picto Version 1.0.12.

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
		Q_ASSERT_X(elements_.contains(nextElementName), "StateMachine::Run",QString("Unable to find element with name: %1").arg(nextElementName).toLatin1());

		currElement_ = elements_[nextElementName].staticCast<StateMachineElement>();
		currElementName = nextElementName;

	}
	//run the exit script
	runExitScript();
	
	//Since we're backing out of this state machine we need to remove it from the path
	//qDebug(QString("Exiting: %1").arg(path_.join("::")).toLatin1());
	path_.takeLast();
	return result;
}
QString StateMachine::run(QSharedPointer<Engine::PictoEngine> engine)
{
	return runPrivate(engine, false);
}

QString StateMachine::slaveRun(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result;
	if(!initScripting(false))
	{
		//! \TODO Make some sort of intelligent error reporting...
		return "scriptingError";
	}

	return result;
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
//		qDebug("Scripting Intialized for: " + element->getName().toLatin1());
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
	
	QString myName = getName();

	//Confirm that there is an initial state transition
	if(initTransition_.isNull())
	{
		QString errMsg = QString(
								"MachineContainer: %1 does not have an initial state transition").arg(getName());
		addError("MachineContainer", errMsg, xmlStreamReader);
		return false;
	}
	return true;
}

bool StateMachine::hasScripts()
{
	if(MachineContainer::hasScripts())
		return true;
	return false;
}


}; //namespace Picto
