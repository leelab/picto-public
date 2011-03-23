#ifndef _DIRECTORREWARDRESPONSEHANDLER_H_
#define _DIRECTORREWARDRESPONSEHANDLER_H_

#include "../../common/protocol/RewardResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief A response handler for REWARD responses
 */
struct DirectorRewardResponseHandler : public Picto::RewardResponseHandler
{
public:
	DirectorRewardResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
