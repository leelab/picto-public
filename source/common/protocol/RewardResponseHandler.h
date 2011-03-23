#ifndef _REWARDRESPONSEHANDLER_H_
#define _REWARDRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A response handler for REWARD responses
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API RewardResponseHandler : public ProtocolResponseHandler
#else
struct RewardResponseHandler
#endif
{
public:
	RewardResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	QString method(){return "REWARD";};
protected:
	virtual bool processResponse(QString directive);

};

}; //namespace Picto

#endif
