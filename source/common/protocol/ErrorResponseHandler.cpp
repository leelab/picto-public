#include "ErrorResponseHandler.h"
using namespace Picto;

ErrorResponseHandler::ErrorResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool ErrorResponseHandler::processResponse(QString directive)
{return true;}