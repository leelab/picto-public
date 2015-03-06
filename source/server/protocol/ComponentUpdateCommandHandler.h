#ifndef _DIRECTORUPDATE_COMMAND_HANDLER_H_
#define _DIRECTORUPDATE_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles COMPONENTUPDATE commands, which are sent by ComponentInterface objects (Director and Proxy).
 *
 *	\details ComponentInterface objects (ie. Director, Proxy... not Workstation) send COMPONENTUPDATE 
 *	commands to the Server at a fixed interval.  These provide up to data information about the Component status,
 *	keep the network connection from timing out and are used to send directives from the Picto Server to the Component.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct ComponentUpdateCommandHandler : Picto::ProtocolCommandHandler
{
public:
	ComponentUpdateCommandHandler();

	QString method() { return QString("COMPONENTUPDATE"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
