#ifndef STATUSMODE_H
#define STATUSMODE_H

#include <QList>
#include <QSharedPointer>
#include <QTimer>
#include "DisplayMode.h"

/*!	\brief A DisplayMode that allows the user to monitor the current status of the Front Panel application
 * \details Displayed information includes Pictobox Name, ServerDisconnected/ServerConnected/Stopped/Running
 * status symbol, Reward duration, Flush duration, Default Reward Controller Number, PictoBox IP address.  
 * This data takes up three lines which the user navigates by turning the input knob.
 * \author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2015
 */
class StatusMode : public DisplayMode
{
	Q_OBJECT

public:
	StatusMode(QSharedPointer<DirectorInterface> directorIf,FrontPanelInfo *panelInfo);
	virtual ~StatusMode();
	virtual void initMode();

protected:
	virtual PanelInfo::DisplayModeType handleUserInput(PanelInfo::InputType type);
	virtual PanelInfo::DisplayModeType myModeType(){return PanelInfo::StatusModeType;};

private:
	void drawStatus();
	QList<QString> statusItems_;
	int currTopItem_;
	QSharedPointer<DirectorInterface> directorIf_;
	FrontPanelInfo *panelInfo_;
	QTimer updateTimer_;

private slots:
	void updateStatus();
};


#endif

