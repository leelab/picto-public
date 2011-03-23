#include "DirectorPauseResponseHandler.h"
using namespace Picto;

DirectorPauseResponseHandler::DirectorPauseResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
PauseResponseHandler(statusManager)
{}

bool DirectorPauseResponseHandler::processResponse(QString directive)
{
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.staticCast<DirectorStatusManager>()->getEngine();
	if(engine.isNull())
		return true;
	engine->pause();
	return true;
}