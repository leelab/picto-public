#include "StateMachine.h"

#include "State.h"
#include "Reward.h"
#include "SwitchElement.h"
#include "PausePoint.h"
#include "LogicResult.h"
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

/*! \brief Constructs a new StateMachine.
 *	\details Adds AssetFactory objects for all of the StateMachineElement children that a StateMachine is allowed to have.
 */
StateMachine::StateMachine() : 
	MachineContainer("Transition","StateMachineElement"),
	ignoreInitialElement_(false),
	dontRunElement_(false)
{

	//AddDefinableProperty("InitialElement","");
	
	//Note that this is the same order as the enum to allow the enum to be
	//used as an index
	//levelEnumStrs_<<"Stage"<<"Trial"<<"Task"<<"Experiment";
	//AddDefinableProperty(PropertyContainer::enumTypeId(),"Level",0,"enumNames",levelEnumStrs_);
	AddDefinableObjectFactory("Level",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));

	//DefinePlaceholderTag("StateMachineElements");

	elementFactory_->addAssetType("Reward",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Reward::Create))));
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

	trialEventCode_ = 0;
}

/*! \brief Creates a new StateMachine object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> StateMachine::Create()
{
	return QSharedPointer<Asset>(new StateMachine());
}

/*! \brief This function appears to no longer be used (ie. The functions that call it are never called themselves).
 *	\details The function was once used when attaching to a remote Experiment
 *	that was already running in order to jump to the currently running part of the StateMachine.  Since then, we have created 
 *	the StateUpdater, SlaveExperimentDriver system that allows us to run "Slave" Experiments from 
 *	within the Qt event loop based on information coming from the Master, and not only from within 
 *	the blocking runTask() function.
 *	
 *	This function should probably be removed.
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
	if(deserializedVersion < "0.0.1")
	{	// In design syntax version "0.0.1", we removed "ScriptElements" and replaced them with "SwitchElements"
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
					else	//It must be stored in attributes (we did that at one point)
						switchElement->setPos(QPoint(obsAssetList.first()->getAttributeValue("x").toInt(),obsAssetList.first()->getAttributeValue("y").toInt()));
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

/*! \brief Runs this StateMachine execution logic within the framework of the input PictoEngine.
 *	\details This function more or less performs the following run procedure:
 *		- Verify that Scripting has been initialized
 *		- Run EntryScript
 *		- Transition to the initial StateMachineElement
 *		- One by one, call StateMachineElement::run() on the current StateMachineElement.  When it returns, get its Result, find the Transition that transitions
 *			from that result, transition to its destination, set that as the current StateMachineElement and loop again.
 *		- When control transitions to a Result, run the Result's EntryScript, then run this StateMachine's ExitScript and exit the function.
 *		- Return the name of the result that caused the StateMachine to exit.
 *	When an Analysis is active during the live experiment (in the TestViewer), AnalysisScripts are run before and after the Entry and Exit Scripts.
*/
QString StateMachine::runPrivate(QSharedPointer<Engine::PictoEngine> engine, bool)
{
	resetScriptableValues();
	path_.append(getName());
	QString pathStr = path_.join("::");
	//qDebug(QString("Entering: %1").arg(path_.join("::")).toLatin1());
	if(!initScripting(engine->operatorIsUser()))	//only debug when in the test viewer
	{
		//! TODO Make some sort of intelligent error reporting...
		return "scriptingError";
	}

	////Reset trialNum_ if we just entered a new Task
	//if(getLevel() == StateMachineLevel::Task)
	//{
	//	trialNum_ = 0;
	//}


	QString currElementName;
	QString nextElementName;

	QString result;
	if(!ignoreInitialElement_)
	{
		//currElementName = propertyContainer_->getPropertyValue("InitialElement").toString();
		currElement_.clear();
		QString initDestination = initTransition_->getDestination();
		if(elements_.contains(initDestination))
		{
			currElementName = initDestination;
			currElement_ = elements_.value(initDestination).staticCast<StateMachineElement>();
		}
		else
		{
			Q_ASSERT(results_.contains(initDestination));
			nextElementName = initDestination;
		}
	}
	else
	{
		currElementName = currElement_->getName();
		ignoreInitialElement_ = false;
	}

	//run the entry script
	runEntryScript();
	//Add the transition to the server
	//Note: Up until DesignSyntax 0.0.1, Picto version 1.0.12, the addStateTransitionForServer was called
	//before runEntryScript().  This is a problem because whenever we see a transition in the session
	//file we will follow it with AnalysisScript before continuing to the results of the EntryScript
	//of the transition destination.  Since this would actually occur in old version BEFORE the transition
	//parent's entry script was called, things will be out of order.  We will need to keep this in
	//mind when analyzing old experiments.
	engine->addStateTransitionForServer(initTransition_);
	while(true)
	{
		
		
		//If we're about to dive into another state machine we need to set it's path
		if(currElement_ && currElement_->inherits("Picto::StateMachine"))
		{
			currElement_.staticCast<StateMachine>()->setPath(path_);
		}

		//if((currElement_->inherits("Picto::Reward")) && (getLevel() == StateMachineLevel::Trial))
		//{
		//	QSharedPointer<CommandChannel> dataChan = engine->getDataCommandChannel();
		//	if(!dataChan.isNull())
		//		dataChan->processResponses(2000);
		//}
		
		if(currElement_)
		{
			result = currElement_->run(engine);

			if(result == "EngineAbort")
			{
				setLatestResult("EngineAbort");
				break;
			}
		

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
		}

		// If we transitioned to a result, then we're done and should return that result
		if(results_.contains(nextElementName))
		{
			result = nextElementName;

			setLatestResult(result);
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

/*! \brief Redirects directly to runPrivate().  See that function for more details.
 *	\details This indiretion is here for historical reasons and can probably be removed.  
 *	runPrivate() should probably be removed with its code copied into this function.
*/
QString StateMachine::run(QSharedPointer<Engine::PictoEngine> engine)
{
	return runPrivate(engine, false);
}

/*! \brief When a StateMachine is run as a slave, it really doesn't do anything since the SlaveExperimentDriver handles
 *	changing of Property values and transitioning control flow based on what happened in the Master experiment.  
 *	This function therefore does nothing except make sure that scripting is initialized.
 */
QString StateMachine::slaveRun(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result;
	if(!initScripting(false))
	{
		//! TODO Make some sort of intelligent error reporting...
		return "scriptingError";
	}

	return result;
}

void StateMachine::postDeserialize()
{
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
		addError(errMsg);
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
