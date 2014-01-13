
#ifndef _TRIAL_COMMAND_HANDLER_H_
#define _TRIAL_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief THIS IS NO LONGER USED.  PICTO NO LONGER ENFORCES A TRIAL STRUCTURE IN EXPERIMENTS.  ORIGINAL DOCUMENTATION APPEARS BELOW FOR REFERENCE.
 *
 *	\details Handles the TRIAL commands, which are sent at the start/end of each trial
 *
 *	At the start/end of a trial (the running of a state machine of the "Trial" level) this
 *	command is sent from PictoDirector to PictoServer.  The collected timestamps are then
 *	used to align the behavioral data with the neural data.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct TrialCommandHandler : Picto::ProtocolCommandHandler
{
public:
	TrialCommandHandler();

	QString method() { return QString("TRIAL"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
