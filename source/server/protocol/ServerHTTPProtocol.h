#ifndef _SERVERHTTPPROTOCOL_H_
#define _SERVERHTTPPROTOCOL_H_

#include <QString>

#include "../../common/common.h"

#include "../../common/protocol/Protocol.h"

/*!	\brief The HTTP protocol for PictoServer
 *
 *	This is actually more of a test protocol.  There is no reason for PictoServer
 *	to implement the HTTP protocol at the moment (although you could use something
 *	like this to stream images of an experiment to a web browser).
 */
struct ServerHTTPProtocol : Picto::Protocol
{
public:
	ServerHTTPProtocol();

private:
	QString id();
	QString version();
};

#endif
