
#include "State.h"
#include "../controlelements/ControlElementFactory.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../engine/PictoEngine.h"
#include "../storage/BehavioralDataUnitPackage.h"
#include "../storage/FrameDataUnitPackage.h"
#include "../engine/SignalChannel.h"
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

#include <QCoreApplication>
#include <QUuid>

namespace Picto {

State::State() :
	MachineContainer("Transition","ControlElement"),
	scene_(QSharedPointer<Scene>(new Scene)),
	hasCursor_(false)
{
	//Define script properties
	AddDefinableProperty("FrameScript","");
	AddDefinableProperty(QVariant::Color,"BackgroundColor","");
	AddDefinableProperty(QVariant::Int,"Revision",-1);
	AddDefinableProperty(QVariant::Int,"EngineNeeded",-1);
	
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

	//AddDefinableObjectFactory("Scene",QSharedPointer<AssetFactory>(new AssetFactory(1,-1,AssetFactory::NewAssetFnPtr(Scene::Create))) );
	//AddDefinableObjectFactory("Link",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ControlLink::Create))) );


	//Add UI Elements
	//Add Audio Elements
	//AddDefinableObjectFactory("AudioElement",QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(AudioElement::Create))) );
	////Add Visual Elements
	//QSharedPointer<AssetFactory> factory(new AssetFactory());
	//factory->addAssetType(ArrowGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ArrowGraphic::Create))));
	//factory->addAssetType(BoxGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BoxGraphic::Create))));
	//factory->addAssetType(CircleGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CircleGraphic::Create))));
	//factory->addAssetType(EllipseGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(EllipseGraphic::Create))));
	//factory->addAssetType(LineGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LineGraphic::Create))));
	//factory->addAssetType(PictureGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PictureGraphic::Create))));
	//factory->addAssetType(RandomlyFilledGridGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomlyFilledGridGraphic::Create))));
	//factory->addAssetType(TextGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TextGraphic::Create))));
	//AddDefinableObjectFactory("VisualElement",factory);
	//Add background color property




	//at some point, these should actually do something, but for the moment
	//we'll leave them as -1 to show that they aren't being used
	revision_ = -1;
	engineNeeded_ = -1;

}

QString State::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();

	sigChannel_ = engine->getSignalChannel("PositionChannel");
	//Add a cursor for the user input
	//addCursor();

	//Figure out which scripts we will be running
	bool runEntryScript = !propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty();
	bool runFrameScript = !propertyContainer_->getPropertyValue("FrameScript").toString().isEmpty();
	bool runExitScript = !propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty();

	QString entryScriptName = getName().simplified().remove(' ')+"Entry";
	QString frameScriptName = getName().simplified().remove(' ')+"Frame";
	QString exitScriptName = getName().simplified().remove(' ')+"Exit";

	//run the entry script
	if(runEntryScript)
		runScript(entryScriptName);

	//Start up all of the control elements
	foreach(QSharedPointer<ResultContainer> control, elements_)
	{
		control.staticCast<ControlElement>()->start(engine);
	}

	QString result = "";
	bool isDone = false;

