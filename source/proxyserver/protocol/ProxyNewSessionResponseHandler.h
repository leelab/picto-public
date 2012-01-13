#ifndef _PROXYNEWSESSIONRESPONSEHANDLER_H_
#define _PROXYNEWSESSIONRESPONSEHANDLER_H_

#include "..\..\common\protocol\NewSessionResponseHandler.h"
#include "..\..\common\network\CommandChannel.h"
#include <QObject>

/*! \brief A response handler for START responses
 */
struct ProxyNewSessionResponseHandler : public Picto::NewSessionResponseHandler
{
public:
	ProxyNewSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<Picto::CommandChannel> dataCommandChannel);

protected:
	virtual bool processResponse(QString directive);
	QSharedPointer<Picto::CommandChannel> dataCommandChannel_;

};

#endif
