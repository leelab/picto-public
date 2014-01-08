#include "DirectorRewardResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorRewardResponseHandler::DirectorRewardResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
RewardResponseHandler(statusManager)
{}

/*! \brief Implements RewardResponseHandler::processResponse() to determine if the directive is a request to change
 *	the default reward time or that a reward be supplied, and which reward channel should be affected.  Then performs the 
 *	requested action.
 */
bool DirectorRewardResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	int channelEndLoc = directive.indexOf("\n");
	int channel = directive.left(channelEndLoc).toInt();
	if(directive.length() == channelEndLoc+1)
	{
		QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
		engine->giveReward(channel);
	}
	else
	{
		int quantity = directive.mid(channelEndLoc+1).toInt();
		QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
		engine->setRewardDuration(channel,quantity);
	}
	return true;
}