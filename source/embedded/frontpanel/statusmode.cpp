
#include "StatusMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

/*! \file
 * \def UPDATEINTERVALMS
 * \brief The interval at which the StatusMode DisplayMode repolls and redisplays status information
 */
#define UPDATEINTERVALMS 5000

StatusMode::StatusMode(QSharedPointer<DirectorInterface> directorIf,FrontPanelInfo *panelInfo) :
currTopItem_(0),
directorIf_(directorIf),
panelInfo_(panelInfo)
{
	updateTimer_.setInterval(UPDATEINTERVALMS);
	connect(&updateTimer_,SIGNAL(timeout()),this,SLOT(updateStatus()));
}

StatusMode::~StatusMode()
{
}
/*!
 * Initializes this MenuMode by drawing the status info to the LCD display and starting
 * an updateTimer to repoll and redisplay status data every UPDATEINTERVALMS.
 */
void StatusMode::initMode()
{
	updateStatus();
	updateTimer_.start();
}
/*! 
 * Handles user input by turning knob rotations into up/down movements within the StatusMode display,
 * and knob presses into commands to move to the MenuMode for other DisplayMode type selection.
 */
PanelInfo::DisplayModeType StatusMode::handleUserInput(PanelInfo::InputType type)
{
	switch(type)
	{
	case buttonPush:
		updateTimer_.stop();	//Stop the update timer so that it won't continue
								//updating the display when this isn't the active mode.
		return MenuModeType;
		break;
	case rotateLeftFast:
	case rotateLeft:
		if(currTopItem_ <= 0)
			break;
		currTopItem_--;
		break;
	case rotateRightFast:
	case rotateRight:
		if(currTopItem_ >= statusItems_.size()-2)
			break;
		currTopItem_++;
		break;
	default:
		return myModeType();
	}
	drawStatus();
	return PanelInfo::StatusModeType;
}
/*!
 * Draws the current status to the LCD display.
 */
void StatusMode::drawStatus()
{
	if(statusItems_.size() > (currTopItem_+1))
		drawText(statusItems_[currTopItem_],statusItems_[currTopItem_+1]);
}
/*!
 * Polls status information and displays it by calling drawStatus()
 */
void StatusMode::updateStatus()
{
	statusItems_.clear();
	int controller = panelInfo_->getRewardController();
	QString name = directorIf_->getName();
	if(name.size() > 18)
		name.resize(18);
	QString statusChar = '\xF';
	switch(directorIf_->getStatus())
	{
	case DirUnknown:
		statusChar = '\xF';
		break;
	case DirDisconnect:
		statusChar = '\xB';
		break;
	case DirConnect:
		statusChar = '\xC';
		break;
	case DirEnding:
		statusChar = '\xC';
		break;
	case DirStop:
		statusChar = '\xD';
		break;
	case DirRun:
		statusChar = '\xE';
		break;
	case DirPause:
		statusChar = '\xE';
		break;
	}

	QString statusStr = QString(19-name.size(),' ') + statusChar;
	statusItems_.append(name+statusStr);
	QString RewDur = "R:"+QString::number((directorIf_->getRewardDuration(controller)))+"\x8";
	QString flushDur = "F:"+QString::number((directorIf_->getFlushDuration(controller)))+"s";
	QString contStr = "C:"+QString::number(controller);
	int spacesLeft = 20-(RewDur.size()+flushDur.size()+contStr.size());
	QString dataStr =	RewDur+QString(spacesLeft/2,' ')
						+flushDur+QString(spacesLeft-spacesLeft/2,' ')
						+contStr;
	statusItems_.append(dataStr);
	statusItems_.append("IP:"+directorIf_->getIP());

	drawStatus();
}