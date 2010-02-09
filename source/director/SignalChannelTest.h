#ifndef SIGNALCHANELTEST_H
#define SIGNALCHANNELTEST_H

#include <QObject>
#include <QTimer>
#include <QSharedPointer>

#include "../common/compositor/PCMAuralTarget.h"
#include "../common/compositor/CompositingSurface.h"
#include "../common/compositor/RenderingTarget.h"
#include "../common/compositor/D3DVisualTarget.h"
#include "../common/engine/MouseSignalChannel.h"
#include "../common/engine/PictoBoxXPAnalogInputSignalChannel.h"
#include "../common/stimuli/CircleGraphic.h"

class SignalChannelTest : public QObject
{
	Q_OBJECT

public:
	SignalChannelTest();
	~SignalChannelTest();

	void exec();

public slots:
	void doFrame();

private:
	QTimer *frameTimer;
	
	QSharedPointer<Picto::D3DVisualTarget> d3dVisualTarget;
	QSharedPointer<Picto::CircleGraphic> circleGraphic;

	Picto::MouseSignalChannel *mouseChannel;
	Picto::PictoBoxXPAnalogInputSignalChannel *aiChannel;

	int frameCounter;
};

#endif