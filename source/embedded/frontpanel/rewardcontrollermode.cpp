
#include "RewardControllerMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
/*!
 * Constructs a new RewardControllerMode object, taking in a panelInfo
 * pointer that will be used to get/set the current rewardController index.
 */
RewardControllerMode::RewardControllerMode(FrontPanelInfo *panelInfo) :
NumberEditMode(1,1,"Controller","",1,1),
panelInfo_(panelInfo)
{
}

RewardControllerMode::~RewardControllerMode()
{
}
/*! Returns the current reward controller number as received from the 
* Panel Info object.
*/
int RewardControllerMode::getEditableNumber()
{
	return panelInfo_->getRewardController();
}
/*! Sets the latest reward controller index to the Panel Info object.
 */
bool RewardControllerMode::setValueToDirector(int val)
{
	panelInfo_->setRewardController(val);
	return true;
}