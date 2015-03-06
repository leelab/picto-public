#ifndef _PROXYLOADEXPRESPONSEHANDLER_H_
#define _PROXYLOADEXPRESPONSEHANDLER_H_

#include "..\..\common\protocol\LoadExpResponseHandler.h"

/*! \brief Extends LoadExpResponseHandler but doesn't actually add anything.
 *	\details Maybe this will be a handy container to have some day :).
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct ProxyLoadExpResponseHandler : public Picto::LoadExpResponseHandler
{
public:
	ProxyLoadExpResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};

#endif
