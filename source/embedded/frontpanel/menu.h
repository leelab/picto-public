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

//The menu definition system deserves some disucssion here.
//Every menu item has a name, a menu number, and an id.
//The name is what is displayed on the screen, the menu number
//declares which menu the item is associated with.

//The menu system is organized by menu number.  The top level
//menu is 0, but after that sub menus can be ordered in any way.
//When the menu is displayed, on the 2 line LCD, the program
//writes the name of the current menu item(currItem), and then
//searches for the next menuItem in the list that has the same
//menu number and displays that.  Then, to change into a sub-menu, 
//all that is neccessary is to change currItem to a menuItem 
//that has a different menu number and redraw the menus.

//This setup makes moving through the menu a bit painful, since 
//we are always searching for the next item in our menu, but
//it makes keeping track of the menu really easy.


/*!	\brief The front panel's menu interface
 *
 *	The front panel's menu interface is defined here.
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

private slots:
	void checkConnections();
	void returnToStatus();
	//void drawFlush();  //Since this is display "animated" it needs to be a slot

signals:
	void updateLCD(int line, QString text);
	void toggleBacklight();
	void turnOnBacklight();

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

