#ifndef _GETDATA_COMMAND_HANDLER_H_
#define _GETDATA_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles the GETDATA commands, which are sent by Director
 *
 *	PictoWorkstation sends these commands to the server to request that 
 *  behavioral data be sent.  The target of the command is the type of data
 *	requested followed by a colon and the time or index of the data last received.
 *
 *	FORMAT
 *		GETDATA datastoretype:index(e.g. CurrentState:3.245 or LatestNeural:23341) PICTO.1/0
 *		Session-ID:{44dcb670-4bea-11df-9879-0800200c9a66}
 *
 *	RESPONSES
 *
 *	The response to this command is always of type 200:OK, and the content
 *	will be an XML fragment containing requested data.
 */

struct GetDataCommandHandler : Picto::ProtocolCommandHandler
{
public:
	GetDataCommandHandler();

	QString method() { return QString("GETDATA"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
