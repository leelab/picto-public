#include "EndSessionResponseHandler.h"
#include <QUuid>
#include "../memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a new EndSessionResponseHandler.
 *	\details statusManager is passed to the ProtocolResponseHandler().  commandChannel is used in the processResponse() function to allow
 *	all remaining Session data to be sent to the Server before the Session ends.
 */
EndSessionResponseHandler::EndSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel):
ProtocolResponseHandler(statusManager),
commandChannel_(commandChannel)
{
	Q_ASSERT(!commandChannel_.isNull());
}

/*! \brief Implements ProtocolCommandHandler::processCommand() to take care of ending the current session.
 *	\details This function sets the ComponentStatusManager status to "ending", then just calls CommandChannel::processResponses() over and over until
 *	that function finishes sending all the data that it needs to send to the Server with verification that all of the data was recieved and saved.  
 *	Once that is all done, the function returns and the Session is over.
 */
bool EndSessionResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	Q_ASSERT(!commandChannel_.isNull());
	statusManager_.toStrongRef()->setStatus(ending);
	while(!commandChannel_.toStrongRef()->processResponses(5000));
	commandChannel_.toStrongRef()->setSessionId(QUuid());
	return true;
}