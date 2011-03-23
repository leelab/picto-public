#include "ProxyResumeResponseHandler.h"
using namespace Picto;

ProxyResumeResponseHandler::ProxyResumeResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ResumeResponseHandler(statusManager)
{}

bool ProxyResumeResponseHandler::processResponse(QString directive)
{return true;}