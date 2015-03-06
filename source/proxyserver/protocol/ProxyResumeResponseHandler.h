#ifndef _PROXYRESUMERESPONSEHANDLER_H_
#define _PROXYRESUMERESPONSEHANDLER_H_

#include "..\..\common\protocol\ResumeResponseHandler.h"

/*! \brief Extends ResumeResponseHandler but doesn't actually add anything.
 *	\details Maybe this will be a handy container to have some day :).
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct ProxyResumeResponseHandler : public Picto::ResumeResponseHandler
{
public:
	ProxyResumeResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};

#endif
