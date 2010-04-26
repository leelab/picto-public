#include "DirectorUpdateCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

#include <QFile>

DirectorUpdateCommandHandler::DirectorUpdateCommandHandler()
{
}

/*! \brief handles a DIRECTORUPDATE command
 *
 */
QSharedPointer<Picto::ProtocolResponse> DirectorUpdateCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	printf("\nDIRECTORUPDATE  handler: %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));

	//Update the DirectorList
	QHostAddress sourceAddr(command->getFieldValue("Source-Address"));
	
	DirectorStatus::DirectorStatus status;

	QString statusStr;
	QString name;
	QString targetStr;

	targetStr = command->getTarget();
	name = targetStr.left(targetStr.indexOf(':'));
	statusStr = targetStr.right(targetStr.length() - targetStr.indexOf(':') - 1);
	if(statusStr == "idle")
	{
		status = DirectorStatus::idle;
	}
	else if(statusStr == "running")
	{
		status = DirectorStatus::running;
	}
	else
		return notFoundResponse;


	ConnectionManager *conMgr = ConnectionManager::Instance();
	conMgr->updateDirector(sourceAddr, name, status);

	//If we're in a session, check for pending directives
	if(command->hasField("Session-ID"))
	{
		QUuid sessionId = QUuid(command->getFieldValue("Session-ID"));
		QSharedPointer<SessionInfo> sessionInfo;
		sessionInfo = ConnectionManager::Instance()->getSessionInfo(sessionId);

		if(sessionInfo.isNull())
		{
			response->setContent("ERROR");
		}
		else
		{
			//Indicate that there was activity on this session.
			sessionInfo->setActivity();

			QString directive = sessionInfo->pendingDirective();
			if(directive.isEmpty())
			{
				response->setContent("OK");
			}
			else
			{
				response->setContent(directive.toUtf8());
			}
		}
	}
	else
	{
		QUuid sessionId = ConnectionManager::Instance()->pendingSession(sourceAddr);

		if(sessionId.isNull())
		{
			response->setContent("OK");
		}
		else
		{
			response->setContent("NEWSESSION "+sessionId.toString().toUtf8());
		}
	}

	return response;
}

