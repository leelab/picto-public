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

Menu::Menu(FrontPanelInfo *panelInfo) :
	panelInfo(panelInfo),
	currItem(0),
	cursorPos(1)
{
	panelInfo->setDispMode(PanelInfo::MenuMode);

	connectionTimer = new QTimer(this);
	connectionTimer->setInterval(1000);
	connect(connectionTimer, SIGNAL(timeout()), this, SLOT(checkConnections()));
	connectionTimer->start();

	flushingTimer = new QTimer(this);
	flushingTimer->setInterval(100);
	connect(flushingTimer,SIGNAL(timeout()), this, SLOT(drawFlush()));

	directorIf_ = new DirectorInterface(panelInfo);

}

Menu::~Menu()
{
	delete connectionTimer;
	delete flushingTimer;
	delete directorIf_;

	while(menuItems.size() > 0)
		menuItems.erase(menuItems.begin());
}

void Menu::initMenu()
{
	loadMenus();
	drawMenu();
}

//User input will be handled differently depeding on the current mode (eg
//in menu mode, rotating the knob shanges the menu, while in other modes,
//it might change the reward duration.)
void Menu::userInputSlot(int type)
{
	//--- Menu Mode ---
	if(panelInfo->getDispMode() == PanelInfo::MenuMode)
	{
		int nextItem,prevItem;
		switch(type)
		{
		case PanelInfo::rotateLeft:
			prevItem = prevMenuItem();
			if(prevItem >= 0)
				currItem = prevItem;
			if(cursorPos == 2)
				cursorPos = 1;
			drawMenu();
			break;
		case PanelInfo::rotateRight:
			nextItem = nextMenuItem();
			if(nextItem >= 0)
			{
				currItem = nextItem;
				if(cursorPos == 1)
					cursorPos = 2;
			}
			drawMenu();
			break;
		case PanelInfo::buttonPush:
			menuAction();
			break;
		default:
			break;
		}

	}
	//--- Status Mode ---
	else if(panelInfo->getDispMode() == PanelInfo::StatusMode)
	{
		//clicking exits to menu mode, everything else is ignored
		switch(type)
		{
		case PanelInfo::buttonPush:
			panelInfo->setDispMode(PanelInfo::MenuMode);
			drawMenu();
			break;
		case PanelInfo::rotateLeft:
		case PanelInfo::rotateRight:
		default:
			break;
		}
	}
	//--- Message Mode ---
	else if(panelInfo->getDispMode() == PanelInfo::MessageMode)
	{
		//any action exits message mode and returns to menu mode
		panelInfo->setDispMode(PanelInfo::MenuMode);
		drawMenu();
	}
	//--- WaitForEngine mode ---
	else if(panelInfo->getDispMode() == PanelInfo::WaitForEngine)
	{
		//any action results in this

		//check to see if we've connected to the engine yet
		QTcpSocket *commSock, *eventSock;
		commSock = panelInfo->getCommandSocket();
		eventSock = panelInfo->getEventSocket();

		if(commSock != NULL && eventSock != NULL)
		{
			panelInfo->setDispMode(PanelInfo::MenuMode);
			currItem = 0;
		}
	}
	//--- Change IP Mode ---
	else if(panelInfo->getDispMode() == PanelInfo::ChangeIPMode)
	{
		bool exit = false;
		bool drawLine2 = false;
		QString addr = panelInfo->getSystemIP().toString();
		QStringList ipFields = addr.split(".");

		switch(type)
		{
		case PanelInfo::rotateLeft:
			ipFields[currIpField] = QString("%1").arg(ipFields[currIpField].toInt()-1);
			if(ipFields[currIpField] == "99" || ipFields[currIpField] == "9")
				drawLine2 = true;
			break;
		case PanelInfo::rotateRight:
			ipFields[currIpField] = QString("%1").arg(ipFields[currIpField].toInt()+1);
			if(ipFields[currIpField] == "100" || ipFields[currIpField] == "10")
				drawLine2 = true;
			break;
		case PanelInfo::buttonPush:
			drawLine2 = true;
			if(currIpField<3)
				currIpField++;
			else
				exit = true;
			break;
		default:
			break;
		}
		if(ipFields[currIpField].toInt() > 255)
			ipFields[currIpField] = "255";
		if(ipFields[currIpField].toInt() < 0)
			ipFields[currIpField] = "0";


		if(exit)
		{
			panelInfo->setDispMode(PanelInfo::MenuMode); 
			drawMenu();

			//inform the engine of the change
			if(!directorIf_->setIP(addr))
			{
				doMessage("IP not updated,","try again");
			}
		}
		else
		{
			addr = QString("%1.%2.%3.%4").arg(ipFields[0]).
				arg(ipFields[1]).arg(ipFields[2]).arg(ipFields[3]);
			panelInfo->setSystemIP(QHostAddress(addr));
			drawChangeIP(drawLine2);
		}
	}
	//--- Change Name Mode ---
	else if(panelInfo->getDispMode() == PanelInfo::ChangeNameMode)
	{
		bool exit = false;
		bool drawLine2 = false;
		QString name = panelInfo->getSystemName();

		switch(type)
		{
		case PanelInfo::rotateLeft:
			if(name[currNameChar] == 'a')
				name[currNameChar] = 'Z';
			else if(name[currNameChar] == 'A')
				name[currNameChar] = '9';
			else if(name[currNameChar] == '0')
				name[currNameChar] = ' ';
			else if(name[currNameChar] == ' ')
				name[currNameChar] = 'z';
			else
				name[currNameChar]=name[currNameChar].toAscii()-1;
			break;
		case PanelInfo::rotateRight:
			if(name[currNameChar] == ' ')
				name[currNameChar] = '0';
			else if(name[currNameChar] == '9')
				name[currNameChar] = 'A';
			else if(name[currNameChar] == 'Z')
				name[currNameChar] = 'a';
			else if(name[currNameChar] == 'z')
				name[currNameChar] = ' ';
			else
				name[currNameChar]=name[currNameChar].toAscii()+1;
			break;
		case PanelInfo::buttonPush:
			drawLine2 = true;
			if(name[currNameChar] == ' ')
				exit = true;
			else if(currNameChar < name.size()-1)
				currNameChar++;
			else
				exit = true;
			break;
		default:
			break;
		}

		if(exit)
		{
			panelInfo->setDispMode(PanelInfo::MenuMode); 
			drawMenu();

			//remove everything from the first space on
			name.truncate(name.indexOf(' '));

			//confirm that the name isn't just an empty string
			if(name.isEmpty())
			{
				doMessage("Name must have at","least one character");
				return;
			}

			//inform the engine of the change
			if(!directorIf_->setName(name))
			{
				doMessage("Name not updated,","try again");
			}
		}
		else
		{
			panelInfo->setSystemName(name);
			drawChangeName(drawLine2);
		}
	}
	//--- RewardDurMode ---
	else if(panelInfo->getDispMode() == PanelInfo::RewardDurMode)
	{
		//NOTE: this design changes the reward duration upon exiting 
		//the reward duration screen
		int rewardDur = panelInfo->getRewardDuration();
		bool exit = false;

		switch(type)
		{
		case PanelInfo::rotateLeft:
			rewardDur -= 10;
			break;
		case PanelInfo::rotateRight:
			rewardDur += 10;
			break;
		case PanelInfo::buttonPush:
			exit = true;
			break;
		default:
			break;
		}

		if(rewardDur<10)
			rewardDur = 10;

		if(exit)
		{
			panelInfo->setDispMode(PanelInfo::MenuMode); 
			drawMenu();

			//inform the engine of the change
			if(!directorIf_->setRewardDuration(rewardDur,panelInfo->getRewardController()))
			{
				doMessage("Duration not updated,","try again");
			}
		}
		else
		{
			panelInfo->setRewardDuration(rewardDur);
			drawRewardDuration(false);
		}

	}
	//--- Reward Controller mode ---
	//User selection of active reward controller
	else if(panelInfo->getDispMode() == PanelInfo::RewardControllerMode)
	{
		bool exit = false;
		int controller = panelInfo->getRewardController();

		switch(type)
		{
		case PanelInfo::rotateLeft:
			controller--;
			break;
		case PanelInfo::rotateRight:
			controller++;
			break;
		case PanelInfo::buttonPush:
			exit = true;
			break;
		default:
			break;
		}

		if(exit)
		{
			panelInfo->setDispMode(PanelInfo::MenuMode); 
			drawMenu();
		}
		else
		{
			//the number of reward controllers is limited by the PictoBox hardware
			//since this code only runs on the PictoBox, we can safely limit
			//this here
			if(controller<1)
				controller = 1;
			if(controller>4)
				controller = 4;

			panelInfo->setRewardController(controller);
			drawChangeController();
		}
	}
	//--- flush duration mode ---
	else if(panelInfo->getDispMode() == PanelInfo::FlushDurMode)
	{
		int flushDur = panelInfo->getFlushDuration();
		bool exit = false;

		switch(type)
		{
		case PanelInfo::rotateLeft:
			flushDur -= 10;
			break;
		case PanelInfo::rotateRight:
			flushDur += 10;
			break;
		case PanelInfo::buttonPush:
			exit = true;
			break;
		default:
			break;
		}
		
		if(flushDur <10)
			flushDur = 10;

		if(exit)
		{
			panelInfo->setDispMode(PanelInfo::MenuMode); 
			drawMenu();

			//inform the engine of the change
			if(!directorIf_->setFlushDuration(flushDur,panelInfo->getRewardController()))
			{
				doMessage("Duration not updated,","try again");
			}
		}
		else
		{
			panelInfo->setFlushDuration(flushDur);
			drawFlushDuration(false);
		}

	}
	//--- flushing mode ---
	else if(panelInfo->getDispMode() == PanelInfo::FlushingMode)
	{
		bool stopFlushing = false;
		switch(type)
		{
		//cancel flushing if the button is pushed
		case PanelInfo::buttonPush:
			stopFlushing = true;
			break;
		default:
			break;
		}	

		if(stopFlushing)
		{
			flushingTimer->stop();

			//send out a stop flushing command
			QTcpSocket *commSock = panelInfo->getCommandSocket();
			int controller = panelInfo->getRewardController();
			if(!directorIf_->stopFlush(panelInfo->getRewardController()))
			{
				doMessage("Flush not stopped,","try again");
			}

			panelInfo->setDispMode(PanelInfo::MenuMode);
			emit turnOnBacklight();
			drawMenu();
		}
			
	}

	return;
}

