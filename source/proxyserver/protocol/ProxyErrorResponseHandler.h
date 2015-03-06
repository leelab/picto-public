#ifndef _PROXYERRORRESPONSEHANDLER_H_
#define _PROXYERRORRESPONSEHANDLER_H_

#include "..\..\common\protocol\ErrorResponseHandler.h"

/*! \brief Extends ErrorResponseHandler but doesn't actually add anything.
 *	\details Maybe this will be a handy container to have some day :).
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct ProxyErrorResponseHandler : public Picto::ErrorResponseHandler
{
public:
	ProxyErrorResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};
#endif
