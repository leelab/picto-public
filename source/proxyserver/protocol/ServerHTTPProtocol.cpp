#include "ServerHTTPProtocol.h"

#include "../../common/common.h"

#include "HTTPGetCommandHandler.h"

ServerHTTPProtocol::ServerHTTPProtocol()
{
	QSharedPointer<HTTPGetCommandHandler> getCommandHandler(new HTTPGetCommandHandler());

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
