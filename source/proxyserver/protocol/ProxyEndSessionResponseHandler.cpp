#include "ProxyEndSessionResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

ProxyEndSessionResponseHandler::ProxyEndSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<Picto::CommandChannel> commandChannel):
EndSessionResponseHandler(statusManager,commandChannel)
{}

bool ProxyEndSessionResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	if(!EndSessionResponseHandler::processResponse(directive))
		return false;
	return true;
}