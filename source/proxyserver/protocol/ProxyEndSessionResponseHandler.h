#ifndef _PROXYENDSESSIONRESPONSEHANDLER_H_
#define _PROXYENDSESSIONRESPONSEHANDLER_H_

#include "..\..\common\protocol\EndSessionResponseHandler.h"
#include "..\..\common\network\commandchannel.h"

/*! \brief Extends EndSessionResponseHandler but does not actually add any functionality.
 *
 *	\details Maybe this will be a handy container to have someday though :).
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct ProxyEndSessionResponseHandler : public Picto::EndSessionResponseHandler
{
public:
	ProxyEndSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<Picto::CommandChannel> commandChannel);

protected:
	virtual bool processResponse(QString directive);

};

#endif
