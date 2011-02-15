#include "StartsessionCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

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
	QString proxyID;
	QByteArray experimentXml;
	QUuid observerId;

	directorID = target.left(target.indexOf('/'));
	proxyID = target.right(target.indexOf('/'));
	experimentXml = command->getContent();
	observerId = QUuid(command->getFieldValue("Observer-ID"));
	
	if(observerId.isNull())
		return notFoundResponse;


	//Check that the Director is ready to go
	if(conMgr->getComponentStatus(directorID) == ComponentStatus::notFound)
	{
		notFoundResponse->setContent("Director ID not found");
		return notFoundResponse;
	}
	else if(conMgr->getComponentStatus(directorID) > ComponentStatus::idle)
	{
		return unauthorizedResponse;
	}

	//Check that the Proxy is ready to go
	if(proxyID == QUuid().toString())
	{
		//No Proxy in this experiment
	}
	else if(conMgr->getComponentStatus(proxyID) == ComponentStatus::notFound)
	{
		notFoundResponse->setContent("proxy ID not found");
		return notFoundResponse;
	}
	else if(conMgr->getComponentStatus(proxyID) > ComponentStatus::idle)
	{
		return unauthorizedResponse;
	}

	//create the session
	QSharedPointer<SessionInfo> sessionInfo;
	sessionInfo = ConnectionManager::Instance()->createSession(QUuid(directorID), QUuid(proxyID), experimentXml, observerId);
	
	if(sessionInfo.isNull())
	{
		notFoundResponse->setContent("Failed to create SessionInfo object");
		return notFoundResponse;
	}

	QString pendingDirective = "LOADEXP\n" + experimentXml;
	sessionInfo->addPendingDirective(pendingDirective,"DIRECTOR");

	//wait for the director to change to stopped status
	while(conMgr->getComponentStatus(directorID) == ComponentStatus::idle)
	{
		QThread::yieldCurrentThread();
		QCoreApplication::processEvents();
	}
	//wait for the proxy to change to stopped status if we're using a proxy.
	if(proxyID != QUuid().toString())
	{
		while(conMgr->getComponentStatus(directorID) == ComponentStatus::idle)
		{
			QThread::yieldCurrentThread();
			QCoreApplication::processEvents();
		}
	}

	QByteArray sessionIDXml;
	QXmlStreamWriter xmlWriter(&sessionIDXml);
	xmlWriter.writeStartElement("SessionID");
	xmlWriter.writeCharacters(sessionInfo->sessionId().toString());
	xmlWriter.writeEndElement();

	okResponse->setContent(sessionIDXml);

	return okResponse;
	

}
