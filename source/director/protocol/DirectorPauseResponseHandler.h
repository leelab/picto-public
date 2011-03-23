#ifndef _DIRECTORPAUSERESPONSEHANDLER_H_
#define _DIRECTORPAUSERESPONSEHANDLER_H_

#include "../../common/protocol/PauseResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief A response handler for PAUSE responses
 */
struct DirectorPauseResponseHandler : public Picto::PauseResponseHandler
{
public:
	DirectorPauseResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
