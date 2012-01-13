#ifndef _PROXYENDSESSIONRESPONSEHANDLER_H_
#define _PROXYENDSESSIONRESPONSEHANDLER_H_

#include "..\..\common\protocol\EndSessionResponseHandler.h"
#include "..\..\common\network\commandchannel.h"

/*! \brief A response handler for STOP responses
 */
struct ProxyEndSessionResponseHandler : public Picto::EndSessionResponseHandler
{
public:
	ProxyEndSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<Picto::CommandChannel> commandChannel);

protected:
	virtual bool processResponse(QString directive);

};

#endif
