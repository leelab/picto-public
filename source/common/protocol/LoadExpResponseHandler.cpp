#include "LoadExpResponseHandler.h"
using namespace Picto;

LoadExpResponseHandler::LoadExpResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool LoadExpResponseHandler::processResponse(QString directive)
{return true;}