	//This is the "rendering loop"  It gets run for every frame
	while(!isDone)
	{
		//----------  Draw the scene --------------
		scene_->render(engine);

		//------------- Send Behavioral data to server --------------
		sendBehavioralData(engine);

		//---------- Check for directives from the server -----------
		//updateServer(engine);

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
						break;
					}
				}
				isDone = true;
			}
		}
		if(elements_.isEmpty())
			isDone = true;

		//-------------- Run the frame script ----------------
		if(runFrameScript && !isDone)
			runScript(frameScriptName);

		//------ Check for engine stop commands ---------------
		if(checkForEngineStop(engine))
		{
			isDone = true;
			result = "EngineAbort";
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
	if(runExitScript)
		runScript(exitScriptName);

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
QString State::runAsSlave(QSharedPointer<Engine::PictoEngine> engine)
{
	//resetScriptableValues();
	sigChannel_ = engine->getSignalChannel("PositionChannel");
	lastFrameCheckTime_ = lastTransitionTime_;

	//Add a cursor for the user input
	addCursor();
	
	//Figure out which scripts we will be running
	//bool runEntryScript = !propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty();
	//bool runFrameScript = !propertyContainer_->getPropertyValue("FrameScript").toString().isEmpty();
	//bool runExitScript = !propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty();

	//QString entryScriptName = getName().simplified().remove(' ')+"Entry";
	//QString frameScriptName = getName().simplified().remove(' ')+"Frame";
	//QString exitScriptName = getName().simplified().remove(' ')+"Exit";

	////run the entry script
	//if(runEntryScript)
	//	runScript(entryScriptName);

	QString result = "";
	bool isDone = false;

	//Start up all of the control elements.
	//THE ONLY REASON THAT WE DO THIS IS TO SET ALL OF THIS CONTROL ELEMENTS
	//CONTROL TARGETS AS ACTIVE.  WE SHOULD PROBABLY BREAK OUT A FUNCTION TO JUST
	//DO THAT
	foreach(QSharedPointer<ResultContainer> control, elements_)
	{
		control.staticCast<ControlElement>()->start(engine);
	}

	//This is the "rendering loop"  It gets run for every frame
	while(!isDone)
	{
		//------ !!!! WARNING !!!! ------
		//The ordering in this rendering loop is really, really sensitive.
		//Think long and hard before rearranging code.  If you do rearrange, 
		//you'll need to test pretty thoroughly.

		//In slave mode, we always process events
		QCoreApplication::processEvents();

		//----------  Draw the scene --------------
		scene_->render(engine);

		//--------- Check for master state change ------------

		if(!engine->updateCurrentStateFromServer())
		{	//The server connection has been lost
			engine->stop();
		}
		result = engine->getServerPathUpdate();
		if(!result.isEmpty())
			isDone = true;

		//------ Check for engine stop commands ---------------
		//This has to be done first, otherwise if we are caught up with the master, 
		//we'll never check for a stop
		if(checkForEngineStop(engine))
		{
			isDone = true;
			result = "EngineAbort";
		}
	}

	//Stop all of the control elements
	//THE ONLY REASON THAT WE DO THIS IS TO SET ALL OF THIS CONTROL ELEMENTS
	//CONTROL TARGETS AS ACTIVE.  WE SHOULD PROBABLY BREAK OUT A FUNCTION TO JUST
	//DO THAT
	foreach(QSharedPointer<ResultContainer> control, elements_)
	{
		control.staticCast<ControlElement>()->stop(engine);
	}
	////run the exit script
	//if(runExitScript)
	//	runScript(exitScriptName);

	return result;
}

/*! \brief Sends the current behavioral data to the server
 *
 *	We update the server with all of the usefule behavioral data.  This includes 
 *	the time at which the most recent frame was drawn, as well as all the input
 *	coordinates from the subject.  Since we're in a hurry, we use the "registered"
 *	command functions in the command channel to send commands without having to 
 *	wait for reponses.  The responses we get will all contain directives, so we 
 *	have to handle those here as well.
 */
void State::sendBehavioralData(QSharedPointer<Engine::PictoEngine> engine)
{
	//Create a new frame data store
	FrameDataUnitPackage frameData;
	Timestamper stamper;

	frameData.addFrame(scene_->getLatestFirstPhosphorTime(),getAssetId());
	engine->setLastFrame(frameData.getLatestFrameId());

	//Update the BehavioralDataUnitPackage
	BehavioralDataUnitPackage behavData;

	QSharedPointer<CommandChannel> dataChannel = engine->getDataCommandChannel();

	if(dataChannel.isNull())
		return;

	//Note that the call to getValues clears out any existing values,
	//so it should only be made once per frame.
	behavData.emptyData();
	behavData.addData(sigChannel_->getValues());

	QSharedPointer<PropertyDataUnitPackage> propPack = engine->getChangedPropertyPackage();
	QSharedPointer<StateDataUnitPackage> statePack = engine->getStateDataPackage();

	//send a PUTDATA command to the server with the most recent behavioral data
	QSharedPointer<Picto::ProtocolResponse> dataResponse;
	QString status = "running";
	int engCmd = engine->getEngineCommand();
	switch(engCmd)
	{
	case Engine::PictoEngine::ResumeEngine:
		status = "running";
		break;
	case Engine::PictoEngine::PauseEngine:
		status = "paused";
		break;
	case Engine::PictoEngine::StopEngine:
		status = "stopped";
		break;
	}
	QString dataCommandStr = "PUTDATA " + engine->getName() + ":" + status + " PICTO/1.0";
	QSharedPointer<Picto::ProtocolCommand> dataCommand(new Picto::ProtocolCommand(dataCommandStr));

	QByteArray dataXml;
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&dataXml));

	xmlWriter->writeStartElement("Data");
	if(behavData.length())
		behavData.toXml(xmlWriter);
	if(propPack && propPack->length())
		propPack->toXml(xmlWriter);
	if(statePack && statePack->length())
		statePack->toXml(xmlWriter);
	QList<QSharedPointer<RewardDataUnit>> rewards = engine->getDeliveredRewards();
	foreach(QSharedPointer<RewardDataUnit> reward,rewards)
	{
		reward->toXml(xmlWriter);
	}
	frameData.toXml(xmlWriter);	//Frame data must go last so that server knows when it reads
								//in a frame, that the data it has defines the state that was
								//in place at that frame.
	xmlWriter->writeEndElement();

	dataCommand->setContent(dataXml);
	dataCommand->setFieldValue("Content-Length",QString::number(dataXml.length()));
	QUuid commandUuid = QUuid::createUuid();

	dataChannel->sendRegisteredCommand(dataCommand);

	dataChannel->processResponses(0);
	//The line below is very fast if the connection isn't broken and takes up to 5 ms reconnecting if it is.
	//This shouldn't be a problem since there should be some time to spare in the state's run loop before the
	//next frame.
	//QTime timer;
	//timer.start();
	dataChannel->assureConnection(5);
	//qDebug(QString("ASSURECONNECTION time: %1").arg(timer.elapsed()).toAscii());
	////check for and process responses
	//while(dataChannel->waitForResponse(0))
	//{
	//	dataResponse = dataChannel->getResponse();
	//	Q_ASSERT(!dataResponse.isNull());
	//	Q_ASSERT(dataResponse->getResponseType() == "OK");
	//	processStatusDirective(engine,dataResponse);

	//}

	//This is useful for debugging, but we should let the state machine handle server drop-outs
	//Q_ASSERT_X(dataChannel->pendingResponses() < 10, "State::Run()","Too many commands sent without receiving responses");

}


