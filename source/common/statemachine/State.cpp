
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
#include "../controlelements/TargetController.h"
#include "../controlelements/ChoiceController.h"

#include <QCoreApplication>
#include <QUuid>

namespace Picto {

State::State() :
	scene_(QSharedPointer<Scene>(new Scene)),
	hasCursor_(false)
{
	AddDefinableProperty("Name","");
	AddDefinableProperty(QVariant::Int,"Revision",-1);
	AddDefinableProperty(QVariant::Int,"EngineNeeded",-1);
	
	DefinePlaceholderTag("ControlElements");
	QSharedPointer<AssetFactory> ctrlElemFactory(new AssetFactory());
	ctrlElemFactory->addAssetType(TestController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TestController::Create))));
	ctrlElemFactory->addAssetType(StopwatchController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(StopwatchController::Create))));
	ctrlElemFactory->addAssetType(TargetController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TargetController::Create))));
	ctrlElemFactory->addAssetType(ChoiceController::ControllerType(),
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ChoiceController::Create))));
	AddDefinableObjectFactory("ControlElement",ctrlElemFactory);

	AddDefinableObjectFactory("Scene",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Scene::Create))) );
	
	DefinePlaceholderTag("Scripts");
	AddDefinableProperty("EntryScript","");
	AddDefinableProperty("FrameScript","");
	AddDefinableProperty("ExitScript","");

	//at some point, these should actually do something, but for the moment
	//we'll leave them as -1 to show that they aren't being used
	revision_ = -1;
	engineNeeded_ = -1;

}

