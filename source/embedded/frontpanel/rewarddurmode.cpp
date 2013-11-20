
#include "RewardDurMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
/*!
 * Constructs a new RewardDurMode object, taking in a DirectorInterface as a parameter that will be
 * used to poll/push the reward duration from/to the director application as well as a panelInfo
 * pointer that will be used to poll the latest rewardController index which is necessary for
 * the DirectorInterface queries.
 */
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
/*! Returns the reward duration for the current reward controller as received from the 
* Director application.
*/
int RewardDurMode::getEditableNumber()
{
	return directorIf_->getRewardDuration(panelInfo_->getRewardController());
}
/*! Sets the latest updated reward duration to the director application
 * for the current reward controller through the DirectorInterface.
 */
bool RewardDurMode::setValueToDirector(int val)
{
	return directorIf_->setRewardDuration(val,panelInfo_->getRewardController());
}