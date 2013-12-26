#ifndef _DISCOVERCOMMANDHANDLER_H_
#define _DISCOVERCOMMANDHANDLER_H_

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

/*!	\brief This was meant to handle DISCOVER commands, but since they are an integral part of the ServerDiscovery system in the
 *	Director and Proxy, handling of DISCOVER commands ended up getting hard coded into the Server class.
 *	\details Using this DiscoverCommandHandler to handle the DISCOVER commands was actually a pretty good idea though so it might
 *	be worthwhile at some point to try to clean things up so that DISCOVER handling isn't hard coded into anything.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
