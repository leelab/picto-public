#ifndef _TASK_COMMAND_HANDLER_H_
#define _TASK_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"
#include "../connections/SessionInfo.h"

#include <QUuid>

class ConnectionManager;

/*! \brief Handles TASK commands
 *
 *	PictoWorkstation send TASK commands when it wants a remote Director
 *	instance to change the current state of a task (start, stop, pause, etc).
 *	It is also used to alter Experimental Property values (init values) during runtime,
 *	provide information about an operator click in the viewer window, trigger a reward,
 *	and change the notes, name or saved value of a Task Run.  Essentially, this
 *	is the Workstation's interface to a running session.  For more details
 *	see processCommand().
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct TaskCommandHandler : Picto::ProtocolCommandHandler
{
public:
	TaskCommandHandler();

	QString method() { return QString("TASK"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

private:
	QSharedPointer<Picto::ProtocolResponse> start(QString taskname,QSharedPointer<SessionInfo> sessInfo);
	QSharedPointer<Picto::ProtocolResponse> stop(QSharedPointer<SessionInfo> sessInfo);
	QSharedPointer<Picto::ProtocolResponse> pause(QSharedPointer<SessionInfo> sessInfo);
	QSharedPointer<Picto::ProtocolResponse> resume(QSharedPointer<SessionInfo> sessInfo);
	QSharedPointer<Picto::ProtocolResponse> reward(int channel, QString details,QSharedPointer<SessionInfo> sessInfo);
	QSharedPointer<Picto::ProtocolResponse> parameter(QString paramId, QString details,QSharedPointer<SessionInfo> sessInfo);
	QSharedPointer<Picto::ProtocolResponse> click(QString details,QSharedPointer<SessionInfo> sessInfo);
	QSharedPointer<Picto::ProtocolResponse> updateServerData(QString data,QSharedPointer<SessionInfo> sessInfo);

	QUuid sessionId_;
	ConnectionManager *conMgr_;
	QSharedPointer<SessionInfo> sessInfo_;
};


#endif
