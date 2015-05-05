#ifndef _MOUSEINPUTPORT_H
#define _MOUSEINPUTPORT_H

#include <QTimer>
#include <QWidget>
#include <QSharedPointer>
#include <QPoint>

#include "../common.h"
#include "InputPort.h"



namespace Picto {
class VisualTarget;
class VisualTargetHost;


/*! \brief A signal channel for use with the mouse.
 *
 *	Although a mouse signal channel is completely useless in an experiment (unless your subject is capable of using a mouse),
 *	it is absolutely essential when testing an experiment.  When updateDataBuffer() is called, the channel polls the mouse
 *	and grabs the most recent coordinates.  This could potentially cause us to miss very quick movements but since Picto
 *	currently calls updateDataBuffer() once a frame, this hasn't been an issue.  Since the polling timing is configured in
 *	the InputPort class, we simulate the correct number of samples when updateDataBuffer() is called even though it is
 *	actually only read once.  To do this, we calculate the time since the last call to updateDataBuffer() and divide by the
 *	configured sample period for the number of samples needed, then copy the mouse data that number of times and enter it
 *	into the data buffer.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	//! This widget is used just for its coordinate translation abilities.
	QWidget *widget_;
	//! Tracks the timestamp of the last submitted simulated sample.
	double lastTime_;

	VisualTarget *visualTarget;
	VisualTargetHost *visualTargetHost;

};

};

#endif