
#include "TextEditMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

/*! Constructs a new TextEditMode object for a value named valName, whose value can
 * have up to maxChars characters.  slowInc sets the number of letters to flip through
 * per tick when the user is turning the input knob slowly.  fastInc sets the number of
 * letters to flip through when the user is turning the knob quickly.
 *
 */
TextEditMode::TextEditMode(int maxChars,QString valName,int slowInc,int fastInc) :
currTextChar_(0),
maxChars_(maxChars),
valName_(valName),
slowInc_(slowInc),
fastInc_(fastInc),
textAtInit_(""),
currText_("")
{
	Q_ASSERT(valName.size()+maxChars_+1<=20);
}

TextEditMode::~TextEditMode()
{
}

/*!
 * Sets up the object by getting the latest editable text, storing it and drawing it to display.
 */
void TextEditMode::initMode()
{
	QString latestText = getEditableText();
	//Only reset the text value if the text received from
	//the director changed, otherwise maintain the currText_ we
	//had when this state was last active.
	if(latestText != textAtInit_)
	{
		textAtInit_ = latestText;
		currText_ = textAtInit_;
	}
	currTextChar_ = 0;
	drawDisplay();
}

/*! 
 * Handles user input by turning knob rotations into letter/number flips through the alphabet and
 * knob presses into a message to accept the current letter and move on to the next one if the cursor
 * is inside the editable string, or a message to accept changes if the cursor is past the editable
 * string.
 */
PanelInfo::DisplayModeType TextEditMode::handleUserInput(PanelInfo::InputType type)
{
	bool setTheVal = false;
	int numIncs = slowInc_;
	switch(type)
		{
		case PanelInfo::rotateLeftFast:
			numIncs = fastInc_;
		case PanelInfo::rotateLeft:
			for(int i=0;i<numIncs;i++)
			{
				if(currText_[currTextChar_] == ' ')
					currText_[currTextChar_] = '0';
				else if(currText_[currTextChar_] == '9')
					currText_[currTextChar_] = 'A';
				else if(currText_[currTextChar_] == 'Z')
					currText_[currTextChar_] = 'a';
				else if(currText_[currTextChar_] == 'z')
					currText_[currTextChar_] = ' ';
				else
					currText_[currTextChar_]=currText_[currTextChar_].toLatin1()+1;
			}
			break;
		case PanelInfo::rotateRightFast:
			numIncs = fastInc_;
		case PanelInfo::rotateRight:
			for(int i=0;i<numIncs;i++)
			{
				if(currText_[currTextChar_] == 'a')
					currText_[currTextChar_] = 'Z';
				else if(currText_[currTextChar_] == 'A')
					currText_[currTextChar_] = '9';
				else if(currText_[currTextChar_] == '0')
					currText_[currTextChar_] = ' ';
				else if(currText_[currTextChar_] == ' ')
					currText_[currTextChar_] = 'z';
				else
					currText_[currTextChar_]=currText_[currTextChar_].toLatin1()-1;
			}
			break;
		case PanelInfo::buttonPush:
			if(currText_[currTextChar_] == ' ')
				setTheVal = true;
			else if(currTextChar_ == maxChars_-1)
				setTheVal = true;
			else if(currTextChar_ < currText_.size()-1)
				currTextChar_++;
			else
				setTheVal = true;
			break;
		default:
			return myModeType();
		}
	if(setTheVal)
	{
		if(currText_.isEmpty())
		{
			drawMessage(2000,"Name must have at","least one character");
			return myModeType();
		}
		if(setValueToDirector(currText_))
		{
			return MenuModeType;
		}
		drawMessage(2000,"Name not updated,","try again");
		return myModeType();
	}
	drawDisplay();
	return myModeType();
}

/*! Draws the current display including data name, data value and cursor.
*/
void TextEditMode::drawDisplay()
{
	QString gap = QString(19-(valName_.size()+maxChars_),' ');
	QString cursorLine = QString(currTextChar_+valName_.size()+1+gap.size(),' ')+'\xA';
	drawText(valName_+":"+gap+currText_,cursorLine);
}