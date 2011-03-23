#include "DirectorNewSessionResponseHandler.h"
using namespace Picto;

DirectorNewSessionResponseHandler::DirectorNewSessionResponseHandler(QSharedPointer<DirectorStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel):
NewSessionResponseHandler(statusManager,commandChannel)
{}

bool DirectorNewSessionResponseHandler::processResponse(QString directive)
{
	NewSessionResponseHandler::processResponse(directive);	
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.staticCast<DirectorStatusManager>()->getEngine();
	engine->setSessionId(commandChannel_->getSessionId());
	statusManager_.staticCast<DirectorStatusManager>()->updateSplashStatus("New Session, Session ID: " + engine->getSessionId().toString());
	return true;
}