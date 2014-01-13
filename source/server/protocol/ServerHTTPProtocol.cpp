#include "ServerHTTPProtocol.h"

#include "../../common/common.h"

#include "HTTPGetCommandHandler.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a ServerHTTPProtocol object which includes a single ProtocolCommandHandler,
 *	the HTTPGetCommandHandler that is used to handle HTTP get commands.
*/
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
