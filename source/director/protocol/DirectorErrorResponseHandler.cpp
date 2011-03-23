#include "DirectorErrorResponseHandler.h"
using namespace Picto;

DirectorErrorResponseHandler::DirectorErrorResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
ErrorResponseHandler(statusManager)
{}

bool DirectorErrorResponseHandler::processResponse(QString directive)
{
	statusManager_.staticCast<DirectorStatusManager>()->updateSplashStatus("ERROR: "+directive);
	return true;
}