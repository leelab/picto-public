#include "../../common/globals.h"
#include "ControlPanelInterface.h"

ControlPanelInterface::ControlPanelInterface()
{
}

ControlPanelInterface::~ControlPanelInterface()
{
}
	
void ControlPanelInterface::rewardDurationChanged(int controller, int duration)
{
	while(rewardDurations_.size() <= controller)
	{
		rewardDurations_.append(-1);
	}
	rewardDurations_[controller] = duration;
	rewardDurationWasChanged(controller);
}

void ControlPanelInterface::flushDurationChanged(int controller, int duration)
{
	while(flushDurations_.size() <= controller)
	{
		flushDurations_.append(-1);
	}
	flushDurations_[controller] = duration;
	flushDurationWasChanged(controller);
}

int ControlPanelInterface::getRewardDuration(int controller)
{
	if(controller >= rewardDurations_.size())
		return -1;
	return rewardDurations_[controller];
}

int ControlPanelInterface::getFlushDuration(int controller)
{
	if(controller >= flushDurations_.size())
		return -1;
	return flushDurations_[controller];
}
