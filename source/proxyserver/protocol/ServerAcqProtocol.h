#ifndef _SERVERACQPROTOCOL_H_
#define _SERVERACQPROTOCOL_H_

#include "../../common/common.h"

#include "../../common/protocol/Protocol.h"

struct ServerAcqProtocol : Picto::Protocol
{
public:
	ServerAcqProtocol();

private:
	QString id();
	QString version();
};

#endif
