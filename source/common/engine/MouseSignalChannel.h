#ifndef _MOUSESIGNALCHANNEL_H
#define _MOUSESIGNALCHANNEL_H

#include <QTimer>
#include <QObject>
#include <QWidget>
#include <QSharedPointer>

#include "../common.h"
#include "SignalChannel.h"



namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API MouseSignalChannel : public QObject, public SignalChannel
#else
class MouseSignalChannel :  public QObject, public SignalChannel
#endif
{
	Q_OBJECT

public:
	MouseSignalChannel(int sampsPerSecond, QWidget *widget);

	bool start();
	bool stop();
	void updateDataBuffer();

private slots:
	void pollMouse();

private:
	QTimer pollingTimer_;
	QWidget *widget_;  //used for coordinate translation

};

};

#endif