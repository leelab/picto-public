
#include "TextEditMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

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
					currText_[currTextChar_]=currText_[currTextChar_].toAscii()+1;
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
					currText_[currTextChar_]=currText_[currTextChar_].toAscii()-1;
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
			break;
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

void TextEditMode::drawDisplay()
{
	QString gap = QString(19-(valName_.size()+maxChars_),' ');
	QString cursorLine = QString(currTextChar_+valName_.size()+1+gap.size(),' ')+'\xA';
	drawText(valName_+":"+gap+currText_,cursorLine);
}