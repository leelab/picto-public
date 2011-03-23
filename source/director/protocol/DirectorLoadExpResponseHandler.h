#ifndef _DIRECTORLOADEXPRESPONSEHANDLER_H_
#define _DIRECTORLOADEXPRESPONSEHANDLER_H_

#include "../../common/protocol/LoadExpResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief A response handler for LOADEXP responses
 */
struct DirectorLoadExpResponseHandler : public Picto::LoadExpResponseHandler
{
public:
	DirectorLoadExpResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
