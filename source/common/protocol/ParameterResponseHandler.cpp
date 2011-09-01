#include "ParameterResponseHandler.h"
using namespace Picto;

ParameterResponseHandler::ParameterResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool ParameterResponseHandler::processResponse(QString directive)
{
	return true;
}