#include "OKResponseHandler.h"
using namespace Picto;

OKResponseHandler::OKResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool OKResponseHandler::processResponse(QString directive)
{
	return true;
}