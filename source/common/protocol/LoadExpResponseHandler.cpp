#include "LoadExpResponseHandler.h"
#include "../memleakdetect.h"
using namespace Picto;

LoadExpResponseHandler::LoadExpResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool LoadExpResponseHandler::processResponse(QString directive)
{return true;}