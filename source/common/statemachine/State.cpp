#include <QCoreApplication>
#include <QUuid>

#include "State.h"
#include "../controlelements/ControlElementFactory.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../engine/PictoEngine.h"
#include "../storage/FrameDataUnitPackage.h"
#include "../engine/SignalChannel.h"
#include "../stimuli/OutputElement.h"
#include "../timing/Timestamper.h"
#include "../stimuli/CursorGraphic.h"
#include "../controlelements/TestController.h"
#include "../controlelements/StopwatchController.h"
#include "../controlelements/ScriptController.h"
#include "../controlelements/TargetController.h"
#include "../controlelements/ChoiceController.h"
//#include "../stimuli/ArrowGraphic.h"
//#include "../stimuli/BoxGraphic.h"
//#include "../stimuli/CircleGraphic.h"
//#include "../stimuli/EllipseGraphic.h"
//#include "../stimuli/LineGraphic.h"
//#include "../stimuli/PictureGraphic.h"
//#include "../stimuli/RandomlyFilledGridGraphic.h"
//#include "../stimuli/TextGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

State::State() :
	MachineContainer("Transition","ControlElement"),
	scene_(Scene::createScene()),
	hasCursor_(false)
{
	//Define script properties
	AddDefinableProperty("FrameScript","");
	AddDefinableProperty(QVariant::Color,"BackgroundColor","");
	
	//Define generatable control elements.
	elementFactory_->addAssetType(TestController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,1,AssetFactory::NewAssetFnPtr(TestController::Create))));
	elementFactory_->addAssetType(StopwatchController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,1,AssetFactory::NewAssetFnPtr(StopwatchController::Create))));
	elementFactory_->addAssetType(ScriptController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,1,AssetFactory::NewAssetFnPtr(ScriptController::Create))));
	elementFactory_->addAssetType(TargetController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,1,AssetFactory::NewAssetFnPtr(TargetController::Create))));
	elementFactory_->addAssetType(ChoiceController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,1,AssetFactory::NewAssetFnPtr(ChoiceController::Create))));
}

QSharedPointer<Asset> State::Create()
{
	return QSharedPointer<Asset>(new State());
}

void State::enableRunMode(bool enable)
{
	MachineContainer::enableRunMode(enable);
	if(!enable)
		return;
	//Since the scene needs access to visual elements stored above it in the tree, we get
	//our output elements from the output element list.
	scene_ = Scene::createScene();
	hasCursor_ = false;
	QList<QSharedPointer<OutputElement>> outputs = getOutputElementList();
	foreach(QSharedPointer<OutputElement> output,outputs)
	{
		if(output.isNull())
			continue;
		if(output.dynamicCast<VisualElement>())
		{
			scene_->addVisualElement(output.staticCast<VisualElement>());
		}
		else if (output.dynamicCast<OutputSignal>())
		{
			scene_->addOutputSignal(output.staticCast<OutputSignal>());
		}
	}
	QColor backgroundColor;
	backgroundColor.setNamedColor(propertyContainer_->getPropertyValue("BackgroundColor").toString());
	scene_->setBackgroundColor(QColor(propertyContainer_->getPropertyValue("BackgroundColor").toString()));
}

QString State::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();

	//sigChannel_ = engine->getSignalChannel("Position");
	//Add a cursor for the user input
	//addCursor();

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
				result = control.staticCast<ControlElement>()->getResult();
				QList<QSharedPointer<Transition>> elemLinks = transitions_.values(control->getName());
				foreach(QSharedPointer<Transition> link,elemLinks)
				{
					if(link->getSourceResult() == result)
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
		}

		//------ Check for engine stop commands ---------------
		if(checkForEngineStop(engine))
		{
			isDone = true;
			result = "EngineAbort";
		} 
		else if(isDone)
		{
			engine->addStateTransitionForServer(resultTrans);	//Added in Picto Version 1.0.12.  Before this transitions within a state weren't being recorded in the session file.		
			Q_ASSERT(results_.contains(result));		//Added in Picto Version 1.0.12.
			//Run result script if there is one.
			results_.value(result)->runResultScript();
		}
		//If we're not in exclusive mode, we should allocate time to process events
		//This would occur if we were running a state machine somewhere other than
		//Director (e.g. debugging it in Workstation)
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

/*! \brief Runs a state in slave mode
 *
 *	Running in slave mode results in two major differences:
 *		1. The controllers are completely ignored, and we return the result 
 *		   pulled from the master engine over the network.
 *		2. Frames are rendered as behavioral data comes in.  For every piece of
 *		   behavioral data received, we advance one frame.  This keeps us from 
 *		   getting ahead of the master engine.
 */
QString State::slaveRun(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result = "";
	return result;
}

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
	sigChannel_->getValues();

	//Deactivate control targets used by this state's control elements
	foreach(QSharedPointer<ResultContainer> control, elements_)
	{
		control.staticCast<ControlElement>()->deactivateTargets();
	}
	return result;
}

