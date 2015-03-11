#include "../../common/globals.h"
#include "ControlPanelInterface.h"
#include "../memleakdetect.h"

ControlPanelInterface::ControlPanelInterface()
{
}

ControlPanelInterface::~ControlPanelInterface()
{
}
/*! \brief Call this to tell the control panel that the Director's reward duration changed on the input controller.
 *	\details Internally, this function calls rewardDurationWasChanged() with the input controller value.
 *	duration is the duration in milliseconds of the reward.
 */	
void ControlPanelInterface::rewardDurationChanged(int controller, int duration)
{
	while(rewardDurations_.size() <= controller)
	{
		rewardDurations_.append(-1);
	}
	rewardDurations_[controller] = duration;
	rewardDurationWasChanged(controller);
}
/*! \brief Call this to tell the control panel that the Director's flush duration changed on the input controller.
 *	\details Internally, this function calls flushDurationWasChanged() with the input controller value.
 *	duration is the duration in seconds of the flush.
 */	
void ControlPanelInterface::flushDurationChanged(int controller, int duration)
{
	while(flushDurations_.size() <= controller)
	{
		flushDurations_.append(-1);
	}
	flushDurations_[controller] = duration;
	flushDurationWasChanged(controller);
}

/*! \brief Returns the current reward duration of the Director on the input controller.  Returned value is in milliseconds.
 *	\details If the controller input is invalid, -1 is returned.
 */
int ControlPanelInterface::getRewardDuration(int controller)
{
	if(controller >= rewardDurations_.size())
		return -1;
	return rewardDurations_[controller];
}
/*! \brief Returns the current flush duration of the Director on the input controller. Returned value is in seconds.
 *	\details If the controller input is invalid, -1 is returned.
 */
int ControlPanelInterface::getFlushDuration(int controller)
{
	if(controller >= flushDurations_.size())
		return -1;
	return flushDurations_[controller];
}
