
#include "NumberEditMode.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"

/*! Constructs a new NumberEditMode object for a value named valName, whose value can
 * have be anywhere between minValue and maxValue.  unitStr indicates the units of the 
 * value, slowIncrement sets the increment per tick when the user is turning the input 
 *knob slowly.  fastIncrement sets the increment when the user is turning the knob quickly.
 */
NumberEditMode::NumberEditMode(int maxValue,int minValue,QString valName,QString unitStr,int fastIncrement,int slowIncrement) :
valName_(valName),
maxValue_(maxValue),
minValue_(minValue),
unitStr_(unitStr),
fastInc_(fastIncrement),
slowInc_(slowIncrement),
currNum_(minValue),
numAtInit_(minValue)
{
	QString maxValStr = QString::number(maxValue_);
	QString minValStr = QString::number(minValue_);
	if(maxValStr.size() > minValStr.size())
		maxDigits_ = maxValStr.size();
	else
		maxDigits_ = minValStr.size();
	Q_ASSERT(valName.size()+maxDigits_+1<=20);
	Q_ASSERT(fastInc_ >= slowInc_);
	Q_ASSERT(slowInc_ > 0);
}

NumberEditMode::~NumberEditMode()
{
}
/*!
 * Sets up the object by getting the latest editable number, storing it and drawing it to display.
 */
void NumberEditMode::initMode()
{
	//Only reset the number value if the number received from
	//the director changed, otherwise maintain the currNum_ we
	//had when this state was last active.
	int latestInitNum = getEditableNumber();
	if(latestInitNum != numAtInit_)
	{
		numAtInit_ = latestInitNum;
		currNum_ = numAtInit_;
	}
	drawDisplay();
}
/*! 
 * Handles user input by turning knob rotations into number increments/decrements and  
 * knob presses into a message to accept the current value.
 */
PanelInfo::DisplayModeType NumberEditMode::handleUserInput(PanelInfo::InputType type)
{
	switch(type)
		{
		case PanelInfo::rotateLeftFast:
			currNum_ = currNum_+fastInc_;
			if(currNum_ > maxValue_)
				currNum_ = maxValue_;
			break;
		case PanelInfo::rotateLeft:
			currNum_ = currNum_+slowInc_;
			if(currNum_ > maxValue_)
				currNum_ = maxValue_;
			break;
		case PanelInfo::rotateRightFast:
			currNum_ = currNum_-fastInc_;
			if(currNum_ < minValue_)
				currNum_ = minValue_;
			break;
		case PanelInfo::rotateRight:
			currNum_ = currNum_-slowInc_;
			if(currNum_ < minValue_)
				currNum_ = minValue_;
			break;
		case PanelInfo::buttonPush:
			if(setValueToDirector(currNum_))
			{
				return MenuModeType;
			}
			drawMessage(2000,"Value not updated,","try again");
			return myModeType();
			break;
		default:
			return myModeType();
		}
	drawDisplay();
	return myModeType();
}
/*! Draws the current display including data name, data value and units.
*/
void NumberEditMode::drawDisplay()
{
	QString valText = QString::number(currNum_)+unitStr_;
	QString gap = QString(19-(valName_.size()+valText.size()),' ');
	drawText(valName_+":"+gap+valText,"");
}