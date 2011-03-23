#ifndef _DIRECTORRESUMERESPONSEHANDLER_H_
#define _DIRECTORRESUMERESPONSEHANDLER_H_

#include "../../common/protocol/ResumeResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief A response handler for RESUME responses
 */
struct DirectorResumeResponseHandler : public Picto::ResumeResponseHandler
{
public:
	DirectorResumeResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