//Menu action is called whenever a use clicks the button while
//we're in menu mode.   The function checks the currently highlighted
//menuItem, and executes code based on that item.
void Menu::menuAction()
{
	QTextStream out(stdout);

	//Main menu Items
	if(menuItems[currItem].menu == 0)
	{
		if(menuItems[currItem].name == "Settings...")
		{
			currItem = menuItems.indexOf(MenuItem("Set IP", 1));
			cursorPos = 1;
			drawMenu();
		}
		else if(menuItems[currItem].name == "Reward Controls...")
		{
			currItem = menuItems.indexOf(MenuItem("Give Reward", 2));
			cursorPos = 1;
			drawMenu();
		}
		else if(menuItems[currItem].name == "Status")
		{
			panelInfo->setDispMode(PanelInfo::StatusMode);
			drawStatus();
		}
	}
	//Settings items
	else if(menuItems[currItem].menu == 1)
	{
		if(menuItems[currItem].name == "Set IP")
		{
			initChangeIP();
		}
		else if(menuItems[currItem].name == "PictoBox name")
		{
			initChangeName();
		}
		else if(menuItems[currItem].name == "Exit")
		{
			currItem = menuItems.indexOf(MenuItem("Settings...",0));
			cursorPos = 1;
			drawMenu();
		}
	}
	//Reward controller items
	else if(menuItems[currItem].menu == 2)
	{
		//NOTE: No indication is given through the UI that a reward has been issued.
		//In the Lee lab, this is not a problem, since the reward solenoids are loud.
		//However, it might be desirable to give some sort of indication of a reward
		//This could be done using the "reward trigger" light on the front panel.
		if(menuItems[currItem].name == "Give Reward")
		{
			//tell the engine to give a reward
			if(!directorIf_->startReward(panelInfo->getRewardController()))
			{
				doMessage("Reward not given,","try again");
			}
		}
		else if(menuItems[currItem].name == "Flush...")
		{
			currItem = menuItems.indexOf(MenuItem("Start Flush", 3));
			cursorPos = 1;
			drawMenu();
		}
		else if(menuItems[currItem].name == "Reward Duration")
		{
			initRewardDuration();
		}
		else if(menuItems[currItem].name == "Change Controller")
		{
			panelInfo->setDispMode(PanelInfo::RewardControllerMode);
			drawChangeController();
		}
		else if(menuItems[currItem].name == "Exit")
		{
			currItem = menuItems.indexOf(MenuItem("Reward Controls...",0));
			cursorPos = 2;
			drawMenu();
		}

	}
	//Flushing Menu Items
	else if(menuItems[currItem].menu == 3)
	{
		if(menuItems[currItem].name == "Start Flush")
		{
			if(panelInfo->getSystemStatus() == PanelInfo::running)
				doMessage("Flushing failed.", "Experiment running.");
			else
				initFlush();
		}
		else if(menuItems[currItem].name == "Set Flush Duration")
		{
			initFlushDuration();
		}
		else if(menuItems[currItem].name == "Exit")
		{
			currItem = menuItems.indexOf(MenuItem("Flush...",2));
			cursorPos = 1;
			drawMenu();
		}
	}
}


