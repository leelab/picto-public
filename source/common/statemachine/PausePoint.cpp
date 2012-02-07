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
	visualElementFactory_->setMaxAssets(0);
	controlTargetFactory_->setMaxAssets(0);
	AddDefinableProperty("Type","PausePoint");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	addRequiredResult("done");
}

QSharedPointer<Asset> PausePoint::Create()
{
	return QSharedPointer<Asset>(new PausePoint());
}

QString PausePoint::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();

	//Suspend execution until play is pressed
	int command = engine->getEngineCommand();
	while(command == Engine::PictoEngine::PauseEngine)
	{
		//Show frame
		scene_->render(engine,getAssetId());
		QCoreApplication::processEvents();
		command = engine->getEngineCommand();
	}
	//If it was a stop that got us out of the pause, return an EngineAbort.
	if(command == Engine::PictoEngine::StopEngine)
		return "EngineAbort";
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

}//namespace Picto