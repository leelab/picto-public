#ifndef _GETCOMMANDHANDLER_H_
#define _GETCOMMANDHANDLER_H_

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

/*!	\brief Handles GET commands.
 *
 *	This command handler was used during testing, but isn't really part of Picto at
 *	this point.  Instead of a more general GET command, I implemented a more specific
 *	GETDATA command (which can be found in /server/protocol/).
 */
#if defined WIN32 || defined WINCE
struct PICTOLIB_API GetCommandHandler : ProtocolCommandHandler
#else
struct GetCommandHandler : ProtocolCommandHandler
#endif
{
public:
	GetCommandHandler();

	QString method() { return QString("GET"); }
	QSharedPointer<ProtocolResponse> processCommand(QSharedPointer<ProtocolCommand>);
};

}; //namespace Picto

#endif
