#include "ComponentUpdateCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

#include <QFile>
#include <QUuid>
#include "../../common/memleakdetect.h"

ComponentUpdateCommandHandler::ComponentUpdateCommandHandler()
{
}

/*! \brief Parses the input ProtocolCommand to check its syntax, extract the included Component data and use it to update the ConnectionManager
 *	and a Session that it is being used in.  Returns any pending directives for the calling Component, or  a simple OK response otherwise.
 *	\note This function takes part in making sure the correct registered command responses are sent to Components to let them know which
 *	of their commands have been processed and their data saved to disk.  See Picto::RegisteredResponseType for more details.
 */
QSharedPointer<Picto::ProtocolResponse> ComponentUpdateCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//qDebug((QString("COMPONENTUPDATE handler: %1 %2").arg(command->getFieldValue("Source-ID")).arg(command->getFieldValue("Command-ID"))).toLatin1());

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
	else if(statusStr.toUpper() == "ENDING")
	{
		status = ComponentStatus::ending;
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
	QString details = QString(command->getContent());

	ConnectionManager *conMgr = ConnectionManager::Instance();
	conMgr->updateComponent(sourceID, sourceAddr, sessionId, name, sourceType, status,details);

	//If we're in a session, check for pending directives
	QString test = sessionId.toString();
	if(!sessionId.isNull())
	{
		QSharedPointer<SessionInfo> sessionInfo;
		sessionInfo = ConnectionManager::Instance()->getSessionInfo(sessionId);

		if(sessionInfo.isNull())
		{
			response->setContent("ERROR\nSessionInfo not found.");
			QString errorMsg = QString("Couldn't find session: %1\n").arg(sessionId.toString());
			printf(errorMsg.toLatin1());
		}
		else
		{
			//Indicate that there was activity on this session.
			sessionInfo->setActivity();

			bool lastDataPrecededFlush = sessionInfo->lastDataPrecededFlush(sourceType);
			bool lastDataFollowedFlush = sessionInfo->lastDataFollowedFlush(sourceType);
			sessionInfo->markLastDataTime(sourceType);
			bool currDataPrecededFlush = sessionInfo->lastDataPrecededFlush(sourceType);
			bool currDataFollowedFlush = sessionInfo->lastDataFollowedFlush(sourceType);
			int regType = 0;
			if(lastDataPrecededFlush && !currDataPrecededFlush)
			{
				regType |= Picto::RegisteredResponseType::FirstInCommandPackage;
			}
			if(!lastDataFollowedFlush && currDataFollowedFlush)
			{
				regType |= Picto::RegisteredResponseType::SendLastCommandPackage;
			}
			response->setRegisteredType(Picto::RegisteredResponseType::RegisteredResponseType(regType));

			QString directive = sessionInfo->pendingDirective(sourceID);
			if(directive.isEmpty())
			{
				response->setContent("OK");
				//if(sessionInfo->needsFlush(sourceType))
				//{
				//	sessionInfo->flushCache(sourceType);
				//	response->setRegisteredType(Picto::RegisteredResponseType::Immediate);
				//}
			}
			else
			{
				//The server crashed when too big of a string was sent into this qDebug call!!! Watch out for this!
				//qDebug(QString("Sent %1 Directive to %2").arg(directive).arg(sourceType).toLatin1());
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

