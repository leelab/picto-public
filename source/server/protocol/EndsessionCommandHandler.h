#ifndef _ENDSESSION_COMMAND_HANDLER_H_
#define _ENDSESSION_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles the ENDSESSION commands
 *
 *	PictoWorkstation sends these commands when it wants to terminate a session.
 *	The command results in an ENDSESSION directive being included in the next reply 
 *	to COMPONENTUPDATE commands from every Component (Director or Proxy).
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct EndsessionCommandHandler : Picto::ProtocolCommandHandler
{
public:
	EndsessionCommandHandler();

	QString method() { return QString("ENDSESSION"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
