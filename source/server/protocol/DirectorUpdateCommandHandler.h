#ifndef _DIRECTORUPDATE_COMMAND_HANDLER_H_
#define _DIRECTORUPDATE_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles the COMPONENTUPDATE commands, which are sent by Director
 *
 *	Passive Picto Components (ie. Director, Proxy... not Workstation) send these 
 *	commands to the Server at a fixed interval.  These keep the network connection 
 *	from timing out and are used to send directives from Server to the Component.
 *
 *	FORMAT
 *		COMPONENTUPDATE name:idle/running PICTO.1/0
 *	RESPONSES
 *		The response is always of type 200 OK.  The first line of content 
 *		contains the directive, and the remaining lines may contain XML
 *		
 *		Legal Commands:
 *			OK, LOADEXP, START, STOP, PAUSE, RESTART
 *		
 */

struct ComponentUpdateCommandHandler : Picto::ProtocolCommandHandler
{
public:
	ComponentUpdateCommandHandler();

	QString method() { return QString("COMPONENTUPDATE"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
