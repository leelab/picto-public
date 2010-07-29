#include "JoinsessionCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"
#include "../connections/ServerConfig.h"

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
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
	QSharedPointer<Picto::ProtocolResponse> unauthorizedResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::Unauthorized));


	ConnectionManager *conMgr = ConnectionManager::Instance();
	
	QString target = command->getTarget();
	QString directorAddr;

	directorAddr = target.left(target.indexOf('/'));

	//Check that the Director is ready to go
	if(conMgr->getDirectorStatus(directorAddr) == DirectorStatus::notFound)
	{
		notFoundResponse->setContent("Director address not found");
		return notFoundResponse;
	}
	else if(conMgr->getDirectorStatus(directorAddr) <= DirectorStatus::idle)
	{
		return unauthorizedResponse;
	}

	//Find the session ID
	QUuid sessionId;
	sessionId = ConnectionManager::Instance()->getSessionInfo(directorAddr)->sessionId();

	//Find the experiment's XML
	QByteArray experimentXml;
	experimentXml = ConnectionManager::Instance()->getSessionInfo(directorAddr)->experimentXml();

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
