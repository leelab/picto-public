#ifndef _DIRECTORCLICKRESPONSEHANDLER_H_
#define _DIRECTORCLICKRESPONSEHANDLER_H_

#include "../../common/protocol/ClickResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief A response handler for REWARD responses
 */
struct  DirectorClickResponseHandler : public Picto::ClickResponseHandler
{
public:
	DirectorClickResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};

#endif
