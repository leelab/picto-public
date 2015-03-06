#ifndef DISPLAYMODE_H
#define DISPLAYMODE_H

#include <QTimer>
#include "FrontPanelInfo.h"
#include "DirectorInterface.h"
using namespace PanelInfo;

/*!	\brief A display mode that defines the results of user interactions and controls the front panel LCD when it is active.
 * \details DisplayMode objects are activated one at a time, and when active they take complete 
 * control over user input handling and LCD output for the frontpanel.  All DisplayModes
 * define an 'activityTimeoutMs()' function as well which defines the amount of time
 * that the DisplayMode will remain active before allowing the FrontPanel application
 * to return control to another more fundamental display mode.  For example, a display
 * mode for setting the name of a pictobox might handle all of the user input and 
 * lcd display for allowing the user to alter the name of the pictobox.  It might define
 * an activityTimeout of 30,000ms at which point, if no user input has occured, the
 * FrontPanel will return control to a read-only Status mode that displays Front Panel 
 * name, reward, and flush timing information.
 * \author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2015
 */
class DisplayMode : public QObject
{
	Q_OBJECT

public:
	DisplayMode();
	virtual ~DisplayMode();

	/**
	 * \brief Called when a DisplayMode is first activated. 
	 * \details Extend this function to initialize the mode and draw the starting display to the
	 * front panel
	 */
	virtual void initMode()=0;
	/**
	 * Defines the amount of time that this mode will retain control without
	 * the user inputting anything before the front panel application can switch 
	 * control to another display mode.
	 */
	virtual int activityTimeoutMs(){return 30000;};
public slots:
	/*! Calls handleUserInput() unless there is a temporary message onscreen. Returns the
	 * PanelInfo::DisplayModeType of the DisplayMode that should be activated next.
	 */
	PanelInfo::DisplayModeType userInputSlot(InputType type);

signals:
	/**
	 * Emitted whenever the Front Panel's LCD should change. Line is 0 or 1,
	 * text is the text to display.
	 */
	void updateLCD(int line, QString text);
	/**
	 * Emitted whenever the Front Panel's LCD toggle the backlight (on to off or off to on)
	 */
	void toggleBacklight();
	/**
	 * Emitted whenever the Front Panel's LCD should turn on the backlight
	 */
	void turnOnBacklight();

protected:
	/**
	 * \brief Used to define responses to user interactions.
	 * \details Defines this DisplayMode's response to 
	 * the input InputType.  Returns a PanelInfo::DisplayModeType to
	 * which application control should switch after user input is processed.  Return this
	 * DisplayMode's type if control should not switch for this particular type of
	 * input.
	 */
	virtual PanelInfo::DisplayModeType handleUserInput(PanelInfo::InputType type) = 0;
	/**
	 * Should be extended to return the PanelInfo::DisplayModeType of this object.
	 */
	virtual PanelInfo::DisplayModeType myModeType() = 0;
	void drawMenuRows(int cursorPos, QString line1, QString line2);
	void drawText(QString line1, QString line2);
	void drawMessage(int durationSecs, QString line1, QString line2);

private:
	QTimer messageTimer_;
	QString lastLine1_;
	QString lastLine2_;

private slots:
	void messageTimedOut();
};


#endif

