#include "DirectorRewardResponseHandler.h"
using namespace Picto;

DirectorRewardResponseHandler::DirectorRewardResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
RewardResponseHandler(statusManager)
{}

bool DirectorRewardResponseHandler::processResponse(QString directive)
{
	int channel = directive.toInt();
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.staticCast<DirectorStatusManager>()->getEngine();
	engine->giveReward(channel);
	return true;
}