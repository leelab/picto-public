#include "StartsessionCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

#include <QXmlStreamWriter>

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
	QString directorAddr;
	QString proxyAddr;

	directorAddr = target.left(target.indexOf('/'));
	proxyAddr = target.mid(target.indexOf('/')+1);

	if(conMgr->getDirectorStatus(directorAddr) == DirectorStatus::notFound)
	{
		return notFoundResponse;
	}
	else if(conMgr->getDirectorStatus(directorAddr) == DirectorStatus::running)
	{
		return unauthorizedResponse;
	}
	else
	{
		QSharedPointer<SessionInfo> sessionInfo;
		sessionInfo = ConnectionManager::Instance()->createSession(directorAddr);
		
		if(sessionInfo.isNull())
		{
			return notFoundResponse;
		}

		QString pendingDirective = "LOADEXP\n" + command->getContent();
		sessionInfo->addPendingDirective(pendingDirective);

		QByteArray sessionIDXml;
		QXmlStreamWriter xmlWriter(&sessionIDXml);
		xmlWriter.writeStartElement("SessionID");
		xmlWriter.writeCharacters(sessionInfo->sessionId().toString());
		xmlWriter.writeEndElement();

		okResponse->setContent(sessionIDXml);

		return okResponse;
	}

}
