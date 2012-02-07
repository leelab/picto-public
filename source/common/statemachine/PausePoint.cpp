#include <QCoreApplication>
#include "PausePoint.h"
#include "../engine/PictoEngine.h"
#include "../stimuli/CursorGraphic.h"
#include "../memleakdetect.h"
namespace Picto
{

PausePoint::PausePoint()
: scene_(QSharedPointer<Scene>(new Scene)),
hasCursor_(false)
{
	setMaxOptionalResults(0);
	parameterFactory_->setMaxAssets(0);
	controlTargetFactory_->setMaxAssets(0);
	AddDefinableProperty("Type","PausePoint");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::Color,"BackgroundColor","");
	AddDefinableProperty("PausingScript","");
	AddDefinableProperty("RestartingScript","");
	AddDefinableProperty(QVariant::Bool,"ForcePause",false);
	addRequiredResult("done");
}

QSharedPointer<Asset> PausePoint::Create()
{
	return QSharedPointer<Asset>(new PausePoint());
}

QString PausePoint::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();
	int command = engine->getEngineCommand();
	//Pause the engine if force pause is enabled
	if( (command != Engine::PictoEngine::StopEngine) 
		&& propertyContainer_->getPropertyValue("ForcePause").toBool())
	{
		//We don't directly pause the engine because then whoever is controlling
		//the engine (componentStatusManager, TestViewer, etc) won't know about it.
		//Instead we use requestPause() to ask the master to pause the engine for us.
		engine->requestPause();

		command = engine->getEngineCommand();
	}
	//If engine is paused, suspend execution until play is pressed
	bool ranPausingScript = false;
	while(command == Engine::PictoEngine::PauseEngine)
	{
		if(!ranPausingScript 
			&& !propertyContainer_->getPropertyValue("PausingScript").toString().isEmpty())
		{
			runScript(getName().simplified().remove(' ')+"Pausing");
			ranPausingScript = true;
		}
		//Show frame
		scene_->render(engine,getAssetId());
		QCoreApplication::processEvents();
		command = engine->getEngineCommand();
	}
	//If it was a stop that got us out of the pause, return an EngineAbort.
	if(command == Engine::PictoEngine::StopEngine)
		return "EngineAbort";
	if(ranPausingScript
		&& !propertyContainer_->getPropertyValue("RestartingScript").toString().isEmpty())
	{	//We must be restarting and there's a restarting script.  Run it.
		runScript(getName().simplified().remove(' ')+"Restarting");
	}
	return "done";
}

QString PausePoint::runAsSlave(QSharedPointer<Engine::PictoEngine> engine)
{
	//Add the cursor to the scene if it isn't there already
	if(!hasCursor_)
	{
		sigChannel_ = engine->getSignalChannel("PositionChannel");
		QSharedPointer<CursorGraphic> cursor(new CursorGraphic(sigChannel_, QColor(255,50,0,255)));
		cursor->setLayer(100000);
		scene_->addVisualElement(cursor);
		hasCursor_ = true;
	}

	QString result;// = run(engine);
	QString masterResult;
	while(true)
	{
		//--------- Check for master state change ------------
		engine->updateCurrentStateFromServer();
		masterResult = engine->getServerPathUpdate();
		if(!masterResult.isEmpty())
			break;
		//----------  Draw the scene --------------
		scene_->render(engine,getAssetId());
		
		//---------   Erase the latest cursor values (This happens in master when data is sent to server)
		sigChannel_->getValues();

		//As long as we're paused, lets process events
		QCoreApplication::processEvents();

		//Check if the engine stopeed
		if(engine->getEngineCommand() == Engine::PictoEngine::StopEngine)
			return "EngineAbort";

	}
	result = masterResult;
	return result; 
}

void PausePoint::postDeserialize()
{
	StateMachineElement::postDeserialize();
	setPropertyRuntimeEditable("ForcePause");
}

bool PausePoint::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;

	if(propertyContainer_->getPropertyValue("Name").toString() == "EngineAbort")
	{
		addError("PausePoint", "EngineAbort is a resticted keyword, and may not be used as the name of a PausePoint", xmlStreamReader);
		return false;
	}

	return true;
}

bool PausePoint::hasScripts()
{
	return (!propertyContainer_->getPropertyValue("PausingScript").toString().isEmpty()
	|| !propertyContainer_->getPropertyValue("RestartingScript").toString().isEmpty());
}
QMap<QString,QString> PausePoint::getScripts()
{
	QMap<QString,QString> scripts;
	if(!hasScripts())
		return scripts;

	if(!propertyContainer_->getPropertyValue("PausingScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"Pausing";
		scripts[scriptName] = propertyContainer_->getPropertyValue("PausingScript").toString();
	}
	if(!propertyContainer_->getPropertyValue("RestartingScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"Restarting";
		scripts[scriptName] = propertyContainer_->getPropertyValue("RestartingScript").toString();
	}
	return scripts;
}

void PausePoint::scriptableContainerWasReinitialized()
{
	scene_ = QSharedPointer<Scene>(new Scene);
	hasCursor_ = false;
	QList<QWeakPointer<Scriptable>> scriptables = getScriptableList();
	foreach(QWeakPointer<Scriptable> scriptable,scriptables)
	{
		if(scriptable.isNull())
			continue;
		if(scriptable.toStrongRef().dynamicCast<VisualElement>())
		{
			scene_->addVisualElement(scriptable.toStrongRef().staticCast<VisualElement>());
		}
	}
	QColor backgroundColor;
	backgroundColor.setNamedColor(propertyContainer_->getPropertyValue("BackgroundColor").toString());
	scene_->setBackgroundColor(QColor(propertyContainer_->getPropertyValue("BackgroundColor").toString()));
}

}//namespace Picto