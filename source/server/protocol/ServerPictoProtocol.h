#ifndef _SERVERPICTOPROTOCOL_H_
#define _SERVERPICTOPROTOCOL_H_

#include "../../common/common.h"

#include "../../common/protocol/Protocol.h"

/*!	\brief The PICTO protocol.
 *
 *	The PICTO protocol contains all of the commands that are used by the Picto system to run
 *	a Session, view its progress and upload its data.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct ServerPictoProtocol : Picto::Protocol
{
public:
	ServerPictoProtocol();

private:
	QString id();
	QString version();
};

#endif
