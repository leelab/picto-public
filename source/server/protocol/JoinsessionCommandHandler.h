#ifndef _JOINSESSION_COMMAND_HANDLER_H_
#define _JOINSESSION_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles the JOINSESSION commands
 *
 *	\details The JOINSESSION command is sent from PictoWorkstation to the server to join 
 *	an already active session. This allows the Workstation instance to observe 
 *	(and possibly control if authorized) the Session. Responses to JOINSESSION commands
 *	include the Session's full Design XML in the response contents and the Session's 
 *	SessionID in the SessionID field.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct JoinsessionCommandHandler : Picto::ProtocolCommandHandler
{
public:
	JoinsessionCommandHandler();

	QString method() { return QString("JOINSESSION"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