void Menu::loadMenus()
{
	MenuItem *newItem;

	//Main menu (menu = 0)
	newItem = new MenuItem("Status",0);
	menuItems.push_back(*newItem);
	newItem = new MenuItem("Settings...", 0);
	menuItems.push_back(*newItem);
	newItem = new MenuItem("Reward Controls...",0);
	menuItems.push_back(*newItem);

	//Settings submenu (menu = 1)
	newItem = new MenuItem("Set IP", 1);
	menuItems.push_back(*newItem);
	newItem = new MenuItem("PictoBox name", 1);
	menuItems.push_back(*newItem);
	newItem = new MenuItem("Exit", 1);
	menuItems.push_back(*newItem);

	//Reward Controls Submenu (menu = 2)
	newItem = new MenuItem("Give Reward", 2);
	menuItems.push_back(*newItem);
	newItem = new MenuItem("Flush...", 2);
	menuItems.push_back(*newItem);
	newItem = new MenuItem("Reward Duration", 2);
	menuItems.push_back(*newItem);
	newItem = new MenuItem("Change Controller", 2);
	menuItems.push_back(*newItem);
	newItem = new MenuItem("Exit", 2);
	menuItems.push_back(*newItem);

	//Flushing sub-sub menu (menu = 3)
	newItem = new MenuItem("Start Flush", 3);
	menuItems.push_back(*newItem);
	newItem = new MenuItem("Set Flush Duration", 3);
	menuItems.push_back(*newItem);
	newItem = new MenuItem("Exit", 3);
	menuItems.push_back(*newItem);




}

