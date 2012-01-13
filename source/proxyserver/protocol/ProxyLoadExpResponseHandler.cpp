#include "ProxyLoadExpResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

ProxyLoadExpResponseHandler::ProxyLoadExpResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
LoadExpResponseHandler(statusManager)
{}

bool ProxyLoadExpResponseHandler::processResponse(QString directive)
{
	LoadExpResponseHandler::processResponse(directive);
	return true;
}