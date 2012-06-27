
#include "RewardDurMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

RewardDurMode::RewardDurMode(QSharedPointer<DirectorInterface> directorIf,FrontPanelInfo *panelInfo) :
NumberEditMode(999,1,"Reward","\x8",10,1),
directorIf_(directorIf),
panelInfo_(panelInfo)
{
	Q_ASSERT(directorIf);
}

RewardDurMode::~RewardDurMode()
{
}

int RewardDurMode::getEditableNumber()
{
	return directorIf_->getRewardDuration(panelInfo_->getRewardController());
}

bool RewardDurMode::setValueToDirector(int val)
{
	return directorIf_->setRewardDuration(val,panelInfo_->getRewardController());
}