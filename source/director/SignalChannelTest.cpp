#include <QRect>
#include <QMap>
#include <QList>
#include <QString>

#include "SignalChannelTest.h"


SignalChannelTest::SignalChannelTest(QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget)
	: pixmapVisualTarget(pixmapVisualTarget)
{
	frameCounter = 0;

	mouseChannel = new Picto::MouseSignalChannel(120, pixmapVisualTarget);
	mouseChannel->start();

	frameTimer = new QTimer(this);
	frameTimer->setInterval(17);
	connect(frameTimer, SIGNAL(timeout()), this, SLOT(doFrame()));
	frameTimer->start();

	elapsedTime.start();
}

SignalChannelTest::~SignalChannelTest()
{
}

void SignalChannelTest::doFrame()
{
	frameCounter++;

	QMap<QString, QList<double> > mouseData;
	mouseData = mouseChannel->getValues();

	int xpos,ypos;

	if(!mouseData.value("xpos").isEmpty())
		xpos = mouseData.value("xpos").at(0);
	else
		xpos = 0;

	if(!mouseData.value("ypos").isEmpty())
		ypos = mouseData.value("ypos").at(0);
	else
		ypos = 0;

	QString text = QString("(%1, %2)").arg(xpos).arg(ypos);


	pixmapVisualTarget->drawNonExperimentText(QFont("Arial",18),
											  QColor(Qt::white),
											  QRect(frameCounter%1280,10,200,50),
											  Qt::AlignCenter,
											  text);

	pixmapVisualTarget->present();

	text = QString("%1 - %2").arg(frameCounter).arg(elapsedTime.elapsed());

	printf(text.toAscii().data());
	printf("\n");
	elapsedTime.restart();
}

