/*! \brief Handles the STARTTASK commands
 *
 *	PictoWorkstation sends these commands when it wants a remote Director
 *	instance to start running a task.  The sequence of events is:
 *		1. Workstation sends STARTTASK command to server
 *		2. Server adds START directive to session
 *		3. Workstation sends DIRECTORUPDATE command to Server
 *		4. Response to DIRECTORUPDATE command includes STARTTASK directive
 *		5. Director immediately sends a response indicating that its status is "running"
 *		6. Director starts running task
 *		7. Server notes that Director's status has changed to "running" and
 *		   sends a 200:OK response
 *
 *	Note that this command takes a while to generate a response, so the sender 
 *	should wait more than 1 second before timing out (since the UPDATEDIRECTOR
 *	commands are only sent out once per second).
 *
 *	FORMAT
 *		STARTTASK taskname PICTO/1.0
 *		Session-ID: 582f4440-43f7-11df-9879-0800200c9a66
 *
 *	RESPONSES
 *		There are 3 possible responses:
 *		200:OK - sent if the command is proccessed sucessfully
 *		400:BadRequest - sent if director never switches to "running", or it is already running
 *		404:NotFound - sent if the session ID isn't recognized
 */

#ifndef _STARTTASK_COMMAND_HANDLER_H_
#define _STARTTASK_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

struct StarttaskCommandHandler : Picto::ProtocolCommandHandler
{
public:
	StarttaskCommandHandler();

	QString method() { return QString("STARTTASK"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
