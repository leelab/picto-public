#include <QTextStream>
#include <QStringList>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QTcpSocket>
#include <QHostAddress>

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

}

Menu::~Menu()
{
	delete connectionTimer;
	delete flushingTimer;

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
		QString addr = panelInfo->getSystemIP().toString();
		QStringList ipFields = addr.split(".");

		switch(type)
		{
		case PanelInfo::rotateLeft:
			ipFields[currIpField] = QString("%1").arg(ipFields[currIpField].toInt()-1);
			break;
		case PanelInfo::rotateRight:
			ipFields[currIpField] = QString("%1").arg(ipFields[currIpField].toInt()+1);
			break;
		case PanelInfo::buttonPush:
			if(currIpField<3)
				currIpField++;
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

			//inform the engine of the change
			QTcpSocket *commSock = panelInfo->getCommandSocket();
			Picto::ProtocolCommand command("FPPUT /IP PICTO/1.0");
			QByteArray content = QString("%1").arg(addr).toUtf8();

			command.setContent(content);
			command.setFieldValue("Content-Length", QString("%1").arg(content.size()));
			command.write(commSock);

			//wait for a response from the engine
			Picto::ProtocolResponse response;
			response.read(commSock);
			if(response.getResponseCode() != 200)
			{
				doMessage("IP not updated,","try again");
			}


		}
		else
		{
			addr = QString("%1.%2.%3.%4").arg(ipFields[0]).
				arg(ipFields[1]).arg(ipFields[2]).arg(ipFields[3]);
			panelInfo->setSystemIP(QHostAddress(addr));
			drawChangeIP();
		}
	}
	//--- Change Name Mode ---
	else if(panelInfo->getDispMode() == PanelInfo::ChangeNameMode)
	{
		bool exit = false;
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
			QTcpSocket *commSock = panelInfo->getCommandSocket();
			Picto::ProtocolCommand command(QString("FPPUT /boxname PICTO/1.0"));
			QByteArray content = QString("%1").arg(name).toUtf8();

			command.setFieldValue("Content-Length", QString("%1").arg(content.size()));
			command.setContent(content);
			command.write(commSock);

			//wait for a response from the engine
			Picto::ProtocolResponse response;
			response.read(commSock);
			if(response.getResponseCode() != 200)
			{
				doMessage("Name not updated,","try again");
			}

		}

		else
		{
			panelInfo->setSystemName(name);
			drawChangeName();
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

		if(exit)
		{
			panelInfo->setDispMode(PanelInfo::MenuMode); 
			drawMenu();

			//inform the engine of the change
			QTcpSocket *commSock = panelInfo->getCommandSocket();
			Picto::ProtocolCommand command(QString("FPPUT /reward/%1/duration PICTO/1.0").arg(panelInfo->getRewardController()));
			QByteArray content = QString("%1").arg(rewardDur).toUtf8();

			command.setContent(content);
			command.setFieldValue("Content-Length", QString("%1").arg(content.size()));
			command.write(commSock);

			//wait for a response from the engine
			Picto::ProtocolResponse response;
			response.read(commSock);
			if(response.getResponseCode() != 200)
			{
				doMessage("Duration not updated,","try again");
			}

		}
		else
		{
			panelInfo->setRewardDuration(rewardDur);
			drawRewardDuration();
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

		if(exit)
		{
			panelInfo->setDispMode(PanelInfo::MenuMode); 
			drawMenu();

			//inform the engine of the change
			QTcpSocket *commSock = panelInfo->getCommandSocket();
			Picto::ProtocolCommand command(QString("FPPUT /reward/%1/flushduration PICTO/1.0").arg(panelInfo->getRewardController()));
			QByteArray content = QString("%1").arg(flushDur).toUtf8();

			command.setContent(content);
			command.setFieldValue("Content-Length", QString("%1").arg(content.size()));
			command.write(commSock);

			//wait for a response from the engine
			Picto::ProtocolResponse response;
			response.read(commSock);
			if(response.getResponseCode() != 200)
			{
				doMessage("Duration not updated,","try again");
			}

		}
		else
		{
			panelInfo->setFlushDuration(flushDur);
			drawFlushDuration();
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

			//send out a start flushing command
			QTcpSocket *commSock = panelInfo->getCommandSocket();
			int controller = panelInfo->getRewardController();
			Picto::ProtocolCommand command(QString("FPSTOPFLUSH /reward/%1 PICTO/1.0").arg(controller));
			command.write(commSock);

			//wait for a response from the engine
			Picto::ProtocolResponse response;
			response.read(commSock);

			panelInfo->setDispMode(PanelInfo::MenuMode);
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
			QTcpSocket *commSock = panelInfo->getCommandSocket();
			Picto::ProtocolCommand command(QString("FPREWARD /reward/%1 PICTO/1.0").arg(panelInfo->getRewardController()));
			command.write(commSock);

			//wait for a response from the engine
			Picto::ProtocolResponse response;
			response.read(commSock);
			if(response.getResponseCode() != 200)
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
			cursorPos = 1;
			drawMenu();
		}

	}
	//Flushing Menu Items
	else if(menuItems[currItem].menu == 3)
	{
		if(menuItems[currItem].name == "Start Flush")
		{
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
void Menu::drawRewardDuration()
{
	int controller = panelInfo->getRewardController();
	int rewardDur = panelInfo->getRewardDuration();
	
	QString line1 = QString("Reward %1 Duration:").arg(controller);
	QString line2 = QString("   %1 \x08").arg(rewardDur);

	emit updateLCD(1,line1);
	emit updateLCD(2,line2);
}

//Sets everything up for the reward duration mode
void Menu::initRewardDuration()
{
	QTcpSocket *commSock = panelInfo->getCommandSocket();

	int controller = panelInfo->getRewardController();

	//send a command
	Picto::ProtocolCommand command(QString("FPGET /reward/%1/duration PICTO/1.0").arg(controller));
	command.write(commSock);

	//get response - I should be doing some error checking here to confirm that we
	//got the whole response, but that seems unneccessary given that we're communicating
	//to localhost...
	Picto::ProtocolResponse engineResponse;
	int bytesRead = engineResponse.read(commSock);
	if(bytesRead<0)
	{
		doMessage("Duration not recv'd","from engine");
		return;
	}

	QString duration = engineResponse.getDecodedContent();

	panelInfo->setRewardDuration(duration.toInt());

	panelInfo->setDispMode(PanelInfo::RewardDurMode);
	drawRewardDuration();

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
	QTcpSocket *commSock = panelInfo->getCommandSocket();

	int controller = panelInfo->getRewardController();

	//send a command
	Picto::ProtocolCommand command(QString("FPGET /reward/%1/flushduration PICTO/1.0").arg(controller));
	command.write(commSock);

	//get response - I should be doing some error checking here to confirm that we
	//got the whole response, but that seems unneccessary given that we're communicating
	//to localhost...
	Picto::ProtocolResponse engineResponse;
	int bytesRead = engineResponse.read(commSock);
	if(bytesRead<0)
	{
		doMessage("Flush dur not recv'd","from engine");
		return;
	}

	QString duration = engineResponse.getDecodedContent();


	panelInfo->setFlushDuration(duration.toInt());

	panelInfo->setDispMode(PanelInfo::FlushDurMode);
	drawFlushDuration();

	return;
}

//This is the code used to draw the controller changing panel
void Menu::drawFlushDuration()
{
	int duration = panelInfo->getRewardController();
	int controller = panelInfo->getFlushDuration();
	
	QString line1 = QString("Flush %1 Duration:").arg(controller);
	QString line2 = QString("   %1 secs").arg(duration);

	emit updateLCD(1,line1);
	emit updateLCD(2,line2);

}

void Menu::initFlush()
{
	int controller = panelInfo->getRewardController();

	//start the timer
	flushingTimer->start();

	//send out a start flushing command
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	Picto::ProtocolCommand command(QString("FPSTARTFLUSH /reward/%1 PICTO/1.0").arg(controller));
	command.write(commSock);

	//wait for a response from the engine
	Picto::ProtocolResponse response;
	response.read(commSock);
	if(response.getResponseCode() != 200)
	{
		doMessage("Flushing failed,","Try again");
	}	

	panelInfo->setDispMode(PanelInfo::FlushingMode);
	tenthSecondCounter = 0;
	drawFlush();
}

void Menu::drawFlush()
{
	bool endFlush = false;;
	int controller = panelInfo->getRewardController();

	//get the time remaining in the flush
	QTcpSocket *commSock = panelInfo->getCommandSocket();
	Picto::ProtocolCommand command(QString("FPGET /reward/%1/flushtimeremain PICTO/1.0").arg(controller));
	command.write(commSock);

	Picto::ProtocolResponse engineResponse;
	int bytesRead = engineResponse.read(commSock);
	if(bytesRead<0)
		endFlush = true;
	
	bool ok;
	int timeRem = engineResponse.getDecodedContent().toInt(&ok);
	if(!ok)
		endFlush = true;
	if(timeRem == 0)
		endFlush = true;

	QString line1 = "Flushing.";
	QString line2 = QString("  %1 s").arg(timeRem);

	/*! \todo Figure out the timing on the flashing backlight (after removing all the debugging I/O) */
	emit toggleBacklight();
	/*tenthSecondCounter++;
	if(tenthSecondCounter == 5)
	{
		tenthSecondCounter = 0;
		emit toggleBacklight();
	}*/
	if(!endFlush)
	{
		emit updateLCD(1,line1);
		emit updateLCD(2,line2);
	}
	else
	{
		emit turnOnBacklight();
		panelInfo->setDispMode(PanelInfo::MenuMode);
		flushingTimer->stop();
		drawMenu();
	}



}

void Menu::initChangeIP()
{
	//grab the system IP from the engine, and update panelInfo
	QTcpSocket *commSock = panelInfo->getCommandSocket();

	//send a command
	Picto::ProtocolCommand command("FPGET /IP PICTO/1.0");
	command.write(commSock);

	//get response - I should be doing some error checking here to confirm that we
	//got the whole response, but that seems unneccessary given that we're communicating
	//to localhost...
	Picto::ProtocolResponse engineResponse;
	int bytesRead = engineResponse.read(commSock);
	if(bytesRead<0)
	{
		doMessage("IP not recv'd","from engine");
		return;
	}

	QString ipString = engineResponse.getDecodedContent();

	QHostAddress addr;
	if(addr.setAddress(ipString))
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
	drawChangeIP();

	return;
}

void Menu::drawChangeIP()
{
	QString addr = panelInfo->getSystemIP().toString();

	QString line1 = QString("IP: %1").arg(addr);


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


	emit updateLCD(1,line1);
	emit updateLCD(2,line2);
}

void Menu::initChangeName()
{
	//grab the system name from the engine, and update panelInfo
	QTcpSocket *commSock = panelInfo->getCommandSocket();

	//send a command
	Picto::ProtocolCommand command("FPGET /boxname PICTO/1.0");
	command.write(commSock);

	//get response - I should be doing some error checking here to confirm that we
	//got the whole response, but that seems unneccessary given that we're communicating
	//to localhost...
	Picto::ProtocolResponse engineResponse;
	int bytesRead = engineResponse.read(commSock);
	if(bytesRead<0)
	{
		doMessage("Name not recv'd","from engine");
		return;
	}

	QString name = engineResponse.getDecodedContent();

	//add spaces to the system name
	//20 chars in an LCD line - sizeof("Name:") = 15 chars left to use
	name.append(QString(15-name.size(),' '));  

	panelInfo->setSystemName(name);

	currNameChar=0;

	panelInfo->setDispMode(PanelInfo::ChangeNameMode);
	drawChangeName();

	return;
}

void Menu::drawChangeName()
{
	QString name = panelInfo->getSystemName();

	QString line1 = QString("Name:%1").arg(name);

	int numSpaces;
	numSpaces = 5; //for "Name:"
	numSpaces += currNameChar;

	QString line2 = QString(numSpaces,' ');
	line2 += QString(1, '\xA');


	emit updateLCD(1,line1);
	emit updateLCD(2,line2);

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
	//check to see if we've connected to the engine yet
	QTcpSocket *commSock, *eventSock;
	commSock = panelInfo->getCommandSocket();
	eventSock = panelInfo->getEventSocket();

	//The order in this "if" is important, since if we call isValid() on a 
	//null pointer, bad things will happen. (Remember, || will return true
	//when it encounters the first true expression.)
	if(!commSock || !eventSock ||
		!commSock->isValid()  || !eventSock->isValid())
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
