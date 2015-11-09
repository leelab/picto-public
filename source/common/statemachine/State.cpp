#include <QCoreApplication>
#include <QUuid>

#include "State.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../engine/PictoEngine.h"
#include "../engine/SignalChannel.h"
#include "../storage/FrameDataUnitPackage.h"
#include "../stimuli/OutputElement.h"
#include "../stimuli/CursorGraphic.h"
#include "../stimuli/InputSignal.h"
#include "../timing/Timestamper.h"
#include "../parameter/AnalysisScriptHolder.h"
#include "../controlelements/TestController.h"
#include "../controlelements/StopwatchController.h"
#include "../controlelements/ScriptController.h"
#include "../controlelements/TargetController.h"
#include "../controlelements/ChoiceController.h"
#include "../controlelements/FixationController.h"
#include "../operator/DataViewElement.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new State object.
 *	\details Initializes the State's Scene object, adds a 'FrameScript" Property and AssetFactories for the various
 *	ControlElements that can be added to the State.
 */
State::State() :
	MachineContainer("Transition","ControlElement"),
	scene_(Scene::createScene()),
	hasCursor_(false)
{
	//Define script properties
	AddDefinableProperty("FrameScript", "");
	AddDefinableProperty(QVariant::Color,"BackgroundColor","");
	
	//Define generatable control elements.
	//elementFactory_->addAssetType(TestController::ControllerType(),
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TestController::Create))));
	elementFactory_->addAssetType(StopwatchController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,1,AssetFactory::NewAssetFnPtr(StopwatchController::Create))));
	elementFactory_->addAssetType(ScriptController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ScriptController::Create))));
	elementFactory_->addAssetType(TargetController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TargetController::Create))));
	elementFactory_->addAssetType(ChoiceController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ChoiceController::Create))));
	elementFactory_->addAssetType(FixationController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(FixationController::Create))));
}

/*! \brief Creates a new State object and returns a shared Asset pointer to it.
 */
QSharedPointer<Asset> State::Create()
{
	return QSharedPointer<Asset>(new State());
}

/*! \brief rebuilds the scene whenever run mode is enable in case something about the scene changed.
 */
void State::enableRunMode(bool enable)
{
	MachineContainer::enableRunMode(enable);
	if(!enable)
		return;
	rebuildScene();
}

/*! \brief Runs this State's execution logic within the framework of the input PictoEngine.
 *	\details This function more or less performs the following run procedure:
 *		- Run EntryScript
 *		- Go through all ControlElements and check to see if any are done.  If they are, run the ControlResult script,
 *			transition to their attached result, run its script, run the State's ExitScript and return.
 *		- If ControlElements aren't done, run the FrameScript, render the Scene and loop back to checking ControlElements.
 *	When an Analysis is active during the live experiment (in the TestViewer), AnalysisScripts are run before and after the
 *	Entry and Exit Scripts.  The AnalysisFrameScript is run after the Scene is rendered.
 *	
 *	The name of the result that ended the State execution is returned from the function as a string.
 */
QString State::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();

	//Set up frame script.
	bool runFrameScript = !propertyContainer_->getPropertyValue("FrameScript").toString().isEmpty();
	QString frameScriptName = getName().simplified().remove(' ')+"Frame";

	//run the entry script
	runEntryScript();

	//Start up all of the control elements
	foreach(QSharedPointer<ResultContainer> control, elements_)
	{
		control.staticCast<ControlElement>()->start(engine);
	}

	QString result = "";
	QSharedPointer<Transition> resultTrans;
	bool isDone = false;

	//This is the "rendering loop"  It gets run for every frame
	while(!isDone)
	{
		//--------- Check control elements------------
		foreach(QSharedPointer<ResultContainer> control, elements_)
		{
			if(control.staticCast<ControlElement>()->isDone(engine))
			{
				//Get the result and call its entry script
				QSharedPointer<Result> controlResult = control.staticCast<ControlElement>()->getResult();
				control->setLatestResult(controlResult->getName());
				controlResult->runResultScript();
				QList<QSharedPointer<Transition>> elemLinks = transitions_.values(control->getName());
				foreach(QSharedPointer<Transition> link,elemLinks)
				{
					if(link->getSourceResultAsset() == controlResult)
					{
						result = link->getDestination();
						resultTrans = link;
						break;
					}
				}
				Q_ASSERT(resultTrans);
				isDone = true;
			}
		}
		if(elements_.isEmpty())
			isDone = true;

		if(!isDone)
		{
			//-------------- If the state is not over, run the frame script ----------------
			//
			if(runFrameScript)
				runScript(frameScriptName);

			//----------  Draw the scene.  This will automatically send behavioral data to server --------------
			scene_->render(engine,getAssetId());


			//----------  Run the analysis frame scripts ---------------------------------------
			runAnalysisFrameScripts();
		}

		//------ Check for engine stop commands ---------------
		if(checkForEngineStop(engine))
		{
			isDone = true;
			setLatestResult("EngineAbort");
			result = "EngineAbort";
		} 
		else if(isDone)
		{
			//Added in Picto Version 1.0.12.  Before this transitions within a state weren't being recorded in the
			//  session file.
			engine->addStateTransitionForServer(resultTrans);
			//Added in Picto Version 1.0.12.
			Q_ASSERT(results_.contains(result));		
			setLatestResult(result);
			//Run result script if there is one.
			results_.value(result)->runResultScript();
		}
		//If we're not in exclusive mode, we should allocate time to process events.  This would occur if we were running
		//  a state machine somewhere other than Director (e.g. debugging it in Workstation)
		if(!engine->getExclusiveMode())
		{
			QCoreApplication::processEvents();
		}
	}

	//Stop all of the control elements
	foreach(QSharedPointer<ResultContainer> control, elements_)
	{
		control.staticCast<ControlElement>()->stop(engine);
	}

	//run the exit script
	runExitScript();

	return result;
}

