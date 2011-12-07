#include "DirectorRewardResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorRewardResponseHandler::DirectorRewardResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
RewardResponseHandler(statusManager)
{}

bool DirectorRewardResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	int channel = directive.toInt();
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	engine->giveReward(channel);
	return true;
}