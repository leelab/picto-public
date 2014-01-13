#ifndef _PROXYLIST_COMMAND_HANDLER_H_
#define _PROXYLIST_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles PROXYLIST commands
 *
 *	PictoWorkstation sends these commands in order to get a list of all the
 *	Picto Proxy instances that are currently connected to the server.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct ProxyListCommandHandler : Picto::ProtocolCommandHandler
{
public:
	ProxyListCommandHandler();

	QString method() { return QString("PROXYLIST"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
