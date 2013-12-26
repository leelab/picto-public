#ifndef _REWARDRESPONSEHANDLER_H_
#define _REWARDRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A base class for ProtocolResponseHandler classes that respond to the "REWARD" method.
 *	\details A REWARD response indicates that a reward should be supplied to the Experiment subject.
 *	The message includes the reward channel on which the reward should be supplied and its duration.
 *	
 *	The processResponse() function implemented by this class is empty.  Extend this class
 *	to add logic that is triggered by the arrival of a new ProtocolRespose.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
