#ifndef _JOINSESSION_COMMAND_HANDLER_H_
#define _JOINSESSION_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles the JoinSESSION commands
 *
 *	The JOINSESSION command is sent from PictoWorkstation to the server to join 
 *	an already active session. This allows the Workstation instance to observe 
 *	(and possibly control) the session. Initially, the command will allow the 
 *	joining Workstation full access to the session, so they can issue task commands. 
 *	However, we may wish to limit the Workstation access at some point in the future.
 *
 *	FORMAT
 *		JOINSESSION 582f4440-43f7-11df-9879-0800200c9a66 PICTO/1.0
 *
 *	The Uuid is the Director ID
 *
 *	RESPONSES
 *		There are 3 possible responses:
 *		200:OK - sent if the command is proccessed sucessfully
 *		401:Unauthorized - The server isn't running a session, so it doesn't make sense to join.
 *		404:NotFound - sent if the director instance or proxy server isn't found
 *
 *		If the response is 200:OK, then an XML fragment is included in the content
 *		declaring the session ID and experiment XML:
 *		<JoinSession>
 *		     <SessionID>f590a5b0-43ea-11df-9879-0800200c9a66</SessionID>
 *		     <Experiment>
 *		          ...
 *		     </Experiment>
 *		</JoinSession> 
 */

struct JoinsessionCommandHandler : Picto::ProtocolCommandHandler
{
public:
	JoinsessionCommandHandler();

	QString method() { return QString("JOINSESSION"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
