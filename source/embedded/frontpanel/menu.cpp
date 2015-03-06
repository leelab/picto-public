#include <QTextStream>
#include <QStringList>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QTcpSocket>
#include <QHostAddress>
#include <QApplication>

#include "menu.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "ChangeNameMode.h"
#include "FlushDurMode.h"
#include "MenuMode.h"
#include "RewardControllerMode.h"
#include "RewardDurMode.h"
#include "StatusMode.h"

/*! \brief Constructs a new Menu object, taking in a panelInfo object that provides access to the Socket connecting to the Director application.
 * \details Initializes various objects within the Menu object.  This includes an activityTimer 
 * that allows a limited amount of time without user interaction before any non-status
 * DisplayModes are aborted and the active DisplayMode returns to the StatusMode.  The DirectorInterface 
 * is also set up here to use the Director Socket from the input panelInfo to control all communication with 
 * the director.
 */
Menu::Menu(FrontPanelInfo *panelInfo) :
	panelInfo(panelInfo),
	wasConnected_(false)
{
	activityTimer = new QTimer(this);
	connect(activityTimer, SIGNAL(timeout()), this, SLOT(returnToStatus()));

	directorIf_ = QSharedPointer<DirectorInterface>(new DirectorInterface(panelInfo));

}

Menu::~Menu()
{
}

/*! \brief Initializes this object by creating all DisplayMode objects, turning on the LCD
 * backlight and setting the active mode to StatusMode.
 */
void Menu::initMenu()
{
	loadMenus();
	returnToStatus();
	emit turnOnBacklight();
}

/*! \brief Accepts user input from the phidgets interface and routes it to the active DisplayMode or appropriate command.
 * \details User input is handled differently depending on the current mode (eg in menu mode, 
 * rotating the knob changes the menu selection, while in RewardDurMode modes, it changes the 
 * reward duration.) Reward and Flush buttons on the other hand always have the same effect of 
 * starting a reward or starting/stopping a flush regardless of the current DisplayMode.  This
 * function routes all of this user inputs accordingly.
 */
void Menu::userInputSlot(int type)
{
	if((type == PanelInfo::rewardButton) || (type == PanelInfo::flushButton))
	{	//reward and flush should always be honored regardless of the current menu state
		if(type == PanelInfo::rewardButton)
		{
			directorIf_->startReward(panelInfo->getRewardController());
		}
		else
		{
			directorIf_->flush(panelInfo->getRewardController());
		}
		return;
	}
	//Stop the activity timer, since something happened.
	activityTimer->stop();
	QSharedPointer<DisplayMode> newMode;
	//Route the user input into the current DisplayMode, get the next mode that should
	//be activated and initialize it if it is not the same as the current one.
	newMode = panelModes_[currMode_->userInputSlot(PanelInfo::InputType(type))];
	if(newMode != currMode_)
		newMode->initMode();
	currMode_ = newMode;
	//Restart the activity timer that brings control back to the StatusMode after a set time
	//has passed without user interaction.  Since user interaction just happened, this is the
	//time to start.
	if(currMode_ != panelModes_[StatusModeType])
		activityTimer->start(currMode_->activityTimeoutMs());
}

/*! \brief Deinitializes this application just before it closes, essentially by turning off
 * the LCD display
 */
void Menu::aboutToQuit()
{
	directorIf_.clear();
	emit turnOffBacklight();
	emit updateLCD(1,"");
	emit updateLCD(2,"");
}

/*! \brief Creates and sets up all of the DisplayMode objects that the user can use in this panel.
 */
void Menu::loadMenus()
{
	panelModes_[PanelInfo::MenuModeType] = QSharedPointer<DisplayMode>(new MenuMode());
	panelModes_[PanelInfo::StatusModeType] = QSharedPointer<DisplayMode>(new StatusMode(directorIf_,panelInfo));
	panelModes_[PanelInfo::RewardDurModeType] = QSharedPointer<DisplayMode>(new RewardDurMode(directorIf_,panelInfo));
	panelModes_[PanelInfo::RewardControllerModeType] = QSharedPointer<DisplayMode>(new RewardControllerMode(panelInfo));
	panelModes_[PanelInfo::FlushDurModeType] = QSharedPointer<DisplayMode>(new FlushDurMode(directorIf_,panelInfo));
	panelModes_[PanelInfo::ChangeNameModeType] = QSharedPointer<DisplayMode>(new ChangeNameMode(directorIf_));

	foreach(QSharedPointer<DisplayMode> mode,panelModes_)
	{
		connect(mode.data(),SIGNAL(updateLCD(int,QString)),this,SIGNAL(updateLCD(int,QString)));
		connect(mode.data(),SIGNAL(toggleBacklight()),this,SIGNAL(toggleBacklight()));
		connect(mode.data(),SIGNAL(turnOnBacklight()),this,SIGNAL(turnOnBacklight()));
	}

	//The first DisplayMode should be the StatusMode
	currMode_ = panelModes_[StatusModeType];
}

/*! \brief Returns the current DisplayMode to the StatusMode.
 * \details This function is called after no user interaction occurs for a set period of time
 * to return the LCD display output to its default StatusMode state.
 */
void Menu::returnToStatus()
{
	activityTimer->stop();
	currMode_ = panelModes_[StatusModeType];
	currMode_->initMode();
}
