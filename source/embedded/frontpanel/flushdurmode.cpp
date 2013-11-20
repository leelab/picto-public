
#include "FlushDurMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
/*!
 * Constructs a new FlushDurMode object, taking in a DirectorInterface as a parameter that will be
 * used to poll/push the flush duration from/to the director application as well as a panelInfo
 * pointer that will be used to poll the latest rewardController index which is necessary for
 * the DirectorInterface queries.
 */
FlushDurMode::FlushDurMode(QSharedPointer<DirectorInterface> directorIf,FrontPanelInfo *panelInfo) :
NumberEditMode(999,1,"Flush","s",10,1),
directorIf_(directorIf),
panelInfo_(panelInfo)
{
	Q_ASSERT(directorIf);
}

FlushDurMode::~FlushDurMode()
{
}
/*! Returns the flush duration for the current reward controller as received from the 
* Director application.
*/
int FlushDurMode::getEditableNumber()
{
	return directorIf_->getFlushDuration(panelInfo_->getRewardController());
}
/*! Sets the latest updated flush duration to the director application
 * for the current reward controller through the DirectorInterface.
 */
bool FlushDurMode::setValueToDirector(int val)
{
	return directorIf_->setFlushDuration(val,panelInfo_->getRewardController());
}