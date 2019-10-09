#include "ProxyStatusManager.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a ProxyStatusManager.
 *	@param commandChannel The CommandChannel that will be used to send data and COMPONENTUPDATE "keep alive"
 *	commands to the PictoServer.
 */
ProxyStatusManager::ProxyStatusManager(QSharedPointer<CommandChannel> commandChannel) :
ComponentStatusManager(),
commandChannel_(commandChannel),
acqPlugin_(NULL),
sessionDBPath_(""),
sessionDBName_("")
{}
/*! \brief Implements ComponentStatusManager::newSession() to... do nothing.
*/
void ProxyStatusManager::newSession()
{
	qDebug(QString("new session=%1").arg(getSessionID().toString()).toLatin1());
}
/*! Overrides setUserInfo to accept the session DB path from the Picto server and
    make the name part of the path available to getSessionDBName()
*/
void ProxyStatusManager::setUserInfo(QString info)
{
	sessionDBPath_ = info.replace("\\", "/");
	Q_ASSERT(sessionDBPath_.lastIndexOf("/") >= 0);
	Q_ASSERT(sessionDBPath_.lastIndexOf(".") >= 0);
	sessionDBName_ = sessionDBPath_.mid(sessionDBPath_.lastIndexOf("/") + 1);
	sessionDBName_ = sessionDBName_.left(sessionDBName_.lastIndexOf("."));
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