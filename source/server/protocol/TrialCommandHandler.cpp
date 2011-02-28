#include "TrialCommandHandler.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

#include "../../common/globals.h"

#include <QXmlStreamReader>
#include <QFile>
#include <QUuid>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

TrialCommandHandler::TrialCommandHandler()
{
}

/*! \brief handles a TRIAL command
 *
 *	The TRIAL command is sent by a Director instance to inform us that a trial has 
 *	started or stopped.
 */
QSharedPointer<Picto::ProtocolResponse> TrialCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	printf("TRIAL command handler\n");
	//If there's an unrecognized target, then something has gone horribly wrong
	if(command->getTarget() != "/start" && command->getTarget() != "/end")
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
		return response;
	}

	//Extract the content
	QByteArray content = command->getContent();
	QXmlStreamReader xmlReader(content);
	QHostAddress sourceAddr(command->getFieldValue("Source-Address"));
	QUuid sourceID(command->getFieldValue("Source-ID"));
	QString sourceType(command->getFieldValue("Source-Type"));
	QUuid sessionID(command->getFieldValue("Session-ID"));

	double time = -1.0;
	int eventCode = -1;
	int trialNum = -1;

	while(!xmlReader.atEnd())
	{
		QString xmlName = xmlReader.name().toString();

		if(!xmlReader.isStartElement())
		{
			xmlReader.readNext();
			continue;
		}

		if(xmlReader.name() == "Time")
		{
			time = xmlReader.readElementText().toDouble();
		}
		else if(xmlReader.name() == "EventCode")
		{
			eventCode = xmlReader.readElementText().toInt();
		}
		else if(xmlReader.name() == "TrialNum")
		{
			trialNum = xmlReader.readElementText().toInt();
		}
		xmlReader.readNext();
	}

	if(xmlReader.hasError())
	{
		QString adf = xmlReader.errorString();
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
		return response;
	}


	if(time < 0 || eventCode < 0 || trialNum <0)
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
		return response;
	}

	//Get the current session info from a session manager
	QSharedPointer<SessionInfo> sessionInfo;
	sessionInfo = ConnectionManager::Instance()->getSessionInfo(sessionID);

	if(sessionInfo.isNull())
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
		response->setContent("SessionID not recognized");
		return response;
	}
	
	//Update the component
	ConnectionManager::Instance()->updateComponent(sourceID,sourceAddr,sessionID, sessionInfo->getComponentByType(sourceType)->getName(),sourceType,ComponentStatus::running);

	//Now that we've got the sessionInfo, we need to add the trial to the database
	sessionInfo->insertTrialEvent(time, eventCode, trialNum, sourceType);

	//Flush the database cache
	sessionInfo->flushCache();

	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QString directive = sessionInfo->pendingDirective(sourceID);
	if(directive.isEmpty())
		response->setContent("OK");
	else
		response->setContent(directive.toUtf8());

	return response;
}

