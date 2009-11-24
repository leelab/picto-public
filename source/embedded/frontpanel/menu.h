#ifndef MENU_H
#define MENU_H

#include <QList>
#include <QString>
#include <QObject>
#include <QTimer>

#include "FrontPanelInfo.h"

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
struct MenuItem
{
	MenuItem(QString name, int menu) : name(name), menu(menu) {};
	bool operator==(const MenuItem& rhs) { return (rhs.name == this->name) && (rhs.menu == this->menu); };

	QString name;
	int menu;
};

class Menu : public QObject
{
	Q_OBJECT

public:
	Menu(FrontPanelInfo *panelInfo);
	~Menu();

	void initMenu();

public slots:
	void userInputSlot(int type);
	void updateStatus();

private slots:
	void checkConnections();
	void drawFlush();  //Since this is display "animated" it needs to be a slot

signals:
	void updateLCD(int line, QString text);
	void toggleBacklight();
	void turnOnBacklight();

private:
	void loadMenus();
	void drawMenu();
	void menuAction();

	void doMessage(QString line1, QString line2="");

	void drawStatus();

	void initRewardDuration();
	void drawRewardDuration(bool firstTime);

	void drawChangeController();

	void initFlushDuration();
	void drawFlushDuration(bool firstTime);

	void initFlush();

	void initChangeIP();
	void drawChangeIP(bool drawLine2);

	void initChangeName();
	void drawChangeName(bool drawLine2);

	int nextMenuItem();
	int prevMenuItem();

	FrontPanelInfo *panelInfo;

	QTimer *connectionTimer;
	QTimer *flushingTimer;

	int currItem;
	int cursorPos;
	QList<MenuItem> menuItems;

	int currIpField;  //note: the ip field starts at 0 for the MSBs and ends at 3 for the LSBs.  
					  //This value is used in text parsing, which is why it's so odd.
	int currNameChar; //used to keep track of which character in the system name is being changed.
	

};


#endif

