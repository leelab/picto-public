#ifndef _DIRECTORSTOPRESPONSEHANDLER_H_
#define _DIRECTORSTOPRESPONSEHANDLER_H_

#include "../../common/protocol/StopResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief A response handler for STOP responses
 */
struct DirectorStopResponseHandler : public Picto::StopResponseHandler

{
public:
	DirectorStopResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
