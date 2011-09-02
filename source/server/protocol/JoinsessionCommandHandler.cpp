#include "JoinsessionCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

#include <QXmlStreamWriter>

JoinsessionCommandHandler::JoinsessionCommandHandler()
{
}

/*! \brief handles a STARTSESSION command
 *
 */
QSharedPointer<Picto::ProtocolResponse> JoinsessionCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	printf("JOINSESSION handler: %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
	notFoundResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> unauthorizedResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::Unauthorized));
	unauthorizedResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);

	ConnectionManager *conMgr = ConnectionManager::Instance();
	
	QString target = command->getTarget();
	QString directorID;

	directorID = target.left(target.indexOf('/'));

	//Check that the Director is ready to go
	if(conMgr->getComponentStatus(directorID) == ComponentStatus::notFound)
	{
		notFoundResponse->setContent("Director ID not found");
		return notFoundResponse;
	}
	else if(conMgr->getComponentStatus(directorID) < ComponentStatus::stopped)
	{
		unauthorizedResponse->setContent("Director is currently ending the session");
		return unauthorizedResponse;
	}

	//Find the session ID
	QSharedPointer<SessionInfo> sessInfo = ConnectionManager::Instance()->getSessionInfoByComponent(directorID);
	if(sessInfo.isNull())
		return notFoundResponse;
	QSharedPointer<ComponentInfo> proxy = sessInfo->getComponentByType("PROXY");
	if(!proxy.isNull())
	{
		QUuid proxyID = proxy->getUuid();
		if(conMgr->getComponentStatus(proxyID) == ComponentStatus::notFound)
		{
			notFoundResponse->setContent("Proxy for this sessionID not found");
			return notFoundResponse;
		}
		else if(conMgr->getComponentStatus(proxyID) < ComponentStatus::stopped)
		{
			unauthorizedResponse->setContent("Proxy is currently ending the session");
			return unauthorizedResponse;
		}
	}
	QUuid sessionId = sessInfo->sessionId();

	//Find the experiment's XML
	QByteArray experimentXml;
	experimentXml = sessInfo->experimentXml();

	//Write out the content
	//The XMLStreamWriter doesn't handle copying text well, so we'll use it first, and then
	//fill in the experiment XML later
	QByteArray joinSessionXml;
	QXmlStreamWriter xmlWriter(&joinSessionXml);
	xmlWriter.writeStartElement("JoinSession");
	xmlWriter.writeTextElement("SessionID",sessionId.toString());
	xmlWriter.writeEndElement();

	int sessionIndex;
	sessionIndex = joinSessionXml.lastIndexOf("</JoinSession>");
	int expIndex;
	expIndex = experimentXml.indexOf("<Experiment>");
	joinSessionXml.insert(sessionIndex,experimentXml.mid(expIndex));


	okResponse->setContent(joinSessionXml);

	return okResponse;
}
