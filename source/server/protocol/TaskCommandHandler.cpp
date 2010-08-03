#include "TaskCommandHandler.h"

#include "../../common/globals.h"
#include "../connections/ConnectionManager.h"

#include <QXmlStreamWriter>
#include <QThread>
#include <QTime>

TaskCommandHandler::TaskCommandHandler()
{
}

/*! \brief handles a TASK command
 *
 */
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{

	//! \TODO Record the time that each command is received

	printf("TASK handler: %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::NotFound));
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));


	//grab the session info and connection manager
	sessionId_ = QUuid(command->getFieldValue("Session-ID"));
	conMgr_ = ConnectionManager::Instance();
	sessInfo_ = conMgr_->getSessionInfo(sessionId_);

	if(sessInfo_.isNull())
	{
		badReqResponse->setContent("Session not found");
		return badReqResponse;
	}

	//Parse the command
	QString target = command->getTarget();

	if(target.startsWith("start",Qt::CaseInsensitive))
	{
		QString taskname;
		int colonIndex = target.indexOf(":");
		taskname = target.mid(colonIndex+1);
		return start(taskname);
	}
	else if(target.startsWith("pause",Qt::CaseInsensitive))
	{
		return pause();
	}
	else if(target.startsWith("stop",Qt::CaseInsensitive))
	{
		return stop();
	}
	else if(target.startsWith("resume",Qt::CaseInsensitive))
	{
		return resume();
	}
	else if(target.startsWith("reward", Qt::CaseInsensitive))
	{
		int channel;
		int colonIndex = target.indexOf(":");
		bool ok;
		channel = target.mid(colonIndex+1).toInt(&ok);
		if(!ok)
			return notFoundResponse;
		else
			return reward(channel);
	}
	else
	{
		return notFoundResponse;
	}

}


//! Starts a task running either from the beginning or from a paused state
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::start(QString taskname)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));

	//Check that we have a Director and that it is idle
	DirectorStatus::DirectorStatus dirStatus = conMgr_->getDirectorStatus(sessionId_);

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

	//Add a START directive to the session info
	sessInfo_->addPendingDirective("START "+taskname);

	//Wait around until the Director's status changes to "running"
	QTime timer;
	timer.start();

	do
	{
		QThread::yieldCurrentThread();
		QCoreApplication::processEvents();
	}while(timer.elapsed() < 10000 && conMgr_->getDirectorStatus(sessionId_) < DirectorStatus::stopped);

	if(timer.elapsed() <10000)
	{
		return okResponse;
	}
	else
	{
		badReqResponse->setContent("Task failed to start");
		return badReqResponse;
	}

}

//! \brief Stops a running task
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::stop()
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));

	if(conMgr_->getDirectorStatus(sessionId_) < DirectorStatus::paused)
	{
		badReqResponse->setContent("Director not currently running");
		return badReqResponse;
	}

	sessInfo_->addPendingDirective("STOP");
	sessInfo_->flushCache();
	//conMgr_->setDirectorStatus(sessionId_,DirectorStatus::stopped);


	return okResponse;

}

//! \brief Pauses a running task
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::pause()
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));

	if(conMgr_->getDirectorStatus(sessionId_) != DirectorStatus::running)
	{
		badReqResponse->setContent("Director not currently running");
		return badReqResponse;
	}

	sessInfo_->addPendingDirective("PAUSE");
	conMgr_->setDirectorStatus(sessionId_,DirectorStatus::paused);


	return okResponse;
}


//! \brief resumes a paused task
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::resume()
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));

	if(conMgr_->getDirectorStatus(sessionId_) != DirectorStatus::paused)
	{
		badReqResponse->setContent("Director not currently puased");
		return badReqResponse;
	}

	sessInfo_->addPendingDirective("RESUME");
	conMgr_->setDirectorStatus(sessionId_,DirectorStatus::running);
	
	return okResponse;
}

//! Delivers a reward
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::reward(int channel)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> badReqResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::BadRequest));

	if(channel <1 || channel >4)
	{
		badReqResponse->setContent("Non-existant reward channel");
		return badReqResponse;
	}

	sessInfo_->addPendingDirective(QString("REWARD %1").arg(channel));

	return okResponse;

}