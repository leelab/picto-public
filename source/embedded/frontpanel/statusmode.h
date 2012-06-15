#ifndef STATUSMODE_H
#define STATUSMODE_H

#include <QList>
#include <QSharedPointer>
#include <QTimer>
#include "DisplayMode.h"

/*!	\brief The front panel's StatusMode interface
 *
 *	The front panel's StatusMode interface is defined here.
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

