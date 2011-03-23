#include "ProxyErrorResponseHandler.h"
using namespace Picto;

ProxyErrorResponseHandler::ProxyErrorResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ErrorResponseHandler(statusManager)
{}

bool ProxyErrorResponseHandler::processResponse(QString directive)
{return true;}