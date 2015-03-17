
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
	return handleUserInput(type);
}

/**
 * Draws two lines of text, line1 and line2 to the LCD display as menu rows.
 * Takes a cursorPos input of 0 or 1 to indicate which line of text is 
 * is currently active (0:line1, 1:line2).  Selection is indicated by
 * drawing an error to the left of the currently selected row.
 */
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

/**
 * Draws two lines of text, line1 and line2 to the LCD display.
 */
void DisplayMode::drawText(QString line1, QString line2)
{
	lastLine1_ = line1;
	lastLine2_ = line2;
	emit updateLCD(1,line1);
	emit updateLCD(2,line2);
	return;
}

/**
 * \brief Draws a brief temporary message to the display.
 * \details Two lines of text, line1 and line2 are written to the LCD display.
 * At the end of durationMs, the text that appeared prior to this function call
 * is restored to the LCD.
 */
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
/**
 * Used to restore lines of text to the display after a timeout set in the messageTimer_;
 */
void DisplayMode::messageTimedOut()
{
	messageTimer_.stop();
	drawText(lastLine1_,lastLine2_);
}