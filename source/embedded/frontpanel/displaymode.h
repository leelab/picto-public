#ifndef DISPLAYMODE_H
#define DISPLAYMODE_H

#include <QTimer>
#include "FrontPanelInfo.h"
#include "DirectorInterface.h"
using namespace PanelInfo;

/*!	\brief The front panel's DisplayMode interface
 *
 *	The front panel's DisplayMode interface is defined here.
 */
class DisplayMode : public QObject
{
	Q_OBJECT

public:
	DisplayMode();
	virtual ~DisplayMode();

	virtual void initMode()=0;
	virtual int activityTimeoutMs(){return 30000;};
public slots:
	PanelInfo::DisplayModeType userInputSlot(InputType type);

signals:
	void updateLCD(int line, QString text);
	void toggleBacklight();
	void turnOnBacklight();

protected:
	//Should be overloaded to handle the user input and return the next mode that
	//should be run.
	virtual PanelInfo::DisplayModeType handleUserInput(PanelInfo::InputType type) = 0;
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

