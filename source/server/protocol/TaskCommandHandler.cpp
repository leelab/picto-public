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

/*! \brief Handles TASK commands.
 *	\details TASK commands are commands that the Workstation sends to cause something to happen in the Session.  They
 *	are defined in the target of the command and include:
 *	- start: Start the task with the name after the colon in the command header
 *	- pause: Pause the currently running task
 *	- stop: Stop the currently running task
 *	- resume: Resume the currently running task
 *	- reward: Supply a reward on the reward channel after the colon in the command header.  If the command has content, use
 *		the number in the content to update the default reward duration for the channel.
 *	- isauthorized:	Checks if the Workstation sending the command is authorized to use other Task commands.  If not an
 *		unauthorized Response will be returned.  If so, an OK response will be returned.
 *	- parameter: Update the initValue of the Property with the AssetId printed after the command header's colon.  Change it
 *		to the value included in the command content.
 *	- click: Inform the Director that an Operator click occured at the coordinates included after the command header's colon.
 *	- updatedata: Change the saved TaskRun information to the serialized value included in the command contents.  This is useful
 *		for adding notes, changing the name or changing the saved value of a Session's Task Run.
 *	
 *	This function processes all of these commands, adds appropriate Directives for the Director when necessary, changes
 *	Session data when necessary, and returns appropriate information when necessary.
 */
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
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

/*! \brief Adds a START directive for the Director to cause it to start the task with the input task name.
 *	Returns an appropraite ProtocolResponse.
*/
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

	sessInfo->addPendingDirective("START "+taskname,"DIRECTOR");

	return okResponse;

}

/*! \brief Adds a STOP directive for the Director to cause it to stop running the current task.
 *	Returns an appropraite ProtocolResponse.
*/
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

	return okResponse;

}

/*! \brief Adds a PAUSE directive for the Director to cause it to pause the currently running task.
*	Returns an appropraite ProtocolResponse.
*/
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


/*! \brief Adds a RESUME directive for the Director to cause it to resume running the task that is currently paused.
*	Returns an appropraite ProtocolResponse.
*/
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

/*! \brief Adds a REWARD directive for the Director to cause it to provide a reward on the input channel with the input details.
 *	\details The details input may be an empty string, if not it includes a new default reward value for the input channel.
 *	That value is included in the REWARD directive.
 *	Returns an appropraite ProtocolResponse.
*/
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

/*! \brief Adds a PARAMETER directive for the Director to cause it update the initValue of the Property with the input AssetId (called paramid) to 
 *	the value input in the details field.
 *	Returns an appropraite ProtocolResponse.
*/
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::parameter(QString paramId, QString details,QSharedPointer<SessionInfo> sessInfo)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	details.prepend(QString("id=%1\n").arg(paramId));
	sessInfo->addPendingDirective(QString("PARAMETER %1").arg(details),"DIRECTOR");

	return okResponse;
}

/*! \brief Adds a CLICK directive for the Director to inform it of an operator click in the workstation at the coordinates input as details in
 *	the format (x,y ie. "100,300").
 *	Returns an appropraite ProtocolResponse.
*/
QSharedPointer<Picto::ProtocolResponse> TaskCommandHandler::click(QString details,QSharedPointer<SessionInfo> sessInfo)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",Picto::ProtocolResponseType::OK));
	okResponse->setRegisteredType(Picto::RegisteredResponseType::Immediate);
	sessInfo->addPendingDirective(QString("CLICK %1").arg(details),"DIRECTOR");
	return okResponse;
}

/*! \brief Creates a TaskRunDataUnit from the serial XML code input as "data".  Uses SessionInfo::modifyTaskRunData() to change the contents of the stored Task Run data 
 *	with the same DataId to match that of the input.  This is useful to update the Task Run name, notes, or saved value.
 *	Returns an appropraite ProtocolResponse.
 */
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
			runUnit->fromXml(xmlReader);
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