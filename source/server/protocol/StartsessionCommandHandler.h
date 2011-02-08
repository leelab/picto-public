#ifndef _STARTSESSION_COMMAND_HANDLER_H_
#define _STARTSESSION_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles the STARTSESSION commands
 *
 *	PictoWorkstation sends these commands when it wants a remote Director
 *	instance to load an experiment.  The command causes a STARTSESSION directive 
 *	to be included the next time that Director instance sends a DIRECTORUPDATE
 *	command.
 *
 *	FORMAT
 *		STARTSESSION 582f4440-43f7-11df-9879-0800200c9a66/5 PICTO/1.0
 *		Content-Length:???
 *
 *		<Experiment> ... </Experiment>
 *	The Uuid is the Director id, and the number is the proxy server id
 *
 *	RESPONSES
 *		There are 3 possible responses:
 *		200:OK - sent if the command is proccessed sucessfully
 *		401:Unauthorized - sent if the director instance is already running
 *		404:NotFound - sent if the director instance or proxy server isn't found
 *
 *		If the response is 200:OK, then an XML fragment is included in the content
 *		declaring the session ID.
 *		
 */

struct StartsessionCommandHandler : Picto::ProtocolCommandHandler
{
public:
	StartsessionCommandHandler();

	QString method() { return QString("STARTSESSION"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
