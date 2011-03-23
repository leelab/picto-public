#include "ResumeResponseHandler.h"
using namespace Picto;

ResumeResponseHandler::ResumeResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool ResumeResponseHandler::processResponse(QString directive)
{return true;}