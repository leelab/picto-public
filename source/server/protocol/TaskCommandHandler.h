/*! \brief Handles the TASK commands
 *
 *	PictoWorkstation sends these commands when it wants a remote Director
 *	instance to change the current state of a task (start, stop, pause)
 *
 *	FORMAT
 *		STARTTASK command[:argument] PICTO/1.0
 *		Session-ID: 582f4440-43f7-11df-9879-0800200c9a66
 *		
 *		Valid commands: start, stop, pause, resume, reward (case insenseitive)
 *
 *	RESPONSES
 *		There are 3 possible responses:
 *		200:OK - sent if the command is proccessed sucessfully
 *		400:BadRequest - sent if the command doesn't make sense (e.g. calling stop on a task that isn't currently running)
 *		404:NotFound - sent if the session ID, taskname, or command isn't recognized
 */

#ifndef _TASK_COMMAND_HANDLER_H_
#define _TASK_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"
#include "../connections/SessionInfo.h"

#include <QUuid>

class ConnectionManager;

struct TaskCommandHandler : Picto::ProtocolCommandHandler
{
public:
	TaskCommandHandler();

	QString method() { return QString("TASK"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

private:
	QSharedPointer<Picto::ProtocolResponse> start(QString taskname);
	QSharedPointer<Picto::ProtocolResponse> stop();
	QSharedPointer<Picto::ProtocolResponse> pause();
	QSharedPointer<Picto::ProtocolResponse> resume();
	QSharedPointer<Picto::ProtocolResponse> reward(int channel);

	QUuid sessionId_;
	ConnectionManager *conMgr_;
	QSharedPointer<SessionInfo> sessInfo_;
};


#endif
