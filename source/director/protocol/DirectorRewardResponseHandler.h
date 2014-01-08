#ifndef _DIRECTORREWARDRESPONSEHANDLER_H_
#define _DIRECTORREWARDRESPONSEHANDLER_H_

#include "../../common/protocol/RewardResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief Implements RewardResponseHandler to determine if a message over the network is intended to change
 *	the default reward time or to supply a reward, and which reward channel should be affected, then performs the 
 *	requested action.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct DirectorRewardResponseHandler : public Picto::RewardResponseHandler
{
public:
	DirectorRewardResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