QString State::run(QSharedPointer<Engine::PictoEngine> engine)
{
	reset();
	frameCounter_ = -1; //We're zero-indexed

	sigChannel_ = engine->getSignalChannel("PositionChannel");

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
	foreach(QSharedPointer<ControlElement> control, controlElements_)
	{
		control->start(engine);
	}

	QString result = "";
	bool isDone = false;

	//This is the "rendering loop"  It gets run for every frame
	while(!isDone)
	{

		//----------  Draw the scene --------------
		scene_->render(engine);
		frameCounter_ ++;

		//------------- Send Behavioral data to server --------------
		sendBehavioralData(engine);

		//---------- Check for directives from the server -----------
		//updateServer(engine);

		//--------- Check control elements------------
		foreach(QSharedPointer<ControlElement> control, controlElements_)
		{
			if(control->isDone(engine))
			{
				result = control->getResult();
				isDone = true;
			}
		}
		if(controlElements_.isEmpty())
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
	reset();
	sigChannel_ = engine->getSignalChannel("PositionChannel");
	lastFrameCheckTime_ = lastTransitionTime_;

	//Add a cursor for the user input
	addCursor();
	
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

	QString result = "";
	bool isDone = false;
	frameCounter_ = -1;

	//This is the "rendering loop"  It gets run for every frame
	while(!isDone)
	{
		//------ !!!! WARNING !!!! ------
		//The ordering in this rendering loop is really, really sensitive.
		//Think long and hard before rearranging code.  If you do rearrange, 
		//you'll need to test pretty thoroughly.

		//In slave mode, we always process events
		QCoreApplication::processEvents();
		
		//--------- Check for master state change ------------

		result = getMasterStateResult(engine);
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

		//--------- Check for master frame ------------
		//Since this "continues" the loop, it has to occur after checking
		//for a state change

		int masterFrame = getMasterFramenumber(engine);
		if(!isDone && masterFrame <= frameCounter_)
		{
			continue;
		}


		//Run the frame scripts enough to catch up
		if(!isDone && runFrameScript)
		{
			for(int i=0; i<masterFrame - frameCounter_; i++)
			{
				runScript(frameScriptName);
			}
		}

		//----------  Draw the scene --------------
		scene_->render(engine);

		frameCounter_ = masterFrame;
	}

	//run the exit script
	if(runExitScript)
		runScript(exitScriptName);

	return result;
}

//!  Resets all of the contained visual stimuli back to their original state (in case they were modified by a script)
void State::reset()
{
	scene_->reset();

	//We may want to consider resetting the scripting environment as well.  At the moment, variables stored
	//in script will maintain their state from run to run.  This could be good or bad...
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

	frameData.addFrame(frameCounter_,stamper.stampSec(),getName());

	//Update the BehavioralDataUnitPackage
	BehavioralDataUnitPackage behavData;

	QSharedPointer<CommandChannel> dataChannel = engine->getDataCommandChannel();

	if(dataChannel.isNull())
		return;

	//Note that the call to getValues clears out any existing values,
	//so it should only be made once per frame.
	behavData.emptyData();
	behavData.addData(sigChannel_->getValues());

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
	frameData.toXml(xmlWriter);
	if(behavData.length())
		behavData.toXml(xmlWriter);
	xmlWriter->writeEndElement();

	dataCommand->setContent(dataXml);
	dataCommand->setFieldValue("Content-Length",QString::number(dataXml.length()));
	QUuid commandUuid = QUuid::createUuid();

	dataChannel->sendRegisteredCommand(dataCommand);

	dataChannel->processResponses(0);
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
void State::runScript(QString scriptName)
{
	qsEngine_->globalObject().property(scriptName).call();
	if(qsEngine_->hasUncaughtException())
	{
		QString errorMsg = "Uncaught exception in State" + getName() +", script "+scriptName+"\n";
		errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
										  .arg(qsEngine_->uncaughtException().toString());
		errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
		qDebug()<<errorMsg;
	}
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
	else if(command == Engine::PictoEngine::PauseEngine)
	{
		QSharedPointer<Picto::CommandChannel> dataChannel = engine->getDataCommandChannel();
		while(command == Engine::PictoEngine::PauseEngine)
		{
			//waste 30 ms
			QTime timer;
			timer.start();
			//updateServer(engine, true);
			sendBehavioralData(engine);
			if(dataChannel.isNull())
				QCoreApplication::processEvents();
			command = engine->getEngineCommand();
			while(timer.elapsed()<30);

		}
		if(command == Engine::PictoEngine::StopEngine)
		{
			return true;
		}
	}

	return false;
}

/*! \brief returns the frame of the master state machine
 *
 *	When running in slave mode, we need to figure out what the current frame number 
 *	of the master engine is.  This function returns that value.  To figure out the 
 *	frame number we use GETDATA FrameDataUnitPackage.  If something goes wrong, this functio
 *	returns a negative value.
 */
int State::getMasterFramenumber(QSharedPointer<Engine::PictoEngine> engine)
{
	QString commandStr = QString("GETDATA FrameDataUnitPackage:%1 PICTO/1.0").arg(lastFrameCheckTime_);
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(commandStr));
	QSharedPointer<Picto::ProtocolResponse> response;

	CommandChannel* slaveToServerChan = engine->getSlaveCommandChannel();
	slaveToServerChan->sendCommand(command);

	if(!slaveToServerChan->waitForResponse(1000))
		return -1;

	response = slaveToServerChan->getResponse();

	if(response->getResponseCode() != Picto::ProtocolResponseType::OK)
		return -2;

	QByteArray xmlFragment = response->getContent();
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));

	while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "Data");

	if(xmlReader->atEnd())
		return -3;

	xmlReader->readNext();
	while(!xmlReader->isEndElement() && xmlReader->name() != "Data" && !xmlReader->atEnd())
	{
		if(xmlReader->name() == "FrameDataUnitPackage")
		{
			FrameDataUnitPackage dataStore;
			dataStore.fromXml(xmlReader);
			if(dataStore.length() <1)
				return -1;
			
			//Since the frames are returned in order, we can simply pull off the 
			//last frame, and know that it is the most recently reported frame
			QSharedPointer<FrameDataUnit> data;
			data = dataStore.takeLastDataPoint();

			lastFrameCheckTime_ = data->time;
			return data->frameNumber;
		}
		else
		{
			return -1;
		}
	}
	return -1;
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

	if(!updateChan->waitForResponse(100))
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

	QSharedPointer<CursorGraphic> cursor(new CursorGraphic(sigChannel_, QColor(255,0,0,255)));

	//Create a new layer
	QSharedPointer<Layer> cursorLayer(new Layer());
	cursorLayer->setOrder(100);  //This puts it on top
	cursorLayer->addVisualElement(cursor);
	
	//add the layer to our state/scene/canvas
	scene_->getCanvas()->addLayer(cursorLayer);

	hasCursor_ = true;
}


