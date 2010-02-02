#include <QRect>
#include <QMap>
#include <QList>
#include <QString>

#include "../common/iodevices/PictoBoxDaqBoard.h"

#include "SignalChannelTest.h"


SignalChannelTest::SignalChannelTest(QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget)
	: pixmapVisualTarget(pixmapVisualTarget)
{
	frameCounter = 0;

	//set up mouse channel
	mouseChannel = new Picto::MouseSignalChannel(120, pixmapVisualTarget);
	mouseChannel->start();

	//set up ai channels
	QSharedPointer<Picto::PictoBoxDaqBoard> daqBoard(new Picto::PictoBoxDaqBoard);
	aiChannel = new Picto::PictoBoxAnalogInputSignalChannel(daqBoard,250);
	aiChannel->addAiChannel("xeye",0);
	aiChannel->addAiChannel("yeye",1);

	QRect visualTargetRect = pixmapVisualTarget->getDimensions();
	aiChannel->setCalibrationRange("xeye", 0,10,0,visualTargetRect.width());
	aiChannel->setCalibrationRange("yeye", 0,10,0,visualTargetRect.height());
	aiChannel->start();

	QSharedPointer<Picto::CompositingSurface> compositingSurface = 
		pixmapVisualTarget->generateCompositingSurface();

	QSharedPointer<Picto::CircleGraphic> circleGraphicTemp(new Picto::CircleGraphic(QPoint(10,10),100,QColor(0,255,0,200)));
	circleGraphic = circleGraphicTemp;
	circleGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	frameTimer = new QTimer(this);
	frameTimer->setInterval(17);
	connect(frameTimer, SIGNAL(timeout()), this, SLOT(doFrame()));
	frameTimer->start();
}

SignalChannelTest::~SignalChannelTest()
{
}

void SignalChannelTest::doFrame()
{
	frameCounter++;

	//Collect mouse data
	//------------------
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

	QString mousePos = QString("(%1, %2)").arg(xpos).arg(ypos);

	pixmapVisualTarget->drawNonExperimentText(QFont("Arial",18),
											  QColor(Qt::white),
											  QRect(frameCounter%1280,10,200,50),
											  Qt::AlignCenter,
											  mousePos);
	
	//Collect eye-tracker data
	//------------------------
	QMap<QString, QList<double> > eyeData;
	eyeData = aiChannel->getValues();

	float eyeX, eyeY;

	if(!eyeData.value("xeye").isEmpty())
		eyeX = eyeData.value("xeye").at(0);
	else
		eyeX = 0.0;

	if(!eyeData.value("yeye").isEmpty())
		eyeY = eyeData.value("yeye").at(0);
	else
		eyeY = 0.0;

	QString eyePos = QString("(%1, %2)").arg(eyeX).arg(eyeY);


	pixmapVisualTarget->drawNonExperimentText(QFont("Arial",18),
											  QColor(Qt::white),
											  QRect(50,100,300,50),
											  Qt::AlignCenter,
											  eyePos);

	pixmapVisualTarget->draw(QPoint((int) eyeX,(int) eyeY), circleGraphic->getCompositingSurface("Pixmap"));

	//Present the image
	//-----------------

	pixmapVisualTarget->present();

}

