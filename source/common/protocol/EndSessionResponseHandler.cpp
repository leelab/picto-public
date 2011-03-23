#include "EndSessionResponseHandler.h"
#include <QUuid>
using namespace Picto;

EndSessionResponseHandler::EndSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel):
ProtocolResponseHandler(statusManager),
commandChannel_(commandChannel)
{
	Q_ASSERT(!commandChannel_.isNull());
}

bool EndSessionResponseHandler::processResponse(QString directive)
{
	statusManager_->setStatus(ending);
	while(!commandChannel_->processResponses(0));
	commandChannel_->setSessionId(QUuid());
	return true;
}