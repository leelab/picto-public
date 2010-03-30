/*! \brief Handles the DIRECTORLIST commands
 *
 *	PictoWorkstation sends these commands in order to get a list of all the
 *	PictoDirector instances that are currently connected to the server.
 *
 *	FORMAT
 *		DIRECTORLIST / PICTO.1/0
 *	RESPONSES
 *		The response is always of type 200 OK.  The content contains an
 *		XML fragment listing the connected director instances.
 *
 *		<DirectorInstances>
 *			<Director>
 *				<Address>192.168.3.51</Address>
 *				<Name>PictoDirector Room 408</Name>
 *				<Status>Idle</Status>
 *			</Director>
 *			<Director>
 *				<Address>192.168.3.164</Address>
 *				<Name>PictoDirector Room 407</Name>
 *				<Status>Running</Status>
 *			</Director>
 *		</DirectorInstances>
 *		
 */

#ifndef _DIRECTORLIST_COMMAND_HANDLER_H_
#define _DIRECTORLIST_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

struct DirectorListCommandHandler : Picto::ProtocolCommandHandler
{
public:
	DirectorListCommandHandler();

	QString method() { return QString("DIRECTORLIST"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
