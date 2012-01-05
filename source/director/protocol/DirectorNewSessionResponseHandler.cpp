#include "DirectorNewSessionResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorNewSessionResponseHandler::DirectorNewSessionResponseHandler(QSharedPointer<DirectorStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel):
NewSessionResponseHandler(statusManager,commandChannel)
{}

bool DirectorNewSessionResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!commandChannel_.isNull());
	Q_ASSERT(!statusManager_.isNull());
	NewSessionResponseHandler::processResponse(directive);	
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	engine->setSessionId(commandChannel_.toStrongRef()->getSessionId());
	statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setUserInfo("New Session, Session ID: " + engine->getSessionId().toString());
	return true;
}