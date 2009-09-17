#include "ServerHTTPProtocol.h"

#include "../../common/common.h"

#include "HTTPGetCommandHandler.h"

ServerHTTPProtocol::ServerHTTPProtocol(QObject *acqPlugin)
{
	QSharedPointer<HTTPGetCommandHandler> getCommandHandler(new HTTPGetCommandHandler(acqPlugin));

	addCommandHandler(getCommandHandler);
}

QString ServerHTTPProtocol::id()
{
	return QString("HTTP");
}

QString ServerHTTPProtocol::version()
{
	return QString("1.1");
}
