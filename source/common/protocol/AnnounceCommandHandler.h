
#ifndef _ANNOUNCECOMMANDHANDLER_H_
#define _ANNOUNCECOMMANDHANDLER_H_

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

/*!	\brief This was meant to handle ANNOUNCE commands, but since they are an integral part of the ServerDiscovery system in the
 *	Director and Proxy, handling of ANNOUNCE commands ended up getting hard coded into the ServerDiscoverer and CommandChannel
 *	classes.
 *	\details Using this AnnounceCommandHandler to handle the ANNOUNCE commands was actually a pretty good idea though so it might
 *	be worthwhile at some point to try to clean things up so that ANNOUNCE handling isn't hard coded into anything.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct PICTOLIB_CLASS AnnounceCommandHandler : ProtocolCommandHandler
{
public:
	AnnounceCommandHandler();

	QString method() { return QString("ANNOUNCE"); }
	QSharedPointer<ProtocolResponse> processCommand(QSharedPointer<ProtocolCommand>);
};

}; //namespace Picto

#endif
