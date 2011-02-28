#include "ComponentUpdateCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

#include <QFile>
#include <QUuid>

ComponentUpdateCommandHandler::ComponentUpdateCommandHandler()
{
}

/*! \brief handles a COMPONENTUPDATE command
 *
 */
QSharedPointer<Picto::ProtocolResponse> ComponentUpdateCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//printf("COMPONENTUPDATE handler: %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));

	//Update the DirectorList
	QHostAddress sourceAddr(command->getFieldValue("Source-Address"));
	QUuid sourceID(command->getFieldValue("Source-ID"));
	QString sourceType(command->getFieldValue("Source-Type"));
	QUuid sessionId(command->getFieldValue("Session-ID"));
	
	ComponentStatus::ComponentStatus status;

	QString statusStr;
	QString name;
	QString targetStr;

	targetStr = command->getTarget();
	name = targetStr.left(targetStr.indexOf(':'));
	statusStr = targetStr.right(targetStr.length() - targetStr.indexOf(':') - 1);

	if(statusStr.toUpper() == "IDLE")
	{
		status = ComponentStatus::idle;
	}
	else if(statusStr.toUpper() == "STOPPED")
	{
		status = ComponentStatus::stopped;
	}
	else if(statusStr.toUpper() == "PAUSED")
	{
		status = ComponentStatus::paused;
	}
	else if(statusStr.toUpper() == "RUNNING")
	{
		status = ComponentStatus::running;
	}


	ConnectionManager *conMgr = ConnectionManager::Instance();
	conMgr->updateComponent(sourceID, sourceAddr, sessionId, name, sourceType, status);

	//If we're in a session, check for pending directives
	QString test = sessionId.toString();
	if(!sessionId.isNull())
	{
		QSharedPointer<SessionInfo> sessionInfo;
		sessionInfo = ConnectionManager::Instance()->getSessionInfo(sessionId);

		if(sessionInfo.isNull())
		{
			response->setContent("ERROR: SessionInfo not found.");
			QString errorMsg = QString("Couldn't find session: %1\n").arg(sessionId);
			printf(errorMsg.toAscii());
		}
		else
		{
			//Indicate that there was activity on this session.
			sessionInfo->setActivity();

			QString directive = sessionInfo->pendingDirective(sourceID);
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
		QUuid sessionId = ConnectionManager::Instance()->pendingSession(sourceID);

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

