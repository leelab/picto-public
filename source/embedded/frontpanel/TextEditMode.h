#ifndef TextEditMode_H
#define TextEditMode_H

#include <QList>
#include <QSharedPointer>
#include "DisplayMode.h"

/*!	\brief A DisplayMode including functionality for the user to edit text
 * \details DisplayMode class that allows a user to change a text string should
 * extend this class.  It displays text with a blinking cursor underneath the 
 * letter to be edited and allows the user to edit the letter by turning the input knob.
 * \author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2015
 */
class TextEditMode : public DisplayMode
{
	Q_OBJECT

public:
	TextEditMode(int maxChars,QString valName,int slowInc,int fastInc);
	virtual ~TextEditMode();
	virtual void initMode();

protected:
	virtual PanelInfo::DisplayModeType handleUserInput(PanelInfo::InputType type);
	/*! Returns a text string that the user can edit while this DisplayMode has control.
	 */
	virtual QString getEditableText()=0;
	/*! \brief Sets the input text value to the Picto director.  
	 * \details This function is called by the TextEditMode object whenever the text returned from getEditableText()
	 * is edited by the user.  It returns true if the message is successfully set to the director, 
	 * and false otherwise.
	 */
	virtual bool setValueToDirector(QString text)=0;

private:
	void drawDisplay();
	QSharedPointer<DirectorInterface> directorIf_;
	QString currText_;
	QString textAtInit_;
	int currTextChar_;
	int maxChars_;
	QString valName_;
	int slowInc_;
	int fastInc_;
};


#endif