/* \Brief Sets up the state for scripting
 *
 *	This gets called before we actually start running a state machine.
 *	Because of scoping issues, each State gets its own scriptengine
 *	To initialize a state, we need to do the following:
 *		1. Bind the parameters to the engine
 *		2. Bind the contained visual elements to the engine
 *		3. Create functions in the engine for the 3 (potential) scripts.
 *	To minimize wasted resources, the state doesn't use a concrete QScriptEngine
 *	but insteads instantiates one if the state needs to run scripts.  The
 *	assumption is that many states (at least 50%) won't need the scripting
 *	functionality.
 */
bool State::initScripting(QScriptEngine &qsEngine)
{
	//Since we have our own engine, we aren't using the passed in one
	Q_UNUSED(qsEngine);

	QStringList scriptTypes;
	scriptTypes<<"Entry"<<"Frame"<<"Exit";

	//Figure out which scripts we will be running
	//(The odd array structure is useful later...)
	bool scriptsToRun[3];
	scriptsToRun[0] = !propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty();
	scriptsToRun[1] = !propertyContainer_->getPropertyValue("FrameScript").toString().isEmpty();
	scriptsToRun[2] = !propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty();

	//If we aren't running any scripts, we're done
	bool noScripts = true;
	for(int i=0; i<sizeof(scriptsToRun); i++)
	{
		if(scriptsToRun[i])
		{
			noScripts = false;
			break;
		}
	}
	if(noScripts)
		return true;


	//create the engine
	qsEngine_ = QSharedPointer<QScriptEngine>(new QScriptEngine());

	//Bind our parameters
	parameterContainer_.bindToScriptEngine(*qsEngine_);

	//Bind the visualElements
	scene_->bindToScriptEngine(qsEngine_);

	//create functions for our scripts
	for(int i=0; i<3; i++)
	{
		if(!scriptsToRun[i])
			continue;

		//We allow ScriptElement names to have whitespace, so we need 
		//to get rid of it for a script function name
		QString functionName = getName().simplified().remove(' ')+scriptTypes[i];

		//confirm that this function doesn't already exist in the engine
		//If you hit this assert, it means that there are two ScriptElements
		//with the asme name.  The GUI shouldn't allow this...
		if(qsEngine_->globalObject().property(functionName).isValid())
		{
			return false;
		}

		QString script = propertyContainer_->getPropertyValue(scriptTypes[i]+"Script").toString();
		QString function = "function " + functionName + "() { " + script + "}";

		//add the function to the engine by calling evaluate on it
		qsEngine_->evaluate(function);
		if(qsEngine_->hasUncaughtException())
		{
			QString errorMsg = "Uncaught exception in State" + getName() + scriptTypes[i] + " script.\n";
			errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
											  .arg(qsEngine_->uncaughtException().toString());
			errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
			qDebug(errorMsg.toAscii());
			return false;
		}
	}

	return true;
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
 *		<Scripts>
 *			<Script type="entry"> ... </Script>
 *			<Script type="frame "> ... </Script>
 *			<Script type="exit"> ... </Script>
 *		</Scripts>

 *		<Results>
 *			<!-- We'll list all possible results from the ControlElements here -->
 *			<Result>
 *				<Name>..</Name>
 *			</Result>
 *		</Results>
 *	</StateMachineElement>
 */
//bool State::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("StateMachineElement");
//	xmlStreamWriter->writeAttribute("type","State");
//	
//	xmlStreamWriter->writeTextElement("Revision",QString("%1").arg(revision_));
//	xmlStreamWriter->writeTextElement("EngineNeeded",QString("%1").arg(engineNeeded_));
//	xmlStreamWriter->writeTextElement("Name", propertyContainer_->getPropertyValue("Name").toString());
//
//	xmlStreamWriter->writeStartElement("ControlElements");
//	foreach(QSharedPointer<ControlElement> c, controlElements_)
//	{
//		c->toXml(xmlStreamWriter);
//	}
//	xmlStreamWriter->writeEndElement();
//
//	scene_->toXml(xmlStreamWriter);
//
//	serializeResults(xmlStreamWriter);
//
//	xmlStreamWriter->writeEndElement(); //State
//	return true;
//}
//
///*!	\Brief Turns a an XML fragment into a State object */
//bool State::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateMachineElement")
//	{
//		addError("State","Incorrect tag, expected <StateMachineElement>",xmlStreamReader);
//		return false;
//	}
//	if(xmlStreamReader->attributes().value("type").toString() != type())
//	{
//		addError("State","Incorrect type of StateMachineElement, expected State",xmlStreamReader);
//		return false;
//	}
//
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateMachineElement") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->name().toString();
//		if(name == "Revision")
//		{
//			bool ok;
//			revision_= xmlStreamReader->readElementText().toInt(&ok);
//			if(!ok)
//			{
//				addError("State","Revision value not an integer", xmlStreamReader);
//				return false;
//			}
//		}
//		else if(name == "EngineNeeded")
//		{
//			bool ok;
//			engineNeeded_= xmlStreamReader->readElementText().toInt(&ok);
//			if(!ok)
//			{
//				addError("State","EngineNeeded value not an integer", xmlStreamReader);
//				return false;
//			}
//		}
//		else if(name == "Name")
//		{
//			propertyContainer_->setPropertyValue("Name",QVariant(xmlStreamReader->readElementText()));
//		}
//		else if(name == "ControlElements")
//		{
//			//do nothing
//		}
//		else if(name == "ControlElement")
//		{
//			ControlElementFactory factory;
//			QString controllerType = xmlStreamReader->attributes().value("type").toString();
//			QSharedPointer<ControlElement> newController = factory.generateControlElement(controllerType);
//			if(!newController)
//			{
//				addError("State", "Failed to create a new Controller of type "+controllerType,xmlStreamReader);
//				return false;
//			}
//			if(!newController->fromXml(xmlStreamReader))
//			{
//				addError("State", "Failed to deserialize <ControlElement>", xmlStreamReader);
//				return false;
//			}
//			addControlElement(newController);
//		}
//		else if(name == "Scene")
//		{
//			QSharedPointer<Scene> newScene(new Scene);
//			if(!newScene->fromXml(xmlStreamReader))
//			{
//				addError("State", "Failed to deserialize <Scene>", xmlStreamReader);
//				return false;
//			}
//			scene_ = newScene;
//		}
//		else if(name == "Scripts")
//		{
//			//do nothing
//		}
//		else if(name == "Script")
//		{
//			QString type = xmlStreamReader->attributes().value("type").toString();
//			QString script = xmlStreamReader->readElementText();
//
//			if(type == "entry")
//			{
//				propertyContainer_->setPropertyValue("EntryScript",script);
//			}
//			else if(type == "frame")
//			{
//				propertyContainer_->setPropertyValue("FrameScript",script);
//			}
//			else if(type == "exit")
//			{
//				propertyContainer_->setPropertyValue("ExitScript",script);
//			}
//			else
//			{
//				addError("State", "Unrecognized script type.  Expecting \"entry\", \"frame\", or \"exit\"", xmlStreamReader);
//				return false;
//			}
//		}
//		else if(name == "Results")
//		{
//			if(!deserializeResults(xmlStreamReader))
//			{
//				addError("State", "Failed to deserialize <Results>", xmlStreamReader);
//				return false;
//			}
//		}
//
//		else
//		{
//			addError("State", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//		xmlStreamReader->readNext();
//	}
//
//	if(xmlStreamReader->atEnd())
//	{
//		addError("State", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//
//	return true;
//}

bool State::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	revision_= propertyContainer_->getPropertyValue("Revision").toInt();
	engineNeeded_= propertyContainer_->getPropertyValue("EngineNeeded").toInt();

	QList<QSharedPointer<Asset>> newCtrlElems = getGeneratedChildren("ControlElement");
	foreach(QSharedPointer<Asset> newCtrlElem,newCtrlElems)
	{
		addControlElement(newCtrlElem.staticCast<ControlElement>());
	}

	scene_ = getGeneratedChildren("Scene").front().staticCast<Scene>();

	//! \todo Validate Scripts

	////Validate Results
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;

	return true;
}

}; //namespace Picto
