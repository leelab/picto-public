#include <QUuid>
#include "NewSessionResponseHandler.h"
#include "..\storage\DataUnit.h"
#include "..\timing\Timestamper.h"
#include "../memleakdetect.h"
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
	Q_ASSERT(!commandChannel_.isNull());
	commandChannel_.toStrongRef()->setSessionId(sessionID);
	DataUnit::resetDataIDGenerator();
	Timestamper::reset();
	return true;
}