#include "NewSessionResponseHandler.h"
#include <QUuid>
using namespace Picto;

NewSessionResponseHandler::NewSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel):
ProtocolResponseHandler(statusManager),
commandChannel_(commandChannel)
{
	Q_ASSERT(!commandChannel_.isNull());
}

bool NewSessionResponseHandler::processResponse(QString directive)
{
	QUuid sessionID(directive);
	commandChannel_->setSessionId(sessionID);
	return true;
}