
#include "RewardControllerMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

RewardControllerMode::RewardControllerMode(FrontPanelInfo *panelInfo) :
NumberEditMode(9,1,"Controller",1,1),
panelInfo_(panelInfo)
{
}

RewardControllerMode::~RewardControllerMode()
{
}

int RewardControllerMode::getEditableNumber()
{
	return panelInfo_->getRewardController();
}

bool RewardControllerMode::setValueToDirector(int val)
{
	panelInfo_->setRewardController(val);
	return true;
}