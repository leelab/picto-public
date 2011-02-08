#include "StartsessionCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"
#include "../connections/ServerConfig.h"

#include <QXmlStreamWriter>
#include <QUuid>

StartsessionCommandHandler::StartsessionCommandHandler()
{
}

/*! \brief handles a STARTSESSION command
 *
 */
QSharedPointer<Picto::ProtocolResponse> StartsessionCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	printf("STARTSESSION handler: %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
	QSharedPointer<Picto::ProtocolResponse> unauthorizedResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::Unauthorized));

	ConnectionManager *conMgr = ConnectionManager::Instance();
	
	QString target = command->getTarget();
	QString directorID;
	int proxyId;
	QByteArray experimentXml;
	QUuid observerId;

	directorID = target.left(target.indexOf('/'));
	proxyId = target.mid(target.indexOf('/')+1).toInt();
	experimentXml = command->getContent();
	observerId = QUuid(command->getFieldValue("Observer-ID"));
	
	if(observerId.isNull())
		return notFoundResponse;


	//Check that the Director is ready to go
	if(conMgr->getDirectorStatus(directorID) == DirectorStatus::notFound)
	{
		notFoundResponse->setContent("Director ID not found");
		return notFoundResponse;
	}
	else if(conMgr->getDirectorStatus(directorID) > DirectorStatus::idle)
	{
		return unauthorizedResponse;
	}

	//Check that the proxy server id is valid
	ServerConfig config;
	if(!config.proxyIdIsValid(proxyId))
	{
		notFoundResponse->setContent("Proxy ID not recognized");
		return notFoundResponse;
	}

	//create the session
	QSharedPointer<SessionInfo> sessionInfo;
	sessionInfo = ConnectionManager::Instance()->createSession(QUuid(directorID), proxyId, experimentXml, observerId);
	
	if(sessionInfo.isNull())
	{
		notFoundResponse->setContent("Failed to create SessionInfo object");
		return notFoundResponse;
	}

	QString pendingDirective = "LOADEXP\n" + experimentXml;
	sessionInfo->addPendingDirective(pendingDirective);

	//wait for the director to change to stopped status
	while(conMgr->getDirectorStatus(directorID) == DirectorStatus::idle)
	{
		QThread::yieldCurrentThread();
		QCoreApplication::processEvents();
	}

	QByteArray sessionIDXml;
	QXmlStreamWriter xmlWriter(&sessionIDXml);
	xmlWriter.writeStartElement("SessionID");
	xmlWriter.writeCharacters(sessionInfo->sessionId().toString());
	xmlWriter.writeEndElement();

	okResponse->setContent(sessionIDXml);

	return okResponse;
	

}
