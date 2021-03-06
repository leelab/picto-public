#include "JoinsessionCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

#include <QXmlStreamWriter>
#include "../../common/memleakdetect.h"

JoinsessionCommandHandler::JoinsessionCommandHandler()
{
}

/*! \brief handles a JOINSESSION command.  If the Director in the Command target is in a Session a ProtocolResponse is sent with a SessionID field 
 *	and the Session's Design XML in the response contents.
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
	//It is important that we get the session ID from the component rather than looking for
	//a component that matches the one we want in the open sessions.  This is because it is
	//possible that an old session is currently timing out and ending.  This would mean that there
	//could be more than one session open that contains the input director ID.  By selecting the
	//session from the Component, we assure that the caller joins the session that the director
	//is currently attached to.
	QUuid sessionId = ConnectionManager::Instance()->GetComponentsSessionId(directorID);
	if(sessionId == QUuid())
	{
		notFoundResponse->setContent("Director is not currently in a session.");
		return notFoundResponse;
	}
	QSharedPointer<SessionInfo> sessInfo = ConnectionManager::Instance()->getSessionInfo(sessionId);
	if(sessInfo.isNull())
		return notFoundResponse;

	QUuid observerId = QUuid(command->getFieldValue("Observer-ID"));
	if(observerId != QUuid() && !sessInfo->isAuthorizedObserver(observerId))
	{
		QString password = command->getFieldValue("Password");
		sessInfo->addAuthorizedObserver(observerId,password);
	}

	okResponse->setFieldValue("SessionID",sessionId.toString());
	
	//Write out the experiment's XML
	QByteArray pictoDataXml = sessInfo->experimentXml();
	okResponse->setContent(pictoDataXml);

	return okResponse;
}
