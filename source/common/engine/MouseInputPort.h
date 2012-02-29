#ifndef _MOUSEINPUTPORT_H
#define _MOUSEINPUTPORT_H

#include <QTimer>
#include <QWidget>
#include <QSharedPointer>
#include <QPoint>

#include "../common.h"
#include "InputPort.h"



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
class PICTOLIB_API MouseInputPort : public InputPort
#else
class MouseInputPort : public InputPort
#endif
{

public:
	MouseInputPort( QWidget *widget);
	virtual QString type(){return "mouse";};

protected:
	virtual bool startSampling();
	virtual void stopSampling();
	virtual double updateDataBuffer();

private:
	QPoint pollMouse();

private:
	//QTimer pollingTimer_;
	QWidget *widget_;  //used for coordinate translation
	double lastTime_;

};

};

#endif