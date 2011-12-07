#include "DirectorEndSessionResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorEndSessionResponseHandler::DirectorEndSessionResponseHandler(QSharedPointer<DirectorStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel):
EndSessionResponseHandler(statusManager,commandChannel)
{}

bool DirectorEndSessionResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	if(!EndSessionResponseHandler::processResponse(directive))
		return false;
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	engine->setSessionId(QUuid());
	statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->updateSplashStatus("Session Ended");
	return true;
}