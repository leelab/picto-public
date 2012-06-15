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

Menu::Menu(FrontPanelInfo *panelInfo) :
	panelInfo(panelInfo),
	wasConnected_(false)
{
	connectionTimer = new QTimer(this);
	connectionTimer->setInterval(1000);
	connect(connectionTimer, SIGNAL(timeout()), this, SLOT(checkConnections()));
	connectionTimer->start();

	activityTimer = new QTimer(this);
	connect(activityTimer, SIGNAL(timeout()), this, SLOT(returnToStatus()));

	//flushingTimer = new QTimer(this);
	//flushingTimer->setInterval(100);
	//connect(flushingTimer,SIGNAL(timeout()), this, SLOT(drawFlush()));

	directorIf_ = QSharedPointer<DirectorInterface>(new DirectorInterface(panelInfo));

}

Menu::~Menu()
{
	delete connectionTimer;
	//delete flushingTimer;
}

void Menu::initMenu()
{
	loadMenus();
}

//User input will be handled differently depeding on the current mode (eg
//in menu mode, rotating the knob shanges the menu, while in other modes,
//it might change the reward duration.)
void Menu::userInputSlot(int type)
{
	activityTimer->stop();
	QSharedPointer<DisplayMode> newMode;
	newMode = panelModes_[currMode_->userInputSlot(PanelInfo::InputType(type))];
	if(newMode != currMode_)
		newMode->initMode();
	currMode_ = newMode;
	if(currMode_ != panelModes_[StatusModeType])
		activityTimer->start(currMode_->activityTimeoutMs());
}


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

	currMode_ = panelModes_[StatusModeType];
}

////This is the code used to draw the controller changing panel
//void Menu::drawFlushDuration(bool firstTime)
//{
//	int controller = panelInfo->getRewardController();
//	int duration = panelInfo->getFlushDuration();
//	
//	if(firstTime)
//	{
//		QString line1 = QString("Flush %1 Duration:").arg(controller);
//		emit updateLCD(1,line1);
//	}
//	QString line2 = QString("   %1 secs").arg(duration);
//	emit updateLCD(2,line2);
//
//}
//
//void Menu::initFlush()
//{
//	//start the timer
//	flushingTimer->start();
//
//	if(!directorIf_->startFlush(panelInfo->getRewardController()))
//	{
//		doMessage("Flushing failed,","Try again");
//	}	
//
//	panelInfo->setDispMode(PanelInfo::FlushingMode);
//
//	//drawFlush only draws the second line of the LCD,
//	//so we have to draw the first line here.  (We can't use the "firstTime"
//	//bool like with the other draw methods, since this one is a slot, and it
//	//needs to be connected to a signal with no arguments).
//	QString line1 = "Flushing.";
//	emit updateLCD(1,line1);
//}
//
//void Menu::drawFlush()
//{
//	flushingTimer->stop();
//	//get the time remaining in the flush
//	int timeRem = directorIf_->getFlushTimeRemaining(panelInfo->getRewardController());
//	if(timeRem > 0)
//	{
//		emit toggleBacklight();
//		QString line2 = QString("  %1 s").arg(timeRem);
//		emit updateLCD(2,line2);
//		flushingTimer->start();
//	}
//	else
//	{
//		//printf("endFlush\n");
//		emit turnOnBacklight();
//		panelInfo->setDispMode(PanelInfo::MenuModeType);
//		drawMenu();
//	}
//}

void Menu::checkConnections()
{
	if(!directorIf_->isConnected())
	{
		QString line1 = "No engine connection\n";
		QString line2 = "";

		emit updateLCD(1, line1);
		emit updateLCD(2, line2);

		wasConnected_ = true;
	}
	//coming out of wait for engine, we return to the status menu
	else if(!wasConnected_)
	{
		currMode_ = panelModes_[StatusModeType];
		currMode_->initMode();
		wasConnected_ = true;
	}
}

void Menu::returnToStatus()
{
	activityTimer->stop();
	currMode_ = panelModes_[StatusModeType];
	currMode_->initMode();
}