void Menu::drawMenu()
{
	//\x09 is the 2nd custom character, which is set to an arrow character
	//(see phidgets.cpp)
	QString line1;
	QString line2;

	int nextItem, prevItem;

	if(cursorPos == 1)
	{
		line1 = '\x09'+menuItems[currItem].name;
		line2 = "";

		//find the next menu item in this menu
		nextItem = nextMenuItem();
		if(nextItem>=0)
			line2 = " "+menuItems[nextItem].name;
	}
	else if(cursorPos == 2)
	{
		line2 = '\x09'+menuItems[currItem].name;
		line1 = "";

		//find the previous menu item in this menu
		prevItem = prevMenuItem();
		if(prevItem>=0)
			line1 = " "+menuItems[prevItem].name;
	}

	emit updateLCD(1,line1);
	emit updateLCD(2,line2);

}

void Menu::doMessage(QString line1, QString line2)
{
	panelInfo->setDispMode(PanelInfo::MessageMode);

	emit updateLCD(1,line1);
	emit updateLCD(2,line2);
	return;
}

void Menu::drawStatus()
{
	QTextStream out(stdout);


	int block = panelInfo->getBlock();
	int trial = panelInfo->getTrial();
	PanelInfo::SystemStatus status = panelInfo->getSystemStatus();
	QString lastEvent = panelInfo->getLastEvent();

	QString line1= QString("Blk:%1 Trial:%2").arg(block).arg(trial);
	line1 = line1.leftJustified(19,' ',true);
	if(status == PanelInfo::disconnected)
		line1 += '\xB';	//frown
	else if(status == PanelInfo::connected)
		line1 += '\xC';	//neutral
	else if(status == PanelInfo::running)
		line1 += '\xD';	//smile
	else
		line1 += '\xB';

	QString line2 = lastEvent;
	line2 = line2.leftJustified(20,' ',true);

	emit updateLCD(1,line1);
	emit updateLCD(2,line2);
}

