#include "RewardResponseHandler.h"
using namespace Picto;

RewardResponseHandler::RewardResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool RewardResponseHandler::processResponse(QString directive)
{return true;}