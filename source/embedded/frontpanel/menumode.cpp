
#include "MenuMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

MenuMode::MenuMode() :
currSelItem_(0),
currTopItem_(0)
{
	menuItems_.append(MenuItem(RewardDurModeType,"Set Reward Duration"));
	menuItems_.append(MenuItem(FlushDurModeType,"Set Flush Duration"));
	menuItems_.append(MenuItem(RewardControllerModeType,"Set Controller Num"));
	menuItems_.append(MenuItem(ChangeNameModeType,"Change Name"));
	menuItems_.append(MenuItem(StatusModeType,"Settings..."));
}

MenuMode::~MenuMode()
{
}

void MenuMode::initMode()
{
	drawMenu();
}

PanelInfo::DisplayModeType MenuMode::handleUserInput(PanelInfo::InputType type)
{
	switch(type)
	{
	case buttonPush:
		return menuItems_[currSelItem_].mode;
		break;
	case rotateLeft:
	case rotateLeftFast:
		if(currSelItem_ <= 0)
			break;
		currSelItem_--;
		if(currSelItem_ < currTopItem_)
			currTopItem_ = currSelItem_;
		break;
	case rotateRight:
	case rotateRightFast:
		if(currSelItem_ >= menuItems_.size()-1)
			break;
		currSelItem_++;
		if(currSelItem_ > currTopItem_+1)
			currTopItem_ = currSelItem_-1;
		break;
	default:
		return myModeType();
	}
	drawMenu();
	return PanelInfo::MenuModeType;
}

void MenuMode::drawMenu()
{
	drawMenuRows((currSelItem_-currTopItem_)%2,menuItems_[currTopItem_].text,menuItems_[currTopItem_+1].text);
}