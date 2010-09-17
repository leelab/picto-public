#ifndef _TERMINATECOMMANDHANDLER_H_
#define _TERMINATECOMMANDHANDLER_H_

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

/*!	\brief Handles the TERMINATE command
 *
 *	This command is supposed to terminate the connection (and was tested to do
 *	so).  However, I never really saw the need to do this, so it isn't really used.
 */

#if defined WIN32 || defined WINCE
struct PICTOLIB_API TerminateCommandHandler : ProtocolCommandHandler
#else
struct TerminateCommandHandler : ProtocolCommandHandler
#endif
{
public:
	TerminateCommandHandler();

	QString method() { return QString("TERMINATE"); }
	QSharedPointer<ProtocolResponse> processCommand(QSharedPointer<ProtocolCommand>);
};

}; //namespace Picto

#endif
