#ifndef MENU_H
#define MENU_H

#include <QList>
#include <QMap>
#include <QString>
#include <QObject>
#include <QTimer>
#include <QSharedPointer>

#include "FrontPanelInfo.h"
#include "DirectorInterface.h"
#include "DisplayMode.h"

/*!	\brief The top level object for the front panel that creates all operable DisplayModes and connects them to the Phidgets interface.
 * \details This is the top level object for the FrontPanel Application.  It creates all of the DisplayMode objects
 * that can handle user interaction in this application and handles the process of switching between these modes.
 * It also handles things like a set DisplayMode timeout after which if the user hasn't done anything, operation
 * returns to the default StatusMode in addition to other minor housekeeping activities.
 * If you are trying to understand the operation of the FrontPanel Application.  This is the place to start.
 * \author Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2013
 */
class Menu : public QObject
{
	Q_OBJECT

public:
	Menu(FrontPanelInfo *panelInfo);
	virtual ~Menu();

	void initMenu();

public slots:
	void userInputSlot(int type);
	void aboutToQuit();

private slots:
	void checkConnections();
	void returnToStatus();
	//void drawFlush();  //Since this is display "animated" it needs to be a slot

signals:
	/*! \brief Emitted when the output line of the LCD should be set to the output text where the top line is 1 and the bottom line is 2*/
	void updateLCD(int line, QString text);
	/*! \brief Emitted when LCD backlight's On/Off state should be toggled*/
	void toggleBacklight();
	/*! \brief Emitted when LCD's backlight should be turned On*/
	void turnOnBacklight();
	/*! \brief Emitted when LCD's backlight should be turned Off*/
	void turnOffBacklight();

private:
	void loadMenus();

	//void initFlush();
	FrontPanelInfo *panelInfo;
	QSharedPointer<DirectorInterface> directorIf_;

	QTimer *connectionTimer;
	QTimer *activityTimer;
	bool wasConnected_;
	/*QTimer *flushingTimer;*/

	QMap<PanelInfo::DisplayModeType,QSharedPointer<DisplayMode>> panelModes_;
	QSharedPointer<DisplayMode> currMode_;	

};


#endif

