#include <QRect>
#include <QMap>
#include <QList>
#include <QString>

#include "SignalChannelTest.h"
#include "../common/memleakdetect.h"

SignalChannelTest::SignalChannelTest()
{
}

void SignalChannelTest::exec()
{
	//stall to allow us to attach debugger remotely
	//while(1);

	//set up graphics
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	d3dVisualTarget = QSharedPointer<Picto::D3DVisualTarget>(new Picto::D3DVisualTarget());

	Picto::RenderingTarget renderingTarget(d3dVisualTarget, pcmAuralTarget);

	d3dVisualTarget->show();

	frameCounter = 0;

	//set up mouse channel
	mouseChannel = new Picto::MouseSignalChannel(120, d3dVisualTarget.data());
	mouseChannel->start();

	//set up ai channels
	aiChannel = new Picto::PictoBoxXPAnalogInputSignalChannel(250);
	aiChannel->addAiChannel("xeye",1);
	aiChannel->addAiChannel("yeye",0);

	QRect visualTargetRect = d3dVisualTarget->getDimensions();
	aiChannel->setCalibrationCoefficientsFromRange("xeye", 0.0,10.0,0.0,(double)visualTargetRect.width());
	aiChannel->setCalibrationCoefficientsFromRange("yeye", 0.0,10.0,0.0,(double)visualTargetRect.height());
	aiChannel->start();

	QSharedPointer<Picto::CompositingSurface> compositingSurface = 
		renderingTarget.generateCompositingSurface();

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

	d3dVisualTarget->drawNonExperimentText(QFont("Arial",18),
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

	QString eyePos = QString("(%1, %2)").arg(eyeX,0,'f',1).arg(eyeY,0,'f',1);


	d3dVisualTarget->drawNonExperimentText(QFont("Arial",18),
											  QColor(Qt::white),
											  QRect(50,100,300,50),
											  Qt::AlignCenter,
											  eyePos);

	//d3dVisualTarget->draw(QPoint((int) eyeX,(int) eyeY), circleGraphic->getCompositingSurface("Direct3D"));
	d3dVisualTarget->draw(QPoint((int) xpos,(int) ypos),QPoint(), circleGraphic->getCompositingSurface("Direct3D"));
	//d3dVisualTarget->draw(QPoint(200,200), circleGraphic->getCompositingSurface("Direct3D"));

	//Present the image
	//-----------------

	d3dVisualTarget->present();

}

