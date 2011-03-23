#ifndef _DIRECTORERRORRESPONSEHANDLER_H_
#define _DIRECTORERRORRESPONSEHANDLER_H_

#include "../../common/protocol/ErrorResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief A response handler for ERROR responses
 */
struct DirectorErrorResponseHandler : public Picto::ErrorResponseHandler
{
public:
	DirectorErrorResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};

#endif
