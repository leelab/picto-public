
#ifndef _TRIAL_COMMAND_HANDLER_H_
#define _TRIAL_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles the TRIAL commands, which are sent at the start/end of each trial
 *
 *	At the start/end of a trial (the running of a state machine of the "Trial" level) this
 *	command is sent from PictoDirector to PictoServer.  The collected timestamps are then
 *	used to align the behavioral data with the neural data.
 */
struct TrialCommandHandler : Picto::ProtocolCommandHandler
{
public:
	TrialCommandHandler();

	QString method() { return QString("TRIAL"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
