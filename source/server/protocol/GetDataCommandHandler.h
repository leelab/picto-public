#ifndef _GETDATA_COMMAND_HANDLER_H_
#define _GETDATA_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles GETDATA commands, which are sent by the Workstation to request up to date Session data.
 *
 *	\details PictoWorkstation sends these commands to the Server to request behavioral, neural and overall Session data. 
 *  The target of the command is the type of data requested followed by a colon and the time or index of the data last received.
 *
 *	FORMAT
 *		GETDATA datastoretype:index(e.g. CurrentState:3.245, LatestNeural:23341, etc.) PICTO.1/0
 *		Session-ID:{44dcb670-4bea-11df-9879-0800200c9a66}
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct GetDataCommandHandler : Picto::ProtocolCommandHandler
{
public:
	GetDataCommandHandler();

	QString method() { return QString("GETDATA"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
