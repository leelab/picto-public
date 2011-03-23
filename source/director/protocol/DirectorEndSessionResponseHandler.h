#ifndef _DIRECTORENDSESSIONRESPONSEHANDLER_H_
#define _DIRECTORENDSESSIONRESPONSEHANDLER_H_

#include "../../common/protocol/EndSessionResponseHandler.h"
#include "../../common/network/commandchannel.h"
#include "../network/DirectorStatusManager.h"

/*! \brief A response handler for ERROR responses
 */
struct DirectorEndSessionResponseHandler : public Picto::EndSessionResponseHandler
{
public:
	DirectorEndSessionResponseHandler(QSharedPointer<DirectorStatusManager> statusManager, QSharedPointer<Picto::CommandChannel> commandChannel);

protected:
	virtual bool processResponse(QString directive);

};

#endif
