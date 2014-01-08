#include "DirectorEndSessionResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorEndSessionResponseHandler::DirectorEndSessionResponseHandler(QSharedPointer<DirectorStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel):
EndSessionResponseHandler(statusManager,commandChannel)
{}

/*! \brief Extends EndSessionResponseHandler::processCommand() to clear the SessionId of the previous session from
 *	the StatusManager and right "Session Ended" to DirectorStatusManager::setUserInfo().
 */
bool DirectorEndSessionResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	if(!EndSessionResponseHandler::processResponse(directive))
		return false;
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	engine->setSessionId(QUuid());
	statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setUserInfo("Session Ended");
	return true;
}