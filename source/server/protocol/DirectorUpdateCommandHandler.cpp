#include "DirectorUpdateCommandHandler.h"

#include "../../common/globals.h"
#include "../session/SessionManager.h"
#include "../session/SessionInfo.h"
#include "../network/DirectorList.h"


#include <QXmlStreamReader>
#include <QFile>

DirectorUpdateCommandHandler::DirectorUpdateCommandHandler()
{
}

/*! \brief handles a STATUS command
 *
 */
QSharedPointer<Picto::ProtocolResponse> DirectorUpdateCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));

	//Update the DirectorList
	QHostAddress sourceAddr(command->getFieldValue("Source-Address"));
	
	DirectorList::DirectorStatus status;
	if(command->getTarget() == "idle")
		status = DirectorList::idle;
	else if(command->getTarget() == "running")
		status = DirectorList::running;
	else
		return notFoundResponse;

	//! /TODO implement director naming
	QString name = sourceAddr.toString();

	DirectorList directorList;
	directorList.updateDirector(sourceAddr, name, status);

	QString blah = directorList.getList();

	//Get the session ID from the command
	QUuid sessionID = QUuid(command->getFieldValue("Session-ID"));

	//Get the current session info from a session manager
	SessionManager sessionMgr;
	QSharedPointer<SessionInfo> sessionInfo;
	sessionInfo = sessionMgr.getSessionInfo(sessionID);

	if(sessionInfo.isNull())
	{
		response->setContent("OK");
		return response;
	}



	QString pendingDirective = sessionInfo->pendingDirective();
	if(pendingDirective.isEmpty())
	{
		response->setContent("OK");
	}
	else
	{
		response->setContent(pendingDirective.toUtf8());
	}

	return response;

}

