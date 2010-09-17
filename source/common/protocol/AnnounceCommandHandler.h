
#ifndef _ANNOUNCECOMMANDHANDLER_H_
#define _ANNOUNCECOMMANDHANDLER_H_

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

/*!	\brief Handles ANNOUNCE commands. Kinda, sorta not implemented
 *
 *	Although this command handler isn't implemented, there is an ANNOUNCE command that
 *	does get passed around the network.  If you dig into the PictoServer code (in particular
 *	/server/network/server.cpp), you'll see that if a UDP ANNOUNCE command is received, we
 *	assume that it is coming from a proxy server, and add that server to our list.
 *
 *	It may be wise to get rid of this command handler...
 */
struct PICTOLIB_CLASS AnnounceCommandHandler : ProtocolCommandHandler
{
public:
	AnnounceCommandHandler();

	QString method() { return QString("ANNOUNCE"); }
	QSharedPointer<ProtocolResponse> processCommand(QSharedPointer<ProtocolCommand>);
};

/*! @} */

}; //namespace Picto

#endif
