#ifndef _SERVERACQPROTOCOL_H_
#define _SERVERACQPROTOCOL_H_

#include "../../common/common.h"

#include "../../common/protocol/Protocol.h"

#include <QObject>

/*!	\brief The ACQ protocol used by the proxy server
 *
 *	The ACQ protocol is used to communicate with the proxy server.  The protocol consists of nothing
 *	more than a GET command which will return neural data.
 */
struct ProxyServerAcqProtocol : Picto::Protocol
{
public:
	ProxyServerAcqProtocol(QObject *acqPlugin);

private:
	QString id();
	QString version();
};

#endif
