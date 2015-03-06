#ifndef _PROXYNEWSESSIONRESPONSEHANDLER_H_
#define _PROXYNEWSESSIONRESPONSEHANDLER_H_

#include "..\..\common\protocol\NewSessionResponseHandler.h"
#include "..\..\common\network\CommandChannel.h"
#include <QObject>

/*! \brief Extends NewSessionResponseHandler to gather Neural data and send it to the Picto Server until
 *	the Session ends or the Proxy application is closed.
 *	\details This class really only extends the processResponse() function.  See that functions documentation for more details.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
