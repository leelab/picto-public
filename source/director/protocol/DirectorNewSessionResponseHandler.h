#ifndef _DIRECTORNEWSESSIONRESPONSEHANDLER_H_
#define _DIRECTORNEWSESSIONRESPONSEHANDLER_H_

#include "../../common/protocol/NewSessionResponseHandler.h"
#include "../../common/network/commandchannel.h"
#include "../network/DirectorStatusManager.h"

/*! \brief A response handler for ERROR responses
 */
struct DirectorNewSessionResponseHandler : public Picto::NewSessionResponseHandler
{
public:
	DirectorNewSessionResponseHandler(QSharedPointer<DirectorStatusManager> statusManager, QSharedPointer<Picto::CommandChannel> commandChannel);

protected:
	virtual bool processResponse(QString directive);

};

#endif
