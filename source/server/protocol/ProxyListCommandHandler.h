#ifndef _PROXYLIST_COMMAND_HANDLER_H_
#define _PROXYLIST_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles the PROXYLIST commands
 *
 *	PictoWorkstation sends these commands in order to get a list of all the
 *	PictoProxyServers that are registered with the server.
 *
 *	FORMAT
 *		PROXYLIST / PICTO.1/0
 *	RESPONSES
 *		The response is always of type 200 OK.  The content contains an
 *		XML fragment listing the connected director instances.
 * 
 *	<Proxies>
 *		<Proxy>
 *			<Id>5</Id>
 *			<Name>Room 408 Proxy Server</Name>
 *		</Proxy>
 *		<Proxy>
 *			<Id>8</Id>
 *			<Name>Room 406 Proxy Server</Name>
 *		</Proxy>
 *	</Proxies> 
 */

struct ProxyListCommandHandler : Picto::ProtocolCommandHandler
{
public:
	ProxyListCommandHandler();

	QString method() { return QString("PROXYLIST"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
