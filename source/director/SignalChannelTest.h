#ifndef SIGNALCHANELTEST_H
#define SIGNALCHANNELTEST_H

#include <QObject>
#include <QTimer>
#include <QSharedPointer>

#include "../common/compositor/PCMAuralTarget.h"
#include "../common/compositor/CompositingSurface.h"
#include "../common/compositor/RenderingTarget.h"
#include "../common/stimuli/CircleGraphic.h"
#include "compositor/D3DVisualTarget.h"

/*!	\brief A test class for playing with signal channels
 *	
 *	This was a test class that was written to test out a signal channel.  It draws a 
 *	circle on the screen, and moves it around according to the inputs from the signal 
 *	channel.  This is actually a really good place to start reading if you're trying 
 *	to understand either signal channels, or the compositing system.
 */
class SignalChannelTest : public QObject
{
	Q_OBJECT

public:
	SignalChannelTest();
	virtual ~SignalChannelTest();

	void exec();

public slots:
	void doFrame();

private:
	QTimer *frameTimer;
	
	QSharedPointer<Picto::D3DVisualTarget> d3dVisualTarget;
	QSharedPointer<Picto::CircleGraphic> circleGraphic;

	//Picto::MouseSignalChannel *mouseChannel;
	//Picto::PictoBoxXPAnalogInputSignalChannel *aiChannel;

	int frameCounter;
};

#endif