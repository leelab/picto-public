#ifndef SIGNALCHANELTEST_H
#define SIGNALCHANNELTEST_H

#include <QObject>
#include <QTimer>
#include <QSharedPointer>

#include "../common/compositor/PixmapVisualTarget.h"
#include "../common/engine/MouseSignalChannel.h"

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

	Picto::MouseSignalChannel *mouseChannel;

	int frameCounter;
};

#endif