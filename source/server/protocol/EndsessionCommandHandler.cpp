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
	
	QString targetSession = command->getTarget();

	QSharedPointer<SessionInfo> sessionInfo = ConnectionManager::Instance()->getSessionInfo(QUuid(targetSession));
	if(sessionInfo.isNull())
	{
		return notFoundResponse;
	}
	else
	{
		ConnectionManager::Instance()->endSession(QUuid(targetSession));
		return okResponse;
	}

}
