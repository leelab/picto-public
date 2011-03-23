#ifndef _PROXYSTARTRESPONSEHANDLER_H_
#define _PROXYSTARTRESPONSEHANDLER_H_

#include "..\..\common\protocol\StartResponseHandler.h"
#include "..\..\common\network\CommandChannel.h"
#include <QObject>

/*! \brief A response handler for START responses
 */
struct ProxyStartResponseHandler : public Picto::StartResponseHandler
{
public:
	ProxyStartResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<Picto::CommandChannel> dataCommandChannel, QObject* acqPlugin);

protected:
	virtual bool processResponse(QString directive);
	QObject* acqPlugin_;
	QSharedPointer<Picto::CommandChannel> dataCommandChannel_;

};

#endif