/*! \brief When a State is run as a slave, it really doesn't do anything except render frames until the State ends, since
 *	the SlaveExperimentDriver handles changing of Property values.  This function is therefore empty and slaveRenderFrame()
 *	handles frame rendering.
 */
QString State::slaveRun(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result = "";
	return result;
}

/*! \brief Handles rendering of the latest frame to the display while this State is active and when the Experiment is
 *	running in slave mode.
 */
QString State::slaveRenderFrame(QSharedPointer<Engine::PictoEngine> engine)
{
	sigChannel_ = engine->getSignalChannel("Position");
	//Add a cursor for the user input
	addCursor();

	QString result = "";
	//Activate any control targets used by this state's control elements so that they
	//will be visible to the user.
	foreach(QSharedPointer<ResultContainer> control, elements_)
	{
		control.staticCast<ControlElement>()->activateTargets();
	}

	//----------  Draw the scene --------------
	scene_->render(engine,getAssetId());

	//---------   Erase the latest cursor values (This happens in master when data is sent to server)
	//sigChannel_->getValues();
	engine->emptySignalChannels();	//Make sure all signal channels are emptied, not just the one that we used.

	//Deactivate control targets used by this state's control elements
	foreach(QSharedPointer<ResultContainer> control, elements_)
	{
		control.staticCast<ControlElement>()->deactivateTargets();
	}
	return result;
}

/*! \brief Checks the current engine command and returns true if we need to stop, false otherwise.
 */
bool State::checkForEngineStop(QSharedPointer<Engine::PictoEngine> engine)
{
	int command = engine->getEngineCommand();

	//To stop, we set isDone to true which breaks out of our loop.
	if(command == Engine::PictoEngine::StopEngine)
	{
		return true;
	}
	return false;
}

/*! \brief Adds a cursor to the Scene so that it can display the current value of the Position signal.
 *	\details This should only be used in Slave mode since we don't show the cursor to the test subject.
 */
void State::addCursor()
{
	//We should only add the cursor the first time we run as a slave.
	if(hasCursor_)
		return;

	QSharedPointer<CursorGraphic> cursor(new CursorGraphic(sigChannel_, QColor(255,50,0,255)));
	cursor->setLayer(100000);

	scene_->addVisualElement(cursor);

	hasCursor_ = true;
}

void State::upgradeVersion(QString deserializedVersion)
{
	MachineContainer::upgradeVersion(deserializedVersion);

	if(deserializedVersion < "0.0.1")
	{
		//Before 0.0.1, newClick on operatorClick info had the effect of reading out the current
		//"clicked" value, and also resetting it.  In 0.0.1, we changed it such that the user
		//must reset it manually.  Since there aren't too many experiments yet, we know how and
		//where this script was used and we are changing operatorClick.newClick to 
		//(operatorClick.newClick && !(operatorClick.newClick = false)) in all places that it was
		//used.
		QString frameScript = propertyContainer_->getPropertyValue("FrameScript").toString();
		frameScript.replace("operatorClick.newClick","(operatorClick.newClick && !(operatorClick.newClick = false))");
		propertyContainer_->setPropertyValue("FrameScript",frameScript);
	}
}

/*! \brief Extends MachineContainer::setDesignConfig() to connect to signals associated with changing active Analyses so
 *	that we can respond when that happens.
 */
