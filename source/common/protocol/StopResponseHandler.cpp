#include "StopResponseHandler.h"
using namespace Picto;

StopResponseHandler::StopResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool StopResponseHandler::processResponse(QString directive)
{
	return true;
}