//Draws the screen for setting the reward duration
void Menu::drawRewardDuration(bool firstTime)
{
	int controller = panelInfo->getRewardController();
	int rewardDur = panelInfo->getRewardDuration();
	
	QString line2 = QString("   %1 \x08").arg(rewardDur);
	emit updateLCD(2,line2);
	if(firstTime)
	{
		QString line1 = QString("Reward %1 Duration:").arg(controller);
		emit updateLCD(1,line1);
	}
}

//Sets everything up for the reward duration mode
void Menu::initRewardDuration()
{
	int rewardDur = directorIf_->getRewardDuration(panelInfo->getRewardController());
	if(rewardDur < 0)
	{
		doMessage("Duration not recv'd","from engine");
		return;
	}
	panelInfo->setRewardDuration(rewardDur);
	panelInfo->setDispMode(PanelInfo::RewardDurMode);
	drawRewardDuration(true);
	return;
}

//This is the code used to draw the controller changing panel
void Menu::drawChangeController()
{
	int controller = panelInfo->getRewardController();
	
	QString line1 = "Current Controller:";
	QString line2 = QString(" %1").arg(controller);

	emit updateLCD(1,line1);
	emit updateLCD(2,line2);

}

//Sets everything up for the flush duration mode
void Menu::initFlushDuration()
{
	int flushDur = directorIf_->getFlushDuration(panelInfo->getRewardController());
	if(flushDur<0)
	{
		doMessage("Flush dur not recv'd","from engine");
		return;
	}
	panelInfo->setFlushDuration(flushDur);
	panelInfo->setDispMode(PanelInfo::FlushDurMode);
	drawFlushDuration(true);
	return;
}

//This is the code used to draw the controller changing panel
void Menu::drawFlushDuration(bool firstTime)
{
	int controller = panelInfo->getRewardController();
	int duration = panelInfo->getFlushDuration();
	
	if(firstTime)
	{
		QString line1 = QString("Flush %1 Duration:").arg(controller);
		emit updateLCD(1,line1);
	}
	QString line2 = QString("   %1 secs").arg(duration);
	emit updateLCD(2,line2);

}

void Menu::initFlush()
{
	//start the timer
	flushingTimer->start();

	if(!directorIf_->startFlush(panelInfo->getRewardController()))
	{
		doMessage("Flushing failed,","Try again");
	}	

	panelInfo->setDispMode(PanelInfo::FlushingMode);

	//drawFlush only draws the second line of the LCD,
	//so we have to draw the first line here.  (We can't use the "firstTime"
	//bool like with the other draw methods, since this one is a slot, and it
	//needs to be connected to a signal with no arguments).
	QString line1 = "Flushing.";
	emit updateLCD(1,line1);
}

