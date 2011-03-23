#ifndef _PROXYLOADEXPRESPONSEHANDLER_H_
#define _PROXYLOADEXPRESPONSEHANDLER_H_

#include "..\..\common\protocol\LoadExpResponseHandler.h"

/*! \brief A response handler for LOADEXP responses
 */
struct ProxyLoadExpResponseHandler : public Picto::LoadExpResponseHandler
{
public:
	ProxyLoadExpResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};

#endif
