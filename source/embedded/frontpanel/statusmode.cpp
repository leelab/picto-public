
#include "StatusMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

#define UPDATEINTERVALMS 3000

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

void StatusMode::initMode()
{
	updateStatus();
	updateTimer_.start();
}

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
	}
	drawStatus();
	return PanelInfo::StatusModeType;
}

void StatusMode::drawStatus()
{
	drawText(statusItems_[currTopItem_],statusItems_[currTopItem_+1]);
}

void StatusMode::updateStatus()
{
	statusItems_.clear();
	int controller = panelInfo_->getRewardController();
	QString name = directorIf_->getName();
	if(name.size() > 18)
		name.resize(18);
	QString statusChar = '\x1';
	switch(directorIf_->getStatus())
	{
	case DirUnknown:
		statusChar = '\x10';
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
		statusChar = '\xF';
		break;
	}

	QString statusStr = QString(19-name.size(),' ') + statusChar;
	statusItems_.append(name+statusStr);
	QString RewDur = "R:"+QString::number((directorIf_->getRewardDuration(controller)));
	QString flushDur = "F:"+QString::number((directorIf_->getFlushDuration(controller)));
	QString contStr = "C:"+QString::number(controller);
	int spacesLeft = 20-(RewDur.size()+flushDur.size()+contStr.size());
	QString dataStr =	RewDur+QString(spacesLeft/2,' ')
						+flushDur+QString(spacesLeft-spacesLeft/2,' ')
						+contStr;
	statusItems_.append(dataStr);
	statusItems_.append("IP:"+directorIf_->getIP());

	drawStatus();
}