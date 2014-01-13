#ifndef _STARTSESSION_COMMAND_HANDLER_H_
#define _STARTSESSION_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles STARTSESSION commands
 *
 *	The PictoWorkstation sends STARTSESSION commands when it wants a remote Director
 *	instance to load an experiment.  The command causes a STARTSESSION directive 
 *	to be included the next time that Director and Proxy instances send COMPONENTUPDATE
 *	commands and causes the Session design XML to be sent to the Director for 
 *	the experimental run.		
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct StartsessionCommandHandler : Picto::ProtocolCommandHandler
{
public:
	StartsessionCommandHandler();

	QString method() { return QString("STARTSESSION"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
private:
	QString extractExperimentFromDesign(QString designXML);
};


#endif