//! Runs a script
//void State::runScript(QString scriptName)
//{
//	qsEngine_->globalObject().property(scriptName).call();
//	if(qsEngine_->hasUncaughtException())
//	{
//		QString errorMsg = "Uncaught exception in State" + getName() +", script "+scriptName+"\n";
//		errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
//										  .arg(qsEngine_->uncaughtException().toString());
//		errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
//		qDebug()<<errorMsg;
//	}
//}

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
	else if(command == Engine::PictoEngine::PauseEngine)
	{
		QSharedPointer<Picto::CommandChannel> dataChannel = engine->getDataCommandChannel();
		while(command == Engine::PictoEngine::PauseEngine)
		{
			////waste 30 ms
			QTime timer;
			timer.start();
			//----------  Draw the scene in paused state --------------
			//scene_->render(engine);
			//------------- Send Behavioral data to server --------------
			sendBehavioralData(engine);

			command = engine->getEngineCommand();
			QCoreApplication::processEvents();
			while(timer.elapsed()<30);

		}
		if(command == Engine::PictoEngine::StopEngine)
		{
			return true;
		}
	}

	return false;
}

/*	\brief Sends an UPDATECOMPONENT command and deals with any directives included in the response
 *
 *	Once per frame, we need to check in with the server.  The response from the server
 *  may contain a "directive", which we will handle here as well (e.g. stop, pause, etc).
 */
