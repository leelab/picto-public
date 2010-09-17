#ifndef _MOUSESIGNALCHANNEL_H
#define _MOUSESIGNALCHANNEL_H

#include <QTimer>
#include <QObject>
#include <QWidget>
#include <QSharedPointer>

#include "../common.h"
#include "SignalChannel.h"



namespace Picto {

/*! \brief A signal channel for use with the mouse.
 *
 *	Although a mouse signal channel is completely useless in an experiment (unless your 
 *	subject is capable of using a mouse), it is absolutely essential when testing an
 *	experiment.  When updateDataBuffer is called, the channel polls the mouse and
 *	grabs the most recent coordinates.  This could potentially cause us to miss very
 *	quick movements (particularly if updateDataBuffer is called infrequently), but so
 *	far this hasn't been an issue.
 */
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