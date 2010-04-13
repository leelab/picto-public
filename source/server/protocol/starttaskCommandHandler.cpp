#include "StarttaskCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"

#include <QXmlStreamWriter>
#include <QThread>
#include <QTime>

StarttaskCommandHandler::StarttaskCommandHandler()
{
}

/*! \brief handles a STARTTASK command
 *
 */
QSharedPointer<Picto::ProtocolResponse> StarttaskCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	printf("STARTTASK handler: %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));

	//Parse the command
	QString taskname = command->getTarget().remove('\"');
	QUuid sessionId = QUuid(command->getFieldValue("Session-ID"));

	//Check that we have a Director and that it is idle
	ConnectionManager *conMgr = ConnectionManager::Instance();
	DirectorStatus::DirectorStatus dirStatus = conMgr->getDirectorStatus(sessionId);

	if(dirStatus == DirectorStatus::running)
	{
		badReqResponse->setContent("Director is already running a task.");
		return badReqResponse;
	}
	else if(dirStatus == DirectorStatus::notFound)
	{
		badReqResponse->setContent("Director not found");
		return badReqResponse;
	}

	QSharedPointer<SessionInfo> sessInfo = conMgr->getSessionInfo(sessionId);
	
	if(sessInfo.isNull())
	{
		badReqResponse->setContent("Session not found");
		return badReqResponse;
	}

	//Add a START directive to the session info
	sessInfo->addPendingDirective("START "+taskname);

	//Wait around until the Director's status changes to "running"
	QTime timer;
	timer.start();

	do
	{
		QThread::yieldCurrentThread();
	}while(timer.elapsed() < 2000 && conMgr->getDirectorStatus(sessionId) != DirectorStatus::running);

	if(timer.elapsed() <2000)
	{
		return okResponse;
	}
	else
	{
		badReqResponse->setContent("Task failed to start");
		return badReqResponse;
	}

}
