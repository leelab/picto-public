#ifndef _PROXYRESUMERESPONSEHANDLER_H_
#define _PROXYRESUMERESPONSEHANDLER_H_

#include "..\..\common\protocol\ResumeResponseHandler.h"

/*! \brief A response handler for RESUME responses
 */
struct ProxyResumeResponseHandler : public Picto::ResumeResponseHandler
{
public:
	ProxyResumeResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};

#endif
