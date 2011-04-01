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
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
	
	QString targetSession = command->getTarget();

	QUuid observerId = QUuid(command->getFieldValue("Observer-ID"));

	//We don't keep any pointers to session info around because this keeps it from destructing when it times out.
	if(ConnectionManager::Instance()->getSessionInfo(QUuid(targetSession)).isNull())
		return notFoundResponse;

	if(!ConnectionManager::Instance()->getSessionInfo(QUuid(targetSession))->isAuthorizedObserver(observerId))
	{
		return unauthResponse;
	}

	QTime timer;
	timer.start();
	while(ConnectionManager::Instance()->sessionIsValid(QUuid(targetSession)))
	{
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
			else
			{
				component = sessionInfo->getComponentByType("PROXY");
				if(!component.isNull() 
				&& (component->getSessionID() == QUuid(targetSession))
				&& (component->getStatus() > ComponentStatus::ending)) 
				{
					//Next send the proxy ENDSESSION until it starts ending
					sessionInfo->addPendingDirective("ENDSESSION","PROXY");
				}
				else
				{
					// Both components are ending.  Consider the command successful
					break;
				}
			}
			timer.start();
		}
		//Keep waiting until either both components start the ending process
		while(timer.elapsed() < 5000)
		{
			//Wait for it.
			QThread::yieldCurrentThread();
			QCoreApplication::processEvents();
		}
	}

	
	return okResponse;

}
