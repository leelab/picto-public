#include "TaskCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/ConnectionManager.h"

#include <QXmlStreamWriter>
#include <QThread>
#include <QTime>
#include "../../common/memleakdetect.h"

TaskCommandHandler::TaskCommandHandler()
{
}

/*! \brief handles a TASK command
 *
 */
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{

	/*! \TODO Record the time that each command is received.  This way less trivial 
	 *	than it appears.  We want the time to be recorded in the session's timebase,
	 *	but the server doesn't hold a timer in that timebase, so at best, we could
	 *	estimate the time using the frame table.  We could also try inserting the 
	 *	command in the state transition table.
	 */



	printf("TASK handler: %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
	notFoundResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> unauthResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::Unauthorized));
	unauthResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
	badReqResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);

	//grab the session info and connection manager
	sessionId_ = QUuid(command->getFieldValue("Session-ID"));
	conMgr_ = ConnectionManager::Instance();
	QSharedPointer<SessionInfo> sessInfo = conMgr_->getSessionInfo(sessionId_);

	if(sessInfo.isNull())
	{
		badReqResponse->setContent("Session not found");
		return badReqResponse;
	}

	//Figure out if the observer sending the command is an authorized user
	QUuid observerId = QUuid(command->getFieldValue("Observer-ID"));
	if(!sessInfo->isAuthorizedObserver(observerId))
		return unauthResponse;

	//Parse the command
	QString target = command->getTarget();

	if(target.startsWith("start",Qt::CaseInsensitive))
	{
		QString taskname;
		int colonIndex = target.indexOf(":");
		taskname = target.mid(colonIndex+1);
		return start(taskname,sessInfo);
	}
	else if(target.startsWith("pause",Qt::CaseInsensitive))
	{
		return pause(sessInfo);
	}
	else if(target.startsWith("stop",Qt::CaseInsensitive))
	{
		return stop(sessInfo);
	}
	else if(target.startsWith("resume",Qt::CaseInsensitive))
	{
		return resume(sessInfo);
	}
	else if(target.startsWith("reward", Qt::CaseInsensitive))
	{
		int channel;
		int colonIndex = target.indexOf(":");
		bool ok;
		channel = target.mid(colonIndex+1).toInt(&ok);
		QString directive(command->getContent());
		if(!ok)
			return notFoundResponse;
		else
			return reward(channel,directive,sessInfo);
	}
	else if(target.startsWith("isauthorized", Qt::CaseInsensitive))
	{
		//This is a weird little command.  It's basically a check so that a workstation can determine
		//if it is authorized to send task commands.  If the workstation isn't authorized, the command
		//will return 401:Unauthorized, otherwise, we do nothing, and return 200:OK
		okResponse->setFieldValue("Password",sessInfo->getPassword());
		return okResponse;
	}
	else if(target.startsWith("parameter", Qt::CaseInsensitive))
	{
		int colonIndex = target.indexOf(":");
		QString paramId = target.mid(colonIndex+1);
		QString directive(command->getContent());
		return parameter(paramId,directive,sessInfo);
	}
	else if(target.startsWith("click", Qt::CaseInsensitive))
	{
		int colonIndex = target.indexOf(":");
		QString details = target.mid(colonIndex+1);
		return click(details,sessInfo);
	}
	else if(target.startsWith("updatedata", Qt::CaseInsensitive))
	{
		QString data(command->getContent());
		return updateServerData(data,sessInfo);
	}
	else
	{
		return notFoundResponse;
	}

}


//! Starts a task running either from the beginning or from a paused state
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::start(QString taskname,QSharedPointer<SessionInfo> sessInfo)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
	badReqResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	//Check that we have a Director and that it is stopped
	ComponentStatus::ComponentStatus dirStatus = conMgr_->getComponentStatusBySession(sessionId_,"DIRECTOR");

	if(dirStatus == ComponentStatus::running)
	{
		badReqResponse->setContent("Director is already running a task.");
		return badReqResponse;
	}
	else if(dirStatus == ComponentStatus::notFound)
	{
		badReqResponse->setContent("Director not found");
		return badReqResponse;
	}else if(dirStatus == ComponentStatus::ending)
	{
		badReqResponse->setContent("Director is in the process of ending its session");
		return badReqResponse;
	}else if(dirStatus == ComponentStatus::idle)
	{
		badReqResponse->setContent("Director is not currently in a session");
		return badReqResponse;
	}

	//QTime timer;
	//if(!sessInfo->getComponentByType("PROXY").isNull())
	//{
	//	//Check that we have a Proxy and that it is stopped
	//	ComponentStatus::ComponentStatus proxyStatus = conMgr_->getComponentStatusBySession(sessionId_,"PROXY");

	//	if(proxyStatus == ComponentStatus::running)
	//	{
	//		badReqResponse->setContent("Proxy is already running a task.");
	//		return badReqResponse;
	//	}
	//	else if(proxyStatus == ComponentStatus::notFound)
	//	{
	//		badReqResponse->setContent("Proxy not found");
	//		return badReqResponse;
	//	}else if(proxyStatus == ComponentStatus::ending)
	//	{
	//		badReqResponse->setContent("Proxy is in the process of ending its session");
	//		return badReqResponse;
	//	}else if(proxyStatus == ComponentStatus::idle)
	//	{
	//		badReqResponse->setContent("Proxy is not currently in a session");
	//		return badReqResponse;
	//	}

	//	//Add a START directive to the session info
	//	timer.start();
	//	sessInfo->addPendingDirective("START "+taskname,"PROXY");
	//	do
	//	{
	//		QThread::yieldCurrentThread();
	//		QCoreApplication::processEvents();
	//	}while(timer.elapsed() < 10000 && conMgr_->getComponentStatusBySession(sessionId_,"PROXY") < ComponentStatus::stopped);
	//	if(timer.elapsed() >=10000)
	//	{
	//		badReqResponse->setContent("Proxy failed to start");
	//		return badReqResponse;
	//	}
	//}

	sessInfo->addPendingDirective("START "+taskname,"DIRECTOR");

	////Wait around until the Director's status changes to "running"
	//timer.start();
	//do
	//{
	//	QThread::yieldCurrentThread();
	//	QCoreApplication::processEvents();
	//}while(timer.elapsed() < 10000 && conMgr_->getComponentStatusBySession(sessionId_,"DIRECTOR") < ComponentStatus::stopped);

	//if(timer.elapsed() <10000)
	//{
	//	return okResponse;
	//}
	//else
	//{
	//	badReqResponse->setContent("Task failed to start");
	//	return badReqResponse;
	//}
	return okResponse;

}

