#ifndef _SERVERACQPROTOCOL_H_
#define _SERVERACQPROTOCOL_H_

#include "../../common/common.h"

#include "../../common/protocol/Protocol.h"

#include <QObject>

/*!	\brief NO LONGER USED.  THIS SHOULD PROBABLY BE DELETED.  Original documentation apperas below.
 *
 *	\details  The ACQ protocol used by the proxy server
 *
 *	The ACQ protocol is used to communicate with the proxy server.  The protocol consists of nothing
 *	more than a GET command which will return neural data.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
