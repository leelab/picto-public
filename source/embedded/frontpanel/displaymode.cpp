
#include "DisplayMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

DisplayMode::DisplayMode()
{
	connect(&messageTimer_,SIGNAL(timeout()),this,SLOT(messageTimedOut()));
}

DisplayMode::~DisplayMode()
{
}

PanelInfo::DisplayModeType DisplayMode::userInputSlot(PanelInfo::InputType type)
{
	//Don't report user actions if we're waiting for a message to timeout
	if(messageTimer_.isActive())
		return myModeType();
	
	//Tell child to handle input
	handleUserInput(type);
}

void DisplayMode::drawMenuRows(int cursorPos, QString line1, QString line2)
{
	//\x09 is the 2nd custom character, which is set to an arrow character
	//(see phidgets.cpp)
	if(cursorPos == 0)
	{
		line1 = '\x09'+line1;
		line2 = " "+line2;
	}
	else
	{
		line1 = " "+line1;
		line2 = '\x09'+line2;
	}
	drawText(line1,line2);
}

void DisplayMode::drawText(QString line1, QString line2)
{
	lastLine1_ = line1;
	lastLine2_ = line2;
	emit updateLCD(1,line1);
	emit updateLCD(2,line2);
	return;
}

void DisplayMode::drawMessage(int durationMs, QString line1, QString line2)
{
	//Draw the message and start a timer.  When the timer runs out, we'll
	//restore the last lines that were on the screen.
	QString currLine1 = lastLine1_;
	QString currLine2 = lastLine2_;
	drawText(line1,line2);
	lastLine1_ = currLine1;
	lastLine2_ = currLine2;
	messageTimer_.setInterval(durationMs);
	messageTimer_.start();
	return;
}

void DisplayMode::messageTimedOut()
{
	messageTimer_.stop();
	drawText(lastLine1_,lastLine2_);
}