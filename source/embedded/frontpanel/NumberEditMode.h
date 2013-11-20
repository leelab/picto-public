#ifndef NUMBEREDITMODE_H
#define NUMBEREDITMODE_H

#include <QList>
#include <QSharedPointer>
#include "DisplayMode.h"

/*!	\brief A DisplayMode including functionality for the user to edit numbers
 * \details DisplayMode class that allows a user to change a number should
 * extend this class.  It displays numbers that can be edited by turning the input knob.
 * \author Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2013
 */
class NumberEditMode : public DisplayMode
{
	Q_OBJECT

public:
	NumberEditMode(int maxValue,int minValue,QString valName,QString unitStr,int fastIncrement,int slowIncrement);
	virtual ~NumberEditMode();
	virtual void initMode();

protected:
	virtual PanelInfo::DisplayModeType handleUserInput(PanelInfo::InputType type);
	/*! Returns a number that the user can edit while this DisplayMode has control.
	 */
	virtual int getEditableNumber()=0;
	/*! \brief Sets the input number value to the Picto director.  
	 * \details This function is called by the NumberEditMode object whenever the number returned from getEditableNumber()
	 * is edited by the user.  It returns true if the message is successfully set to the director, 
	 * and false otherwise.
	 */
	virtual bool setValueToDirector(int val)=0;

private:
	void drawDisplay();
	QSharedPointer<DirectorInterface> directorIf_;
	int currNum_;
	int maxDigits_;
	int maxValue_;
	int minValue_;
	int fastInc_;
	int slowInc_;
	int numAtInit_;
	QString valName_;
	QString unitStr_;
};


#endif

