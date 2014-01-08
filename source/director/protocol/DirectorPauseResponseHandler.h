#ifndef _DIRECTORPAUSERESPONSEHANDLER_H_
#define _DIRECTORPAUSERESPONSEHANDLER_H_

#include "../../common/protocol/PauseResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief Implements PauseResponseHandler to pause the current Task run when a pause request is
 *	received over the network.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct DirectorPauseResponseHandler : public Picto::PauseResponseHandler
{
public:
	DirectorPauseResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
