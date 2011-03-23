#ifndef _PROXYERRORRESPONSEHANDLER_H_
#define _PROXYERRORRESPONSEHANDLER_H_

#include "..\..\common\protocol\ErrorResponseHandler.h"

/*! \brief A response handler for ERROR responses
 */
struct ProxyErrorResponseHandler : public Picto::ErrorResponseHandler
{
public:
	ProxyErrorResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};
#endif
