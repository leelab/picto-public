#ifndef MENUMODE_H
#define MENUMODE_H

#include <QList>
#include "DisplayMode.h"

/*!	\brief The front panel's MenuMode interface
 *
 *	The front panel's MenuMode interface is defined here.
 */
class MenuMode : public DisplayMode
{
	Q_OBJECT

public:
	MenuMode();
	virtual ~MenuMode();
	virtual void initMode();
	virtual int activityTimeoutMs(){return 5000;};

protected:
	virtual PanelInfo::DisplayModeType handleUserInput(PanelInfo::InputType type);
	virtual PanelInfo::DisplayModeType myModeType(){return PanelInfo::MenuModeType;};

private:
	void drawMenu();
	struct MenuItem
	{
		MenuItem(PanelInfo::DisplayModeType m,QString t){mode=m;text=t;};
		PanelInfo::DisplayModeType mode;
		QString text;
	};
	QList<MenuItem> menuItems_;
	int currSelItem_;
	int currTopItem_;
};


#endif

