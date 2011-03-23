#include "DirectorEndSessionResponseHandler.h"
using namespace Picto;

DirectorEndSessionResponseHandler::DirectorEndSessionResponseHandler(QSharedPointer<DirectorStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel):
EndSessionResponseHandler(statusManager,commandChannel)
{}

bool DirectorEndSessionResponseHandler::processResponse(QString directive)
{
	if(!EndSessionResponseHandler::processResponse(directive))
		return false;
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.staticCast<DirectorStatusManager>()->getEngine();
	engine->setSessionId(QUuid());
	statusManager_.staticCast<DirectorStatusManager>()->updateSplashStatus("Session Ended");
	return true;
}