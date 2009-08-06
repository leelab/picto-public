#ifndef _SERVERPICTOPROTOCOL_H_
#define _SERVERPICTOPROTOCOL_H_

#include "../../common/common.h"

#include "../../common/protocol/Protocol.h"

struct ServerPictoProtocol : Picto::Protocol
{
public:
	ServerPictoProtocol();

private:
	QString id();
	QString version();
};

#endif
