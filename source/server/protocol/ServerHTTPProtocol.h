#ifndef _SERVERHTTPPROTOCOL_H_
#define _SERVERHTTPPROTOCOL_H_

#include <QString>

#include "../../common/common.h"

#include "../../common/protocol/Protocol.h"

struct ServerHTTPProtocol : Picto::Protocol
{
public:
	ServerHTTPProtocol();

private:
	QString id();
	QString version();
};

#endif
