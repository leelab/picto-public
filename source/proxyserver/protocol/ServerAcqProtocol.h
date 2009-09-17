#ifndef _SERVERACQPROTOCOL_H_
#define _SERVERACQPROTOCOL_H_

#include "../../common/common.h"

#include "../../common/protocol/Protocol.h"

#include <QObject>

struct ServerAcqProtocol : Picto::Protocol
{
public:
	ServerAcqProtocol(QObject *acqPlugin);

private:
	QString id();
	QString version();
};

#endif