/*void State::updateServer(QSharedPointer<Engine::PictoEngine> engine, bool paused)
{
	QSharedPointer<Picto::CommandChannel>updateChan = engine->getUpdateCommandChannel();
	
	if(updateChan.isNull())
		return;

	while(updateChan->incomingResponsesWaiting())
	{
		updateChan->getResponse();
		Q_ASSERT(false);	//This will break things in debug, but ignore issues in release
	}


	QSharedPointer<Picto::ProtocolResponse> updateResponse;

	QString updateCommandStr;

	if(paused)
		updateCommandStr = "COMPONENTUPDATE "+engine->getName()+":paused PICTO/1.0";
	else
		updateCommandStr = "COMPONENTUPDATE "+engine->getName()+":running PICTO/1.0";

	QSharedPointer<Picto::ProtocolCommand> updateCommand(new Picto::ProtocolCommand(updateCommandStr));

	updateChan->sendCommand(updateCommand);

	if(!updateChan->waitForResponse(50))
	{
		Q_ASSERT_X(false,"State::updateServer", "Server failed to reply to COMPONENTUPDATE command within 100 ms");
		return;
	}

	updateResponse = updateChan->getResponse();
	Q_ASSERT(updateResponse);
	Q_ASSERT(updateResponse->getResponseType() == "OK");

	QString statusDirective = updateResponse->getDecodedContent().toUpper();

	if(statusDirective.startsWith("OK"))
	{
		//do nothing
	}
	else if(statusDirective.startsWith("STOP"))
	{
		engine->stop();
	}
	else if(statusDirective.startsWith("PAUSE"))
	{
		engine->pause();
	}
	else if(statusDirective.startsWith("RESUME"))
	{
		engine->resume();
	}
	else if(statusDirective.startsWith("REWARD"))
	{
		int channel = statusDirective.split(" ").value(1).toInt();
		engine->giveReward(channel);	
	}
	else
	{
		Q_ASSERT_X(false, "State::updateServer", "Unrecognized directive received from server");
	}

}*/

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
//			qDebug(errorMsg.toAscii());
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
	revision_= propertyContainer_->getPropertyValue("Revision").toInt();
	engineNeeded_= propertyContainer_->getPropertyValue("EngineNeeded").toInt();

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

QMap<QString,QString> State::getScripts()
{
	QMap<QString,QString> scripts = MachineContainer::getScripts();
	if(!hasScripts())
		return scripts;

	if(!propertyContainer_->getPropertyValue("FrameScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"Frame";
		scripts[scriptName] = propertyContainer_->getPropertyValue("FrameScript").toString();
	}
	return scripts;
}

void State::scriptableContainerWasReinitialized()
{
	scene_ = QSharedPointer<Scene>(new Scene);
	hasCursor_ = false;
	QList<QSharedPointer<Scriptable>> scriptables = getScriptableList();
	foreach(QSharedPointer<Scriptable> scriptable,scriptables)
	{
		if(scriptable.dynamicCast<VisualElement>())
		{
			scene_->addVisualElement(scriptable.staticCast<VisualElement>());
		}
	}
	QColor backgroundColor;
	backgroundColor.setNamedColor(propertyContainer_->getPropertyValue("BackgroundColor").toString());
	scene_->setBackgroundColor(QColor(propertyContainer_->getPropertyValue("BackgroundColor").toString()));
}

}; //namespace Picto
