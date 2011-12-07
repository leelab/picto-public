#include "RewardResponseHandler.h"
#include "../memleakdetect.h"
using namespace Picto;

RewardResponseHandler::RewardResponseHandler(QSharedPointer<ComponentStatusManager> statusManager):
ProtocolResponseHandler(statusManager)
{}

bool RewardResponseHandler::processResponse(QString directive)
{return true;}