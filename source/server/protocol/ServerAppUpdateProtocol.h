#ifndef _SERVERAPPUPDATEPROTOCOL_H_
#define _SERVERAPPUPDATEPROTOCOL_H_

#include "../../common/common.h"

#include "../../common/protocol/Protocol.h"

/*!	\brief The Server App Update protocol.
 *
 *	The Server App Update protocol contains all of the commands that are used by the App Update system.
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
