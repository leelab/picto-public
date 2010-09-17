#ifndef _ENDSESSION_COMMAND_HANDLER_H_
#define _ENDSESSION_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles the ENDSESSION commands
 *
 *	PictoWorkstation sends these commands when it wants to terminate a session.
 *	The command results in an ENDSESSION directive being included in the next reply 
 *	to a DIRECTORUPDATE command.
 *
 *	FORMAT
 *		STARTSESSION 489360f0-6e88-11df-be2b-0800200c9a66 PICTO/1.0

 *	The target is the session ID to kill
 *
 *	RESPONSES
 *		There are 2 possible responses:
 *		200:OK - sent if the command is proccessed sucessfully
 *		404:NotFound - sent if the session ID isn't recognized
 */

struct EndsessionCommandHandler : Picto::ProtocolCommandHandler
{
public:
	EndsessionCommandHandler();

	QString method() { return QString("ENDSESSION"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
