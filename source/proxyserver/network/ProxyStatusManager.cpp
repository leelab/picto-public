#include "ProxyStatusManager.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a ProxyStatusManager.
 *	@param commandChannel The CommandChannel that will be used to send data and COMPONENTUPDATE "keep alive"
 *	commands to the PictoServer.
 */
ProxyStatusManager::ProxyStatusManager(QSharedPointer<CommandChannel> commandChannel):
ComponentStatusManager(),
commandChannel_(commandChannel),
acqPlugin_(NULL)
{}
/*! \brief Implements ComponentStatusManager::newSession() to... do nothing.
*/
void ProxyStatusManager::newSession()
{
}
/*! \brief Implements ComponentStatusManager::doServerUpdate() to send a COMPONENTUPDATE command
 *	to the PictoServer.
 */
void ProxyStatusManager::doServerUpdate()
{
	Q_ASSERT(!commandChannel_.isNull());
	
	QSharedPointer<Picto::ProtocolCommand> updateCommand(new Picto::ProtocolCommand());
	updateCommand->setMethod("COMPONENTUPDATE");
	updateCommand->setProtocolName("PICTO");
	updateCommand->setProtocolVersion("1.0");
	commandChannel_->sendCommand(updateCommand);
	
}