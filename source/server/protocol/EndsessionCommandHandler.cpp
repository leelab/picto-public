#include "EndsessionCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

EndsessionCommandHandler::EndsessionCommandHandler()
{
}

/*! \brief handles an ENDSESSION command
 *
 */
QSharedPointer<Picto::ProtocolResponse> EndsessionCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	printf("ENDSESSION handler: %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
	QSharedPointer<Picto::ProtocolResponse> unauthResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::Unauthorized));
	
	QString targetSession = command->getTarget();

	QUuid observerId = QUuid(command->getFieldValue("Observer-ID"));

	QSharedPointer<SessionInfo> sessionInfo = ConnectionManager::Instance()->getSessionInfo(QUuid(targetSession));
	if(sessionInfo.isNull())
		return notFoundResponse;

	if(sessionInfo->isAuthorizedObserver(observerId))
	{
		ConnectionManager::Instance()->endSession(QUuid(targetSession));
		return okResponse;
	}
	else
		return unauthResponse;

}
