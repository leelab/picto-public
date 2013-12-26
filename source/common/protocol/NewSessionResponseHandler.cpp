#include <QUuid>
#include "NewSessionResponseHandler.h"
#include "..\storage\DataUnit.h"
#include "..\timing\Timestamper.h"
#include "../memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a new NewSessionResponseHandler.
 *	\details statusManager is passed to the ProtocolResponseHandler().  commandChannel is used in the processResponse() function so that
 *	the new SessionId can be set to it.
 */
NewSessionResponseHandler::NewSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel):
ProtocolResponseHandler(statusManager),
commandChannel_(commandChannel)
{
	Q_ASSERT(!commandChannel_.isNull());
}

/*! \brief Implements ProtocolCommandHandler::processCommand() to take care of starting a new session.
 *	\details This function takes care of initializing various variables for the beginning of a new session.
 *	It makes use of the input directive to get the SessionId of the new session.
 */
bool NewSessionResponseHandler::processResponse(QString directive)
{
	QUuid sessionID(directive);
	Q_ASSERT(!commandChannel_.isNull());
	commandChannel_.toStrongRef()->setSessionId(sessionID);
	DataUnit::resetDataIDGenerator();	//Restart DataIds from 1
	Timestamper::reset();				//Restart Timestamps from time 0.
	return true;
}