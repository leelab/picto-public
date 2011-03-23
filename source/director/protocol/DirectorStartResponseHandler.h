#ifndef _DIRECTORSTARTRESPONSEHANDLER_H_
#define _DIRECTORSTARTRESPONSEHANDLER_H_

#include "../../common/protocol/StartResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief A response handler for START responses
 */
struct DirectorStartResponseHandler : public Picto::StartResponseHandler
{
public:
	DirectorStartResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