void Menu::drawFlush()
{
	flushingTimer->stop();
	//get the time remaining in the flush
	int timeRem = directorIf_->getFlushTimeRemaining(panelInfo->getRewardController());
	if(timeRem > 0)
	{
		emit toggleBacklight();
		QString line2 = QString("  %1 s").arg(timeRem);
		emit updateLCD(2,line2);
		flushingTimer->start();
	}
	else
	{
		//printf("endFlush\n");
		emit turnOnBacklight();
		panelInfo->setDispMode(PanelInfo::MenuMode);
		drawMenu();
	}
}

void Menu::initChangeIP()
{
	QString ipString = directorIf_->getIP();
	QHostAddress addr;
	if(!ipString.isEmpty() && addr.setAddress(ipString))
	{
		panelInfo->setSystemIP(addr);
	}
	else
	{
		doMessage("IP not recv'd","from engine");
		return;
	}
	currIpField=0;
	panelInfo->setDispMode(PanelInfo::ChangeIPMode);
	drawChangeIP(true);
	return;
}

void Menu::drawChangeIP(bool drawLine2)
{
	QString addr = panelInfo->getSystemIP().toString();
	QString line1 = QString("IP: %1").arg(addr);

	emit updateLCD(1,line1);


	if(drawLine2)
	{
		QStringList addrFields = addr.split(".");
		if(addrFields.size() != 4)
			doMessage("IP Adress invalid","");

		int numSpaces;
		numSpaces = 4; //for "IP: "
		for(int i=0; i<currIpField; i++)
		{
			numSpaces++; //for trailing dot
			numSpaces += addrFields[i].length(); //for digits
		}

		QString line2 = QString(numSpaces,' ');
		line2 += QString(addrFields[currIpField].length(), '\xA');
		emit updateLCD(2,line2);
	}
}

void Menu::initChangeName()
{
	QString name = directorIf_->getName();
	if(name.isEmpty())
	{
		doMessage("Name not recv'd","from engine");
		return;
	}
	//add spaces to the system name
	//20 chars in an LCD line - sizeof("Name:") = 15 chars left to use
	name.append(QString(15-name.size(),' '));  
	panelInfo->setSystemName(name);
	currNameChar=0;
	panelInfo->setDispMode(PanelInfo::ChangeNameMode);
	drawChangeName(true);
	return;
}

void Menu::drawChangeName(bool drawLine2)
{
	QString name = panelInfo->getSystemName();

	QString line1 = QString("Name:%1").arg(name);
	emit updateLCD(1,line1);

	if(drawLine2)
	{
		int numSpaces;
		numSpaces = 5; //for "Name:"
		numSpaces += currNameChar;

		QString line2 = QString(numSpaces,' ');
		line2 += QString(1, '\xA');

		emit updateLCD(2,line2);
	}

}


//Returns the next item in the current menu
//returns -1 if there is no next item
int Menu::nextMenuItem()
{
	for(int x=currItem+1; x<menuItems.size(); x++)
		{
			if(menuItems[x].menu == menuItems[currItem].menu)
			{
				return x;
			}
		}
	return -1;
}
//Returns the previous item in the current menu
int Menu::prevMenuItem()
{	
	for(int x=currItem-1; x>=0; x--)
		{
			if(menuItems[x].menu == menuItems[currItem].menu)
			{
				return x;
			}
		}
	return -1;

}

void Menu::updateStatus()
{
	if(panelInfo->getDispMode() == PanelInfo::StatusMode)
		drawStatus();
}

void Menu::checkConnections()
{
	if(!directorIf_->isConnected())
	{
		QString line1 = "No engine connection\n";
		QString line2 = "";

		emit updateLCD(1, line1);
		emit updateLCD(2, line2);

		panelInfo->setDispMode(PanelInfo::WaitForEngine);
		panelInfo->setSystemStatus(PanelInfo::disconnected);
	}
	//coming out of wait for engine, we return to the home menu
	else if(panelInfo->getDispMode() == PanelInfo::WaitForEngine)
	{
		panelInfo->setDispMode(PanelInfo::MenuMode);
		panelInfo->setSystemStatus(PanelInfo::connected);
		currItem=0;
		drawMenu();
	}
	//otherwise we are happy, and do nothing.
	else
	{
	}
}
