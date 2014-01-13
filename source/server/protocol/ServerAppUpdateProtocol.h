#ifndef _SERVERAPPUPDATEPROTOCOL_H_
#define _SERVERAPPUPDATEPROTOCOL_H_

#include "../../common/common.h"

#include "../../common/protocol/Protocol.h"

/*!	\brief The Server App Update protocol.
 *
 *	\details The Server App Update protocol contains all of the commands that are used by the App Update system.
 *	We keep this Protocol separate from the Picto protocol so that older Picto installations will still be able
 *	to talk to the update system and get themselves updated even if the Picto protocol itself is updated.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct ServerAppUpdateProtocol : Picto::Protocol
{
public:
	ServerAppUpdateProtocol();

private:
	QString id();
	QString version();
};

#endif
