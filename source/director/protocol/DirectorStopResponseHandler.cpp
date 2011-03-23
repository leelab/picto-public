#include "DirectorStopResponseHandler.h"
using namespace Picto;

DirectorStopResponseHandler::DirectorStopResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
StopResponseHandler(statusManager)
{}

bool DirectorStopResponseHandler::processResponse(QString directive)
{
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.staticCast<DirectorStatusManager>()->getEngine();
	if(engine.isNull())
		return true;
	engine->stop();
	statusManager_.staticCast<DirectorStatusManager>()->updateSplashStatus("Engine Stopped");
	return false;	// status will be set to stopped by START handler when it exits and engine actually stops
}