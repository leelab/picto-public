#ifndef _PROXYPAUSERESPONSEHANDLER_H_
#define _PROXYPAUSERESPONSEHANDLER_H_

#include "..\..\common\protocol\PauseResponseHandler.h"

/*! \brief Extends PauseResponseHandler but doesn't actually add anything.
 *	\details Maybe this will be a handy container to have some day :).
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct ProxyPauseResponseHandler : public Picto::PauseResponseHandler
{
public:
	ProxyPauseResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};

#endif
