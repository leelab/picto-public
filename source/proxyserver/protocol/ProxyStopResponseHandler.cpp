#include "ProxyStopResponseHandler.h"
using namespace Picto;

ProxyStopResponseHandler::ProxyStopResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
StopResponseHandler(statusManager)
{}

bool ProxyStopResponseHandler::processResponse(QString directive)
{
	return true;
}