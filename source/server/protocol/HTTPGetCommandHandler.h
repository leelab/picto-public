#ifndef _HTTPGETCOMMANDHANDLER_H_
#define _HTTPGETCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*!	\brief Handles GET commands from the HTTP protocol.
 *
 *	This command handler was used during testing, but isn't really part of Picto at
 *	this point.  It responds to GET commands sent using HTTP protocol (instead of 
 *	PICTO).  Actually, if you are so inclined, PictoServer can be used as a web server.
 *	This isn't currently being done, but it could prove useful for certain applications.
 *	(e.g. streaming "video" of an ongoing experiment to a destination that doesn't have
 *	a copy of PictoWorkstation, and doesn't need to be able to control an experiment).
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct HTTPGetCommandHandler : Picto::ProtocolCommandHandler
{
public:
	HTTPGetCommandHandler();

	QString method() { return QString("GET"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};

/*! @} */

#endif
