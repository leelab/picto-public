#include "ProxyStatusManager.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

ProxyStatusManager::ProxyStatusManager(QSharedPointer<CommandChannel> commandChannel):
ComponentStatusManager(),
commandChannel_(commandChannel),
acqPlugin_(NULL)
{}
void ProxyStatusManager::newSession()
{
}
void ProxyStatusManager::doServerUpdate()
{
	Q_ASSERT(!commandChannel_.isNull());
	
	QSharedPointer<Picto::ProtocolCommand> updateCommand(new Picto::ProtocolCommand());
	updateCommand->setMethod("COMPONENTUPDATE");
	updateCommand->setProtocolName("PICTO");
	updateCommand->setProtocolVersion("1.0");
	commandChannel_->sendCommand(updateCommand);
	
}