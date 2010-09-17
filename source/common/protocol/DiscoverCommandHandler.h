#ifndef _DISCOVERCOMMANDHANDLER_H_
#define _DISCOVERCOMMANDHANDLER_H_

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

/*!	\brief Handles DISCOVER commands. Kinda, sorta not implemented
 *
 *	Although this command handler isn't implemented, there is an DISCOVER command that
 *	does get passed around the network.  If you dig into the PictoServer code (in particular
 *	/server/network/server.cpp), you'll see that if a DISCOVER command is received via a UDP
 *	broadcast, we assume that it is coming from someone who is looking for a server instance
 *	respond by broadacasting an ANNOUNCE command back.
 *
 *	It may be wise to get rid of this command handler...
 */

#if defined WIN32 || defined WINCE
struct PICTOLIB_API DiscoverCommandHandler : ProtocolCommandHandler
#else
struct DiscoverCommandHandler : ProtocolCommandHandler
#endif
{
public:
	DiscoverCommandHandler();

	QString method() { return QString("DISCOVER"); }
	QSharedPointer<ProtocolResponse> processCommand(QSharedPointer<ProtocolCommand>);
};


}; //namespace Picto

#endif
