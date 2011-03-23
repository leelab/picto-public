#ifndef _PROXYSTOPRESPONSEHANDLER_H_
#define _PROXYSTOPRESPONSEHANDLER_H_

#include "..\..\common\protocol\StopResponseHandler.h"

/*! \brief A response handler for STOP responses
 */
struct ProxyStopResponseHandler : public Picto::StopResponseHandler
{
public:
	ProxyStopResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};

#endif
