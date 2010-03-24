/*! \brief Handles the TRIAL commands, which are sent at the start/end of each trial
 *
 *	At the start/end of a trial (the running of a state machine of the "Trial" level) this
 *	command is sent from PictoDirector to PictoServer.  The collected timestamps are then
 *	used to align the behavioral data with the neural data.
 */

#ifndef _TRIAL_COMMAND_HANDLER_H_
#define _TRIAL_COMMAND_HANDLER_H_

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

#if defined WIN32 || defined WINCE
struct PICTOLIB_API TrialCommandHandler : ProtocolCommandHandler
#else
struct TrialCommandHandler : ProtocolCommandHandler
#endif
{
public:
	TrialCommandHandler();

	QString method() { return QString("TRIAL"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};

} //namespace Picto

#endif
