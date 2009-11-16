#include "FrontPanelInfo.h"

FrontPanelInfo::FrontPanelInfo() :
	rewardDuration(0),
	flushDuration(0),
	rewardController(1),
	dispMode(PanelInfo::MenuMode),
	block(1),
	trial(1),
	status(PanelInfo::disconnected)
{
	commandSocket = 0;
	eventSocket = 0;
}