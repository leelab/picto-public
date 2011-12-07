#include "EndSessionResponseHandler.h"
#include <QUuid>
#include "../memleakdetect.h"
using namespace Picto;

EndSessionResponseHandler::EndSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel):
ProtocolResponseHandler(statusManager),
commandChannel_(commandChannel)
{
	Q_ASSERT(!commandChannel_.isNull());
}

bool EndSessionResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	Q_ASSERT(!commandChannel_.isNull());
	statusManager_.toStrongRef()->setStatus(ending);
	while(!commandChannel_.toStrongRef()->processResponses(5000));
	commandChannel_.toStrongRef()->setSessionId(QUuid());
	return true;
}