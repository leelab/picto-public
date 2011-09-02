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
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
	notFoundResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> unauthResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::Unauthorized));
	unauthResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
	badReqResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	
	QString targetSession = command->getTarget();

	QUuid observerId = QUuid(command->getFieldValue("Observer-ID"));

	//We don't keep any pointers to session info around because this keeps it from destructing when it times out.
	if(ConnectionManager::Instance()->getSessionInfo(QUuid(targetSession)).isNull())
		return notFoundResponse;

	if(!ConnectionManager::Instance()->getSessionInfo(QUuid(targetSession))->isAuthorizedObserver(observerId))
	{
		return unauthResponse;
	}

	//Add pending EndSession directives to each of the components.  The sessions will end when they timeout.
	QSharedPointer<SessionInfo> sessionInfo = ConnectionManager::Instance()->getSessionInfo(QUuid(targetSession));
	if(!sessionInfo.isNull())
	{
		QSharedPointer<ComponentInfo> component = sessionInfo->getComponentByType("DIRECTOR");
		if(!component.isNull() 
			&& (component->getSessionID() == QUuid(targetSession))
			&& (component->getStatus() > ComponentStatus::ending)) 
		{
			//First send the director ENDSESSION and stop when it starts ending
			sessionInfo->addPendingDirective("ENDSESSION","DIRECTOR");
		}

		component = sessionInfo->getComponentByType("PROXY");
		if(!component.isNull() 
		&& (component->getSessionID() == QUuid(targetSession))
		&& (component->getStatus() > ComponentStatus::ending)) 
		{
			//Next send the proxy ENDSESSION until it starts ending
			sessionInfo->addPendingDirective("ENDSESSION","PROXY");
		}
	}
	else
	{
		notFoundResponse->setContent("Requested Session is not active.");
		return notFoundResponse;
	}
	return okResponse;
}
