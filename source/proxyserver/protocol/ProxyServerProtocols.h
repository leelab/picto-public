#ifndef _SERVERPROTOCOLS_H_
#define _SERVERPROTOCOLS_H_

#include <map>

#include <QString>
#include <QSharedPointer>

#include "../../common/common.h"
#include "../../common/protocol/Protocol.h"

/*!	\brief NO LONGER USED (except by things which themselves are no longer used).  SHOULD PROBABLY BE DELTED.  Original documentation appears below.
 *
 *	\details The protocols for the proxy server.
 *
 *	This object holds any protocols that are being used by the proxy server.  This is
 *	identical (or almost identical) to the same oject used by PictoServer.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct ProxyServerProtocols
{
public:
	ProxyServerProtocols();
	
	void addProtocol(QSharedPointer<Picto::Protocol> protocol);
	QSharedPointer<Picto::Protocol> getProtocol(QString protocolName);

private:
	std::map<QString,QSharedPointer<Picto::Protocol> > protocols;
};

#endif
