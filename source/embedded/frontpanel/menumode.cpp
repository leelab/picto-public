
#include "MenuMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

/*! \brier Constructs a new MenuMode object
 * \details Sets up the menuItems_ list to with all of the DisplayMode items that
 * are selectable from this MenuMode
 */ 
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

/*!
 * Initializes this MenuMode by drawing the menu to the LCD display.
 */
void MenuMode::initMode()
{
	drawMenu();
}

/*! 
 * Handles user input by turning knob rotations into up/down movements through the DisplayMode menu,
 * and knob presses into DisplayMode selections.
 */
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

/*!
 * Draws the currently selected item and another one, either above or below, to the LCD display.
 */
void MenuMode::drawMenu()
{
	drawMenuRows((currSelItem_-currTopItem_)%2,menuItems_[currTopItem_].text,menuItems_[currTopItem_+1].text);
}