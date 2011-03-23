#include "PauseResponseHandler.h"
using namespace Picto;

PauseResponseHandler::PauseResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool PauseResponseHandler::processResponse(QString directive)
{return true;}