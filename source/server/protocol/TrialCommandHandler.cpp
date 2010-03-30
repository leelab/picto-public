#include "TrialCommandHandler.h"
#include "../session/SessionInfo.h"
#include "../session/SessionManager.h"

#include "../../common/globals.h"

#include <QXmlStreamReader>
#include <QFile>
#include <QUuid>
#include <QSqlDatabase>
#include <QSqlQuery>
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
	//If there's an unrecognized target, then something has gone horribly wrong
	if(command->getTarget() != "/start" && command->getTarget() != "/end")
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
		return response;
	}

	//Extract the content
	QByteArray content = command->getContent();
	QXmlStreamReader xmlReader(content);

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

	//Get the session ID from the command
	QUuid sessionID = QUuid(command->getFieldValue("Session-ID"));

	//Get the current session info from a session manager
	SessionManager sessionMgr;
	QSharedPointer<SessionInfo> sessionInfo;
	sessionInfo = sessionMgr.getSessionInfo(sessionID);

	if(sessionInfo.isNull())
	{
		QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
		response->setContent("SessionID not recognized");
		return response;
	}

	//Now that we've got the sessionInfo, we need to add the trial to the database
	QSqlQuery query(sessionInfo->sessionDb());

	query.prepare("INSERT INTO behavioraltrials (timestamp,aligncode,trialnumber,matched)"
		"VALUES(:timestamp, :aligncode, :trialnumber, 0)");
	query.bindValue(":timestamp", time);
	query.bindValue(":aligncode", eventCode);
	query.bindValue(":trialnumber", trialNum);
	query.exec();







	//! \todo Actually implement this stuff...
	//For the moment we are simply going to print out the start trial info...
	/*QFile outFile("TRIALevents.txt");
	outFile.open(QIODevice::Append);

	if(command->getTarget() == "/start")
	{
		outFile.write("Trial Start\n");
	}
	else if(command->getTarget() == "/end")
	{
		outFile.write("Trial End\n");
	}

	outFile.write(QString(" Time: %1\n").arg(time).toAscii());
	outFile.write(QString(" Trial number: %1\n").arg(trialNum).toAscii());
	outFile.write(QString(" Event code: %1\n\n").arg(eventCode).toAscii());

	outFile.close();*/

	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));

	return response;
}