void State::setDesignConfig(QSharedPointer<DesignConfig> designConfig)
{
	//We need to know whenever Analyses are activated or deactivated, so we connect to the appropriate signal from the
	//DesignConfig.
	if(getDesignConfig())
		disconnect(getDesignConfig().data(),SIGNAL(activeAnalysisIdsChanged()),this,SLOT(activeAnalysisIdsChanged()));
	MachineContainer::setDesignConfig(designConfig);
	connect(getDesignConfig().data(),SIGNAL(activeAnalysisIdsChanged()),this,SLOT(activeAnalysisIdsChanged()));
}

void State::postDeserialize()
{
	MachineContainer::postDeserialize();
	//We're not using this right now, but maybe someday we will, so we're not getting rid of it, just hiding it from
	//the UI.
	propertyContainer_->getProperty("BackgroundColor")->setVisible(false);
}

/*! \brief Extends MachineContainer::validateObject() to verify that this State has at least one ControlElement.
 */
bool State::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	////Validate Results (so they're ready for link checking)
	if(!MachineContainer::validateObject(xmlStreamReader))
		return false;
	
	//Start up all of the control elements
	//Check this.  It causes problems if you add a control element and validate before saving.
	if(!elements_.size())
	{
		addError("At least one Control Element must be defined inside a State.");
		return false;
	}

	return true;
}

bool State::hasScripts()
{
	if(MachineContainer::hasScripts())
		return true;
	return ( !propertyContainer_->getPropertyValue("FrameScript").toString().isEmpty());
}

QMap<QString,QString> State::getScripts()
{
	QMap<QString,QString> scripts = MachineContainer::getScripts();
	if(!propertyContainer_->getPropertyValue("FrameScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"Frame";
		scripts[scriptName] = QString("FrameScript");
	}

	return scripts;
}

/*! \brief Runs this State's AnalysisFrameScript if it isn't empty.
 */
void State::runAnalysisFrameScripts()
{
	runAnalysisScripts(StateMachineElement::FRAME);
}

/*! \brief Initializes the Scene based on all current in scope OutputElement objects so that it can correctly render the
 *	current scene each frame.
 *	\details This needs to be called at least once before a State is run and after all OutputElement objects that are in
 *	scope have been added.
 */
void State::rebuildScene()
{
	//Since the scene needs access to visual elements stored above it in the tree, we get
	//our output elements from the output element list.
	scene_ = Scene::createScene();
	hasCursor_ = false;
	QList<QSharedPointer<ContainerElement>> outputs = getElementList();
	QList<QUuid> activeAnalyses = getDesignConfig()->getActiveAnalysisIds();
	QHash<QUuid,bool> activeAnalysisHash;
	foreach(QUuid analysisId,activeAnalyses)
	{
		activeAnalysisHash[analysisId] = true;
	}
	foreach(QSharedPointer<ContainerElement> output, outputs)
	{
		if(output.isNull())
			continue;
		if(output.dynamicCast<VisualElement>())
		{
			//If this is an analysis element and its analysis id isn't active, don't add it to the scene.
			AssociateElement* analysisVisElem = dynamic_cast<AssociateElement*>(output.data());
			if(analysisVisElem && !activeAnalysisHash.contains(analysisVisElem->getAssociateId()))
				continue;
			scene_->addVisualElement(output.staticCast<VisualElement>());
		}
		else if (output.dynamicCast<OutputSignal>())
		{
			scene_->addOutputSignal(output.staticCast<OutputSignal>());
		}
		else if(output.dynamicCast<AudioElement>())
		{
			scene_->addAudioElement(output.staticCast<AudioElement>());
		}
		else if(output.dynamicCast<InputSignal>())
		{
			scene_->addInputSignal(output.staticCast<InputSignal>());
		}
		else if (output.dynamicCast<DataViewElement>())
		{
			scene_->addDataViewElement(output.staticCast<DataViewElement>());
		}
	}
	QColor backgroundColor;
	backgroundColor.setNamedColor(propertyContainer_->getPropertyValue("BackgroundColor").toString());
	scene_->setBackgroundColor(QColor(propertyContainer_->getPropertyValue("BackgroundColor").toString()));
}

/*! \brief Calls rebuildScene() any time the Active Analysis changes.
 *	\details This should be more useful when/if we start adding Analysis Output Elements (plots or text info)
 *	to the scene in the Workstation when Analyses are active.
 */
void State::activeAnalysisIdsChanged()
{
	//Even though this happens as a result of a change in the UI.  Since its connected on a signal->slot basis, the
	//  function should run in the Experiment thread.  Since the experiment thread never runs during the course of scene
	//  rendering (if they are in two different threads, the experiment thread pauses and waits), there should be no issue
	//  of accidentally changing scene values while they are being used in the UI thread.
	rebuildScene();
}

}; //namespace Picto