/*! \brief Checks for engine commands and returns true if we need to stop
 *
 *	The commands are a bit funny, but they work.  When Engine::stop() is called,
 *	the engine command gets set to StopEngine (and never gets reset).  This results
 *	in all of our states returning.  When pause is called, we simply sit and spin
 *	until a new command is issued (play or stop). 
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


///* \Brief Sets up the state for scripting
// *
// *	This gets called before we actually start running a state machine.
// *	Because of scoping issues, each State gets its own scriptengine
// *	To initialize a state, we need to do the following:
// *		1. Bind the parameters to the engine
// *		2. Bind the contained visual elements to the engine
// *		3. Create functions in the engine for the 3 (potential) scripts.
// *	To minimize wasted resources, the state doesn't use a concrete QScriptEngine
// *	but insteads instantiates one if the state needs to run scripts.  The
// *	assumption is that many states (at least 50%) won't need the scripting
// *	functionality.
// */
//bool State::initScripting(QScriptEngine &qsEngine)
//{
//	//Since we have our own engine, we aren't using the passed in one
//	Q_UNUSED(qsEngine);
//
//	return initScriptEngine(false);
//}

//bool State::initScriptEngine(bool forScriptDefinitions)
//{
//	QStringList scriptTypes;
//	scriptTypes<<"Entry"<<"Frame"<<"Exit";
//
//	//Figure out which scripts we will be running
//	//(The odd array structure is useful later...)
//	bool scriptsToRun[3];
//	scriptsToRun[0] = !propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty();
//	scriptsToRun[1] = !propertyContainer_->getPropertyValue("FrameScript").toString().isEmpty();
//	scriptsToRun[2] = !propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty();
//
//	//If we aren't running any scripts, and we're not looking for script definitions, we're done
//	if(!forScriptDefinitions)
//	{
//		bool noScripts = true;
//		for(int i=0; i<sizeof(scriptsToRun); i++)
//		{
//			if(scriptsToRun[i])
//			{
//				noScripts = false;
//				break;
//			}
//		}
//		if(noScripts)
//			return true;
//	}
//
//	//create the engine
//	qsEngine_ = QSharedPointer<QScriptEngine>(new QScriptEngine());
//
//	//Bind our parameters and visual elements
//	bindToScriptEngine(*qsEngine_);
//
//	//Bind the visualElements
//	//scene_->bindToScriptEngine(*qsEngine_);
//
//	//create functions for our scripts
//	for(int i=0; i<3; i++)
//	{
//		if(!scriptsToRun[i])
//			continue;
//
//		//We allow ScriptElement names to have whitespace, so we need 
//		//to get rid of it for a script function name
//		QString functionName = getName().simplified().remove(' ')+scriptTypes[i];
//
//		//confirm that this function doesn't already exist in the engine
//		//If you hit this assert, it means that there are two ScriptElements
//		//with the asme name.  The GUI shouldn't allow this...
//		if(qsEngine_->globalObject().property(functionName).isValid())
//		{
//			return false;
//		}
//
//		QString script = propertyContainer_->getPropertyValue(scriptTypes[i]+"Script").toString();
//		QString function = "function " + functionName + "() { " + script + "}";
//
//		//add the function to the engine by calling evaluate on it
//		qsEngine_->evaluate(function);
//		if(qsEngine_->hasUncaughtException())
//		{
//			QString errorMsg = "Uncaught exception in State" + getName() + scriptTypes[i] + " script.\n";
//			errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
//											  .arg(qsEngine_->uncaughtException().toString());
//			errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
//			qDebug(errorMsg.toLatin1());
//			return false;
//		}
//	}
//
//	return true;
//}

//QString State::getInfo()
//{
//	initScripting(true);
//	if(!qsEngine_)
//		return "No Parameters Available";
//	QString returnVal;
//	QScriptValueIterator it(qsEngine_->globalObject());
//	while (it.hasNext())
//	{
//		it.next();
//		if(!(it.flags() & (QScriptValue::SkipInEnumeration | QScriptValue::Undeletable)))
//		{
//			returnVal.append(QString("Name:%1\n").arg(it.getName()));
//			QScriptValueIterator it1(it.value());
//			while(it1.hasNext())
//			{
//				it1.next();
//				if(!(it1.flags() & (QScriptValue::SkipInEnumeration | QScriptValue::Undeletable)))
//				{
//					returnVal.append(QString("\tName:%1\n").arg(it1.getName()));
//				}
//			}
//		}
//	}
//	if(returnVal.length())
//		returnVal.prepend("Available Parameters:\n");
//	return returnVal;
//}

void State::postDeserialize()
{
	//QList<QSharedPointer<Asset>> newVisElems = getGeneratedChildren("VisualElement");
	//foreach(QSharedPointer<Asset> newVisElem,newVisElems)
	//{
	//	scene_->addVisualElement(newVisElem.staticCast<VisualElement>());
	//	addScriptable(newVisElem.staticCast<Scriptable>());
	//}
	MachineContainer::postDeserialize();
}

bool State::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	////Validate Results (so they're ready for link checking)
	if(!MachineContainer::validateObject(xmlStreamReader))
		return false;
	
	//Start up all of the control elements
	//Check this.  It causes problems if you add a control element and validate before saving.
	if(!elements_.size())
	{
		addError("State", "At least one Control Element must be defined inside a State.");
		return false;
	}
	//! \todo Validate Scripts

	return true;
}

bool State::hasScripts()
{
	if(MachineContainer::hasScripts())
		return true;
	return !propertyContainer_->getPropertyValue("FrameScript").toString().isEmpty();
}

QMap<QString,QPair<QString,QString>> State::getScripts()
{
	QMap<QString,QPair<QString,QString>> scripts = MachineContainer::getScripts();
	if(!propertyContainer_->getPropertyValue("FrameScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"Frame";
		scripts[scriptName] = QPair<QString,QString>(QString(),"FrameScript");
	}
	return scripts;
}

}; //namespace Picto
