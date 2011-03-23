#ifndef _PROXYPAUSERESPONSEHANDLER_H_
#define _PROXYPAUSERESPONSEHANDLER_H_

#include "..\..\common\protocol\PauseResponseHandler.h"

/*! \brief A response handler for PAUSE responses
 */
struct ProxyPauseResponseHandler : public Picto::PauseResponseHandler
{
public:
	ProxyPauseResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};

#endif