//! \brief Stops a running task
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::stop(QSharedPointer<SessionInfo> sessInfo)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
	badReqResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	if(conMgr_->getComponentStatusBySession(sessionId_,"DIRECTOR") < ComponentStatus::paused)
	{
		badReqResponse->setContent("Director not currently running");
		return badReqResponse;
	}
	
	sessInfo->addPendingDirective("STOP","DIRECTOR");
	//QTime timer;
	//timer.start();
	//do
	//{
	//	QThread::yieldCurrentThread();
	//	QCoreApplication::processEvents();
	//}while(timer.elapsed() < 10000 && conMgr_->getComponentStatusBySession(sessionId_,"DIRECTOR") > ComponentStatus::stopped);
	//if(timer.elapsed() >=10000)
	//{
	//	badReqResponse->setContent("Director failed to stop");
	//	return badReqResponse;
	//}
	//if(!sessInfo->getComponentByType("PROXY").isNull())
	//{
	//	sessInfo->addPendingDirective("STOP","PROXY");
	//	timer.start();
	//	do
	//	{
	//		QThread::yieldCurrentThread();
	//		QCoreApplication::processEvents();
	//	}while(timer.elapsed() < 10000 && conMgr_->getComponentStatusBySession(sessionId_,"DIRECTOR") > ComponentStatus::stopped);
	//	if(timer.elapsed() >=10000)
	//	{
	//		badReqResponse->setContent("Proxy failed to stop");
	//		return badReqResponse;
	//	}
	//}

	//conMgr_->setComponentStatus(sessionId_,"DIRECTOR"ComponentStatus::stopped);


	return okResponse;

}

//! \brief Pauses a running task
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::pause(QSharedPointer<SessionInfo> sessInfo)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
	badReqResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	if(conMgr_->getComponentStatusBySession(sessionId_,"DIRECTOR") != ComponentStatus::running)
	{
		badReqResponse->setContent("Director not currently running");
		return badReqResponse;
	}

	sessInfo->addPendingDirective("PAUSE","DIRECTOR");
	conMgr_->setComponentStatus(sessionId_,"DIRECTOR",ComponentStatus::paused);


	return okResponse;
}


//! \brief resumes a paused task
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::resume(QSharedPointer<SessionInfo> sessInfo)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
	badReqResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	if(conMgr_->getComponentStatusBySession(sessionId_,"DIRECTOR") != ComponentStatus::paused)
	{
		badReqResponse->setContent("Director not currently puased");
		return badReqResponse;
	}

	sessInfo->addPendingDirective("RESUME","DIRECTOR");
	conMgr_->setComponentStatus(sessionId_,"DIRECTOR",ComponentStatus::running);
	
	return okResponse;
}

//! Delivers a reward
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::reward(int channel, QString details,QSharedPointer<SessionInfo> sessInfo)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));
	badReqResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	if(channel <1 || channel >4)
	{
		badReqResponse->setContent("Non-existant reward channel");
		return badReqResponse;
	}

	sessInfo->addPendingDirective(QString("REWARD %1\n%2").arg(channel).arg(details),"DIRECTOR");

	return okResponse;

}

QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::parameter(QString paramId, QString details,QSharedPointer<SessionInfo> sessInfo)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	details.prepend(QString("id=%1\n").arg(paramId));
	sessInfo->addPendingDirective(QString("PARAMETER %1").arg(details),"DIRECTOR");

	return okResponse;
}

QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::click(QString details,QSharedPointer<SessionInfo> sessInfo)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	sessInfo->addPendingDirective(QString("CLICK %1").arg(details),"DIRECTOR");
	return okResponse;
}

QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::updateServerData(QString data,QSharedPointer<SessionInfo> sessInfo)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
	notFoundResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
		
	//Read the content
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(data));
	bool foundSomething = false;
	while(!xmlReader->atEnd())
	{
		xmlReader->readNext();
		if(!xmlReader->isStartElement())
		{
			continue;
		}

		//We do different things depending on the type of data being sent
		QString dataType = xmlReader->name().toString();
		if(dataType == "TaskRunDataUnit")
		{
			QSharedPointer<Picto::TaskRunDataUnit> runUnit(new Picto::TaskRunDataUnit());
			runUnit->fromXml(data);
			sessInfo->modifyTaskRunData(runUnit);
			foundSomething = true;
		}
		else
		{
			return notFoundResponse;
		}
	}
	if(!foundSomething)
	{
		return notFoundResponse;
	}
	return okResponse;	
}