#include "StartResponseHandler.h"
#include "../memleakdetect.h"
using namespace Picto;

StartResponseHandler::StartResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool StartResponseHandler::processResponse(QString directive)
{return true;}