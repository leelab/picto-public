#ifndef _DIRECTORLIST_COMMAND_HANDLER_H_
#define _DIRECTORLIST_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles the DIRECTORLIST commands
 *
 *	PictoWorkstation sends these commands in order to get a list of all the
 *	PictoDirector instances that are currently connected to the server.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct DirectorListCommandHandler : Picto::ProtocolCommandHandler
{
public:
	DirectorListCommandHandler();

	QString method() { return QString("DIRECTORLIST"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
