#include "ProxyPauseResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

ProxyPauseResponseHandler::ProxyPauseResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
PauseResponseHandler(statusManager)
{}

bool ProxyPauseResponseHandler::processResponse(QString directive)
{
	PauseResponseHandler::processResponse(directive);
	return true;
}