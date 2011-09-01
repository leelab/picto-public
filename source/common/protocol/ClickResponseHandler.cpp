#include "ClickResponseHandler.h"
using namespace Picto;

ClickResponseHandler::ClickResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool ClickResponseHandler::processResponse(QString directive)
{
	return true;
}