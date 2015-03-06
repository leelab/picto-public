#ifndef _DIRECTORSTARTRESPONSEHANDLER_H_
#define _DIRECTORSTARTRESPONSEHANDLER_H_

#include "../../common/protocol/StartResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief Implements StartResponseHandler to run a Task whose name is included in a message over the network.
 *	\details This class actually runs the entire Experimental Task inside its processResponse() function.
 *	This is not a clean run model, and we should fix it at some point such that processResponse() only schedules
 *	that the task be run, and the Qt event loop actually does the work of starting the Task.  
 *	See Picto::CommandChannel::processResponses() for more details.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct DirectorStartResponseHandler : public Picto::StartResponseHandler
{
public:
	DirectorStartResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
