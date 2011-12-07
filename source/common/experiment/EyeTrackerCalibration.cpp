#include "EyeTrackerCalibration.h"
#include "../memleakdetect.h"

namespace Picto
{
EyeTrackerCalibration::EyeTrackerCalibration()
{
}

EyeTrackerCalibration::~EyeTrackerCalibration()
{
}
//This was originally written as CE specific code.
//That was a mistake, and it needs to be fixed (probably
//by creating a more generic analog input channel object),
//but for the time being, I am simply killing the code entirely
/*void EyeTrackerCalibration::RunAutoCalibration(QSharedPointer<VisualTarget> visualTarget,
								QSharedPointer<RewardController> rewardController,
								QSharedPointer<Picto::PictoBoxDaqBoard> daqBoard)
{


	//Set up data storage
	//------------------------------------------------
	QMap<int, QList<QPoint> > inputVoltageMap;

	//Set up graphics
	//--------------------------------------------------
	RenderingTarget renderingTarget(visualTarget, QSharedPointer<AuralTarget>());
	
	QSharedPointer<Picto::CompositingSurface> compositingSurface;

	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::BoxGraphic> boxGraphic(
		new Picto::BoxGraphic(QPoint(0,0),QRect(0,0,20,20),QColor(255,255,0,255)));
	boxGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	//find coordinates for box positions
	QRect visualTargetRect = visualTarget->getDimensions();
	int centerX = visualTargetRect.center().x();
	int centerY = visualTargetRect.center().y();
	int radius = visualTargetRect.height()/4;

	//Set up Signal Channel
	//-------------------------------------------------
	Picto::PictoBoxAnalogInputSignalChannel *eyeChannel;
	eyeChannel = new Picto::PictoBoxAnalogInputSignalChannel(daqBoard,250);
	eyeChannel->addAiChannel("xeye",0);
	eyeChannel->addAiChannel("yeye",1);
	eyeChannel->start();

	//Run through calibration routine
	//------------------------------------------------
	QList<QPoint> targetCoordList;
	bool calComplete = false;
	int calAttempt = 0;
	QTime time;

	while(!calComplete && calAttempt < 3)
	{
		//(re)populate the target list
		targetCoordList.push_back(visualTargetRect.center());
		targetCoordList.push_back(QPoint(centerX, centerY+radius));
		targetCoordList.push_back(QPoint(centerX, centerY-radius));
		targetCoordList.push_back(QPoint(centerX+radius, centerY));
		targetCoordList.push_back(QPoint(centerX-radius, centerY));
		

		//display each of the 5 targets for 600ms, deliver a reward, and display 
		//the target for an additional 600ms
		int displayTime = 1200;
		while(!targetCoordList.empty())
		{
			int targetIndex = rand()%targetCoordList.length();
			QPoint targetCoord = targetCoordList.takeAt(targetIndex);
			visualTarget->draw(targetCoord,boxGraphic->getCompositingSurface(visualTarget->getTypeName()));
			visualTarget->present();

			//wait for half the delay time
			time.start();
			while(time.elapsed() < displayTime/2);

			//collect the final value from the eye tracker (assuming
			//that it is reasonably correct) and store it in the 
			//input voltage map (indexed using the same values as the 
			//targetCoordList)
			QMap<QString,QList<double> > eyeData;
			eyeData = eyeChannel->getValues();
			QPoint newPoint(eyeData.value("xpos").last(),
							eyeData.value("ypos").last());

			//****************************
			//  THIS IS UNFINISHED CODE
			//****************************
			//inputVoltageMap.value(targetIndex).push_back(newPoint);

			
			rewardController->giveReward(1);

			visualTarget->draw(targetCoord,boxGraphic->getCompositingSurface(visualTarget->getTypeName()));
			visualTarget->present();

			//wait for half the delay time
			time.start();
			while(time.elapsed() < displayTime/2);

		}


		calAttempt++;
	}

}	*/



} //namespace Picto