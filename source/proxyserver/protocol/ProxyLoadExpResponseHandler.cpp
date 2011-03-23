#include "ProxyLoadExpResponseHandler.h"
using namespace Picto;

ProxyLoadExpResponseHandler::ProxyLoadExpResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
LoadExpResponseHandler(statusManager)
{}

bool ProxyLoadExpResponseHandler::processResponse(QString directive)
{return true;}