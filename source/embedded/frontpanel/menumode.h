#ifndef MENUMODE_H
#define MENUMODE_H

#include <QList>
#include "DisplayMode.h"

/*!	\brief A DisplayMode that allows the user to move through a menu including 
 * all of the DisplayModes available for view in the Front Panel system.
 * \details Allows users to navigate through a list of other DisplayModes by
 * turning the input knob and select one by pressing it.
 * \author Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2013
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
	/*!
	 * A struct containing each PanelInfo::DisplayModeType along with a user friendly name.
	 */
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

