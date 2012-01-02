#include "DirectorRewardResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorRewardResponseHandler::DirectorRewardResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
RewardResponseHandler(statusManager)
{}

bool DirectorRewardResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	int channelEndLoc = directive.indexOf("\n");
	int channel = directive.left(channelEndLoc).toInt();
	int quantity = directive.mid(channelEndLoc+1).toInt();
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	engine->giveReward(channel,quantity,50);
	return true;
}