#ifndef SIGNALCHANELTEST_H
#define SIGNALCHANNELTEST_H

#include <QObject>
#include <QTimer>
#include <QSharedPointer>

#include "../common/compositor/PixmapVisualTarget.h"
#include "../common/engine/MouseSignalChannel.h"
#include "../common/engine/PictoBoxAnalogInputSignalChannel.h"
#include "../common/stimuli/CircleGraphic.h"

class SignalChannelTest : public QObject
{
	Q_OBJECT

public:
	SignalChannelTest(QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget);
	~SignalChannelTest();

public slots:
	void doFrame();

private:
	QTimer *frameTimer;
	
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget;
	QSharedPointer<Picto::CircleGraphic> circleGraphic;

	Picto::MouseSignalChannel *mouseChannel;
	Picto::PictoBoxAnalongInputSignalChannel *aiChannel;

	int frameCounter;
};

#endif