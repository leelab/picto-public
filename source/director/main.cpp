#include <QApplication>
#include <QWidget>
#include <QDesktopWidget>
#include <QPainter>
#include <QTranslator>
#include <QSharedPointer>
#include <QTimer>

#include "../common/globals.h"

#include "../common/network/ServerDiscoverer.h"

#include "../common/compositor/RenderingTarget.h"
#include "../common/compositor/PixmapVisualTarget.h"
#include "../common/compositor/PCMAuralTarget.h"
#include "../common/compositor/CompositingSurface.h"
#include "../common/compositor/MixingSample.h"

#include "../common/stimuli/CircleGraphic.h"
#include "../common/stimuli/PictureGraphic.h"
#include "../common/stimuli/LineGraphic.h"
#include "../common/stimuli/BoxGraphic.h"
#include "../common/stimuli/ArrowGraphic.h"
#include "../common/stimuli/EllipseGraphic.h"
#include "../common/stimuli/RandomlyFilledGridGraphic.h"

#include "../common/experiment/RewardCalibration.h"
#include "../common/experiment/EyeTrackerCalibration.h"

#include "EngineTest.h"



void TestPixmapRendering();

#ifdef WINCE
	#include "../common/compositor/DDrawVisualTarget.h"
	#include "../common/compositor/D3DMVisualTarget.h"
	#include "../common/iodevices/PictoBoxRewardControllerCE.h"
	void TestD3DMRendering();
	void TestVSync();
	void TestEyeTrackerCalibration();
#endif

#ifdef WIN32
	#include "../common/compositor/D3DVisualTarget.h"
	#include "../common/iodevices/PictoBoxXPRewardController.h"
	#include "../common/iodevices/PictoBoxXPEventCodeGenerator.h"
	#include "SignalChannelTest.h"

	void TestD3DRendering();
	void TestRewardCalibration();
	void TestEventGeneration();
#endif

int main(int argc, char *argv[])
{

	bool bWindowed = false;

	if(argc>1)
	{
		QString arg = argv[1];
		if(arg.toLower() == "-testing")
		{
			bWindowed = true;
		}
	}

	QApplication app(argc, argv);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	QTranslator appTranslator;
	appTranslator.load(":/translations/director_" + localeLanguageCode + ".qm");
	app.installTranslator(&appTranslator);

	Picto::InitializeLib(&app,localeLanguageCode);

	//This is probably not the recommended seeding approach, but I was in a hurry...
	srand(QTime::currentTime().msecsTo(QTime(0,0)));


/*************************************************
 * Testing stuff
 *************************************************/
	EngineTest engineTest;
	engineTest.exec();
	//TestPixmapRendering();

#ifdef WINCE
	//TestRendering2();
#endif

#ifdef WIN32
	//TestD3DRendering();
#endif

	//TestEventGeneration();

	//SignalChannelTest sigChanTest;
	//sigChanTest.exec();

	//TestVSync();

	//TestRewardCalibration();
	//TestEyeTrackerCalibration();

	//Picto::ServerDiscoverer serverDiscoverer;
	//QObject::connect(&serverDiscoverer, SIGNAL(foundServer(QHostAddress, quint16)), &renderingTarget, SLOT(foundServer(QHostAddress, quint16)));
	//QObject::connect(&serverDiscoverer, SIGNAL(discoverFailed()), &renderingTarget, SLOT(discoverServerFailed()));

	//serverDiscoverer.discover();

	/*! \todo DirectorCommandChannel directorCommandChannel; */

	
	//int result = app.exec();

	Picto::CloseLib();

	//return result;
	//return 0;
}

/*! \todo PictoEngine object which loads an experiment object and can execute its contained tasks rendering to one or
 *        more RenderingTarget derived objects.  The PictoEngine can be set to have precise timing control, or to ignore
 *        frame intervals (to catch up a playback, allow for fast forward and rewind behaviors, enable generation of movie
 *        files, allow analysis of replayed data, etcetera).  The storage for behavioral data must accomodate out of
 *        order receipt.  Additionally the PictoEngine can be assigned a CommandChannel with which to communicate with
 *        a PictoServer instance.
 *        
 *        PictoEngine objects will have an exclusive mode in which they enable precise timing, boost their priority and
 *        otherwise attempt to monopolize the system's resources so as to provide maximum fidelity.  If not in exclusive
 *        mode, then multiple PictoEngine instances can be running simultaneously.  This enables monitoring several
 *        experiments simultaneously.
 *        
 *        Once per frame, the PictoEngine pipeline will collect behavioral data.  If in exclusive or testing mode, the
 *        PictoEngine will query behavioral data from directly attached sources.  In addition, any remote commands or
 *        behavioral data received via the CommandChannel will be processed.  Behavioral data received in this fashion
 *        will be processed normally with the exception of triggering state transitions (which will arrive as their own
 *        commands via the CommandChannel.  It is important to note that this is bi-directional, even for exclusive
 *        mode PictoEngine objects.  This is because investigators can trigger commands remotely as well as provide
 *        "behavioral" data (e.g. to map a receptive field).
 *        
 *        When the CommandChannel object receives instructions, it will set a signal that pending instructions are
 *        available for processing.  During the PictoEngine's execution pipeline, it will check for available instructions
 *        and then invoke the corresponding engine methods to handle them.  Similarly, during the execution pipeline,
 *        the PictoEngine will transmit received data via the CommandChannel.
 *        
 *        The CommandChannel object abstracts the underlying connection layers, and can be used by directly accessing
 *        its members to send/receive commands and data when no actual network connection is present (e.g. during testing).
 *        
 *        Each RenderingTarget will contain a VisualTarget object and an AuralTarget object both of which shall act as
 *        factory classes for elements that can then be combined by the renderer to produce the final output.
 *        
 *        Each VisualElement object will render in pure software for reproducability to a QImage which will then be
 *        passed to contained CompositingSurface objects.  Each CompositingSurface is generated by a RenderingTarget
 *        and assigned to the VisualElement upon construction.  If no CompositingSurface is provided, a pure software
 *        CompositingSurface will be used.  Each RenderingTarget can select the type of composition most appropriate to
 *        it, be that pure software (e.g. for the web interface, movie file generation, printing, etcetera), pixmap based
 *        (e.g. for the workstation), D3D based (e.g. for the director on a full Windows machine), DirectDraw based
 *        (e.g. for the director on a PictoBox), or null based (e.g. for analysis).
 *        
 *        Each AudioElement object will render in pure software for reproducability to a QByteArray as 16 bit uncompressed
 *        waveform audio which will then be passed to contained MixingSample objects.  Each MixingSample is generated
 *        by a RenderingTarget and assigned to the AudioElement upon construction.  If no MixingSample is provided, an
 *        uncompressed raw 16bit wave format will be used.
 *        
 *        When a PictoEngine is not executing, it will render a splash screen to the RenderingTarget.
 */



void TestPixmapRendering()
{
	/*! \todo this should create a target surface (optionally windowed if a -testing argument is present)
	 *        set it to all black with a logo and status message (the status message should be reflected
	 *        on the LCD display if present).  The director instance should attempt to discover and
	 *        connect a persistent command channel to a picto server.  If this fails, it should retry
	 *        until successful.  In the future we'll allow for configuring a direct connection manually.
	 */
	bool bWindowed = true;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget(new Picto::PixmapVisualTarget(bWindowed));
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());

	Picto::RenderingTarget renderingTarget(pixmapVisualTarget, pcmAuralTarget);
	//renderingTarget.showSplash();
	//renderingTarget.updateStatus(QString("Searching for a %1").arg(Picto::Names->serverAppName));
	//renderingTarget.show();

	pixmapVisualTarget->show();

	/*! \todo PictoEngine pictoEngine; */
//////////////////////////////
	QSharedPointer<Picto::CompositingSurface> compositingSurface;
	
	//CIRCLE
	printf("\nCircle\n");
	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::CircleGraphic> circleGraphic(
											new Picto::CircleGraphic(QPoint(50,50),100,QColor(0,255,0,127)));
	circleGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	//PICTURE
	printf("\nPicture\n");
	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::PictureGraphic> pictureGraphic(
											new Picto::PictureGraphic(QPoint(0,0),":/common/images/splash.png"));
	pictureGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
	QRect splashScreenRect = pictureGraphic->getBoundingRect();
	QRect visualTargetRect = pixmapVisualTarget->getDimensions();
	pictureGraphic->setPosition(QPoint((visualTargetRect.width()-splashScreenRect.width())/2,
								       (visualTargetRect.height()-splashScreenRect.height())/2));

	//LINE
	printf("\nLine\n");
	compositingSurface = renderingTarget.generateCompositingSurface();
	QVector<QPoint> points;
	points.push_back(QPoint(10,10));
	points.push_back(QPoint(790,590));
	points.push_back(QPoint(10,590));
	points.push_back(QPoint(790,10));
	points.push_back(QPoint(10,10));
	points.push_back(QPoint(10,590));
	points.push_back(QPoint(790,590));
	points.push_back(QPoint(790,10));
	QSharedPointer<Picto::LineGraphic> lineGraphic(
		new Picto::LineGraphic(QPoint(50,50),points,QColor(255,0,0,127)));
	lineGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	//BOX
	printf("\nBox\n");
	compositingSurface = renderingTarget.generateCompositingSurface();
	QRect dimensions(0,0,100,100);
	QSharedPointer<Picto::BoxGraphic> boxGraphic(
		new Picto::BoxGraphic(QPoint(1,1),dimensions,QColor(255,0,0,255)));
	boxGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	printf("\nBox2\n");
	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::BoxGraphic> boxGraphic2(
		new Picto::BoxGraphic(QPoint(0,0),QRect(0,0,200,200),QColor(255,255,255,255)));
	boxGraphic2->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	printf("\nBox3\n");
	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::BoxGraphic> boxGraphic3(
		new Picto::BoxGraphic(QPoint(0,0),QRect(0,0,20,500),QColor(255,255,255,255)));
	boxGraphic3->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	//ARROW
	printf("\nArrow\n");
	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::ArrowGraphic> arrowGraphic1(
		new Picto::ArrowGraphic(QPoint(0,0),QPoint(400,400),QPoint(300,500),1,QColor(255,255,0,255)));
	arrowGraphic1->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	//ELLIPSE
	printf("\nEllipse\n");
	compositingSurface = renderingTarget.generateCompositingSurface();
	dimensions = QRect(0,0,200,75);
	QSharedPointer<Picto::EllipseGraphic> ellipseGraphic(
		new Picto::EllipseGraphic(QPoint(10,85),dimensions,QColor(0,0,255,255)));
	ellipseGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);


	pixmapVisualTarget->draw(pictureGraphic->getPosition(), pictureGraphic->getCompositingSurface("Pixmap"));
	pixmapVisualTarget->draw(circleGraphic->getPosition(), circleGraphic->getCompositingSurface("Pixmap"));
	pixmapVisualTarget->draw(lineGraphic->getPosition(), lineGraphic->getCompositingSurface("Pixmap"));
	pixmapVisualTarget->draw(boxGraphic->getPosition(), boxGraphic->getCompositingSurface("Pixmap"));
	pixmapVisualTarget->draw(arrowGraphic1->getPosition(), arrowGraphic1->getCompositingSurface("Pixmap"));
	pixmapVisualTarget->draw(ellipseGraphic->getPosition(), ellipseGraphic->getCompositingSurface("Pixmap"));

	pixmapVisualTarget->drawNonExperimentText(QFont("Arial",18),
											  QColor(Qt::white),
											  QRect(0,			  
												   (visualTargetRect.height()-splashScreenRect.height())/2+splashScreenRect.height()+25,
												   visualTargetRect.width(),
												   50),
											  Qt::AlignCenter,
											  "Testing");

	pixmapVisualTarget->present();

}
#ifdef WINCE



	printf("\nEND: event code generation\n\n");
}

void TestVSync()
{
	enum{
		IOCTRL_VSYNC,
		IOCTRL_WAIT_VBLANK_START,
		IOCTRL_WAIT_VBLANK_END
	};

	BYTE outBuf;
	DWORD bytesReturned;

	HANDLE hCRT;
	SetLastError(0);
	hCRT = CreateFile(_T("CRT1:"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,0);

	printf("CreateFile error: %d\n", GetLastError());
	if(!hCRT)
	{
		printf("Failed to create file CRT1\n");
		return;
	}

	LARGE_INTEGER ticksPerMs;
	LARGE_INTEGER tick, tock;
	double elapsedTimeArr[30];

	QueryPerformanceFrequency(&ticksPerMs);
	ticksPerMs.LowPart /= 1000;

	QueryPerformanceCounter(&tick);

	//Find 30 vsync events and record thier times
	for(int i=0; i<30; i++)
	{
		outBuf = 0;
		while(outBuf != 1)
		{
			DeviceIoControl(hCRT,IOCTRL_VSYNC,NULL,0,&outBuf,1,&bytesReturned,NULL);
		}
		QueryPerformanceCounter(&tock);
		elapsedTimeArr[i] = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerMs.LowPart);

	}

	for(int i=0; i<30; i++)
	{
		printf("In vsync at %f ms\n", elapsedTimeArr[i]);
	}

	//Time the duration of a frame using vsync start and vsync start
	DeviceIoControl(hCRT,IOCTRL_WAIT_VBLANK_START,NULL,0,&outBuf,1,&bytesReturned,NULL);
	QueryPerformanceCounter(&tick);
	DeviceIoControl(hCRT,IOCTRL_WAIT_VBLANK_START,NULL,0,&outBuf,1,&bytesReturned,NULL);
	QueryPerformanceCounter(&tock);
	elapsedTimeArr[0] = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerMs.LowPart);
	printf("\nFrame time using VBLANK_START: %f ms\n", elapsedTimeArr[0]);

	//Time the duration of a frame using vsync start and vsync end
	DeviceIoControl(hCRT,IOCTRL_WAIT_VBLANK_END,NULL,0,&outBuf,1,&bytesReturned,NULL);
	QueryPerformanceCounter(&tick);
	DeviceIoControl(hCRT,IOCTRL_WAIT_VBLANK_END,NULL,0,&outBuf,1,&bytesReturned,NULL);
	QueryPerformanceCounter(&tock);
	elapsedTimeArr[0] = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerMs.LowPart);
	printf("\nFrame time using IOCTRL_WAIT_VBLANK_END: %f ms\n", elapsedTimeArr[0]);

	//Time the duration of the blanking period
	DeviceIoControl(hCRT,IOCTRL_WAIT_VBLANK_END,NULL,0,&outBuf,1,&bytesReturned,NULL);
	QueryPerformanceCounter(&tick);
	DeviceIoControl(hCRT,IOCTRL_WAIT_VBLANK_START,NULL,0,&outBuf,1,&bytesReturned,NULL);
	QueryPerformanceCounter(&tock);
	elapsedTimeArr[0] = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerMs.LowPart);
	printf("\nDuration of blanking: %f ms\n", elapsedTimeArr[0]);
}


void TestRewardCalibration()
{
	Picto::PictoBoxRewardController rewardController(1);
	Picto::RewardCalibration rewCal(&rewardController);
	rewCal.RunCalibration(10,250);
}


void TestEyeTrackerCalibration()
{
	QSharedPointer<Picto::PictoBoxRewardController> rewardController(new Picto::PictoBoxRewardController(1));
	QSharedPointer<Picto::D3DMVisualTarget> d3dmVisualTarget(new Picto::D3DMVisualTarget());
	QSharedPointer<Picto::PictoBoxDaqBoard> daqBoard(new Picto::PictoBoxDaqBoard());

	Picto::EyeTrackerCalibration eyeCal;
	eyeCal.RunAutoCalibration(d3dmVisualTarget,rewardController, daqBoard);
}

//This rendering test uses D3DM and multiple sprites.
//At least one of the sprites will be animated.

void TestD3DMRendering()
{
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	QSharedPointer<Picto::D3DMVisualTarget> d3dmVisualTarget(new Picto::D3DMVisualTarget());

	Picto::RenderingTarget renderingTarget(d3dmVisualTarget, pcmAuralTarget);

	d3dmVisualTarget->show();

	QSharedPointer<Picto::CompositingSurface> compositingSurface;

	//Generate 6 circles (3 top, 3 bottom)
	QRect visualTargetRect = d3dmVisualTarget->getDimensions();

	QList<QSharedPointer<Picto::CircleGraphic> > circleList;
	for(int i=0; i<6; i++)
	{
		QPoint circleLocation = visualTargetRect.center();
		int radius = 10;
		circleLocation.setX(circleLocation.x()-radius*4 + radius*3*(i%3));
		circleLocation.setY(circleLocation.y()+16*radius - (i/3)*40*radius);
		compositingSurface = renderingTarget.generateCompositingSurface();
		QSharedPointer<Picto::CircleGraphic> circleGraphic(
												new Picto::CircleGraphic(circleLocation,radius,QColor(0,255,0,255)));
		circleGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
		circleList.push_back(circleGraphic);
	}
	//Generate 4 large ish boxes
	QList<QSharedPointer<Picto::BoxGraphic> > boxList;
	for(int i=0; i<4; i++)
	{
		QPoint boxLocation = visualTargetRect.center();
		int side = 180;
		boxLocation.setX(boxLocation.x()-side*2+side*3*(i%2));
		boxLocation.setY(boxLocation.y()+side - (i/2)*3*side);
		compositingSurface = renderingTarget.generateCompositingSurface();
		QSharedPointer<Picto::BoxGraphic> boxGraphic(
												new Picto::BoxGraphic(boxLocation,QRect(0,0,side,side),QColor(255,255,0,255)));
		boxGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
		boxList.push_back(boxGraphic);
	}

	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::BoxGraphic> animatedBoxGraphic(
											new Picto::BoxGraphic(QPoint(0,0),QRect(0,0,20,1000),QColor(0,0,255,255)));
	animatedBoxGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);


	//generate a randomly filled grid
	int side = 200;
	QPoint randomGridLocation = visualTargetRect.center();
	randomGridLocation.setX(randomGridLocation.x() - side/2);
	randomGridLocation.setY(randomGridLocation.y() - side/2);
	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::RandomlyFilledGridGraphic> randomGridGraphic(
		new Picto::RandomlyFilledGridGraphic(randomGridLocation,QRect(0,0,side,side),QColor(255,0,0),QColor(0,0,255),10,10,50,true,1));
	randomGridGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);



	//My mini event loop to test framerate
	LARGE_INTEGER ticksPerSec;
	LARGE_INTEGER tick, tock;
	double elapsedTime[60];
	int frameCounter = 0;
	QueryPerformanceFrequency(&ticksPerSec);

	while(frameCounter < 600)
	{

		QueryPerformanceCounter(&tick);

		//draw the circles
		QSharedPointer<Picto::CircleGraphic> circle;
		foreach(circle, circleList)
		{
			circle->updateAnimation(frameCounter,QTime());
			d3dmVisualTarget->draw(circle->getPosition(), circle->getCompositingSurface("Direct3DMobile"));
		}

		//draw the boxes
		QSharedPointer<Picto::BoxGraphic> box;
		foreach(box, boxList)
		{
			box->updateAnimation(frameCounter,QTime());
			d3dmVisualTarget->draw(box->getPosition(), box->getCompositingSurface("Direct3DMobile"));
		}

		randomGridGraphic->updateAnimation(frameCounter,QTime());
		d3dmVisualTarget->draw(randomGridGraphic->getPosition(), randomGridGraphic->getCompositingSurface("Direct3DMobile"));

		animatedBoxGraphic->updateAnimation(frameCounter,QTime());
		d3dmVisualTarget->draw(QPoint((frameCounter%120)*10,0), animatedBoxGraphic->getCompositingSurface("Direct3DMobile"));
		

		d3dmVisualTarget->present();

		QueryPerformanceCounter(&tock);
		if(frameCounter % 10 == 0)
			elapsedTime[frameCounter/10] = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerSec.LowPart);
		
		frameCounter++;

	}

	for(int i=0; i<60; i++)
		printf("Frame %d elapsed time: %f\n", i*10, elapsedTime[i]);


}

#endif
#ifdef WIN32


void TestD3DRendering()
{
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	QSharedPointer<Picto::D3DVisualTarget> d3dVisualTarget(new Picto::D3DVisualTarget());

	Picto::RenderingTarget renderingTarget(d3dVisualTarget, pcmAuralTarget);

	d3dVisualTarget->show();

	QSharedPointer<Picto::CompositingSurface> compositingSurface;

	//Generate 6 circles (3 top, 3 bottom)
	QRect visualTargetRect = d3dVisualTarget->getDimensions();

	QList<QSharedPointer<Picto::CircleGraphic> > circleList;
	for(int i=0; i<6; i++)
	{
		QPoint circleLocation = visualTargetRect.center();
		int radius = 10;
		circleLocation.setX(circleLocation.x()-radius*4 + radius*3*(i%3));
		circleLocation.setY(circleLocation.y()+16*radius - (i/3)*40*radius);
		compositingSurface = renderingTarget.generateCompositingSurface();
		QSharedPointer<Picto::CircleGraphic> circleGraphic(
												new Picto::CircleGraphic(circleLocation,radius,QColor(0,255,0,255)));
		circleGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
		circleList.push_back(circleGraphic);
	}
	//Generate 4 large ish boxes
	QList<QSharedPointer<Picto::BoxGraphic> > boxList;
	for(int i=0; i<4; i++)
	{
		QPoint boxLocation = visualTargetRect.center();
		int side = 180;
		boxLocation.setX(boxLocation.x()-side*2+side*3*(i%2));
		boxLocation.setY(boxLocation.y()+side - (i/2)*3*side);
		compositingSurface = renderingTarget.generateCompositingSurface();
		QSharedPointer<Picto::BoxGraphic> boxGraphic(
												new Picto::BoxGraphic(boxLocation,QRect(0,0,side,side),QColor(255,255,0,255)));
		boxGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
		boxList.push_back(boxGraphic);
	}

	//animated box
	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::BoxGraphic> animatedBoxGraphic(
											new Picto::BoxGraphic(QPoint(0,0),QRect(0,0,20,1000),QColor(0,0,255,255)));
	animatedBoxGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	//Animated ellipse
	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::EllipseGraphic> animatedEllipseGraphic(
											new Picto::EllipseGraphic(QPoint(50,600),QRect(0,0,500,50),QColor(0,0,255,255)));
	animatedEllipseGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
	int ellipseWidth = 500;
	bool ellipseShrinking = true;


	//generate a randomly filled grid
	int side = 200;
	QPoint randomGridLocation = visualTargetRect.center();
	randomGridLocation.setX(randomGridLocation.x() - side/2);
	randomGridLocation.setY(randomGridLocation.y() - side/2);
	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::RandomlyFilledGridGraphic> randomGridGraphic(
		new Picto::RandomlyFilledGridGraphic(randomGridLocation,QRect(0,0,side,side),QColor(255,0,0),QColor(0,0,255),10,10,50,true,1));
	randomGridGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	//PictureGraphic
	compositingSurface = renderingTarget.generateCompositingSurface();
	QSharedPointer<Picto::PictureGraphic> pictureGraphic(
											new Picto::PictureGraphic(QPoint(0,500),":/common/images/splash.png"));
	pictureGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);


	//My mini event loop to test framerate
	LARGE_INTEGER ticksPerSec;
	LARGE_INTEGER tick, tock;
	QList<double> elapsedTimes;
	int frameCounter = 0;
	QueryPerformanceFrequency(&ticksPerSec);

	while(frameCounter < 600)
	{

		QueryPerformanceCounter(&tick);
		//draw some text
		//WARNING: Drawing text results in dropped frames!
		/*d3dVisualTarget->drawNonExperimentText(QFont("Arial",18),
											  QColor(Qt::white),
											  QRect(0,0,300,60),
											  Qt::AlignCenter,
											  "Testing");*/

		//draw the circles
		QSharedPointer<Picto::CircleGraphic> circle;
		foreach(circle, circleList)
		{
			circle->updateAnimation(frameCounter,QTime());
			d3dVisualTarget->draw(circle->getPosition(), circle->getCompositingSurface("Direct3D"));
		}

		//draw the boxes
		QSharedPointer<Picto::BoxGraphic> box;
		foreach(box, boxList)
		{
			box->updateAnimation(frameCounter,QTime());
			d3dVisualTarget->draw(box->getPosition(), box->getCompositingSurface("Direct3D"));
		}

		randomGridGraphic->updateAnimation(frameCounter,QTime());
		d3dVisualTarget->draw(randomGridGraphic->getPosition(), randomGridGraphic->getCompositingSurface("Direct3D"));

		//This gives us a bar moving across the screen
		animatedBoxGraphic->updateAnimation(frameCounter,QTime());
		d3dVisualTarget->draw(QPoint((frameCounter%120)*10,0), animatedBoxGraphic->getCompositingSurface("Direct3D"));

		//This gives us an animated Ellipse
		if(ellipseShrinking)
		{
			ellipseWidth -= 1;
			if(ellipseWidth <260)
				ellipseShrinking = false;
		}
		else
		{
			ellipseWidth += 1;
			if(ellipseWidth > 500)
				ellipseShrinking = true;
		}
		animatedEllipseGraphic->setWidth(ellipseWidth);
		
		animatedEllipseGraphic->updateAnimation(frameCounter,QTime());
		d3dVisualTarget->draw(animatedEllipseGraphic->getPosition(), animatedEllipseGraphic->getCompositingSurface("Direct3D"));

		//pictureGraphic->updateAnimation(frameCounter,QTime());
		//d3dVisualTarget->draw(pictureGraphic->getPosition(),pictureGraphic->getCompositingSurface("Direct3D"));

		d3dVisualTarget->present();

		QueryPerformanceCounter(&tock);
		elapsedTimes.append((double)(tock.LowPart-tick.LowPart)/(double)(ticksPerSec.LowPart));
		
		frameCounter++;
	}

	FILE* outFile;
	outFile = fopen("output.txt", "w");
	Q_ASSERT(outFile);
	for(int i=0; i<elapsedTimes.length(); i++)
		fprintf(outFile,"Frame %d elapsed time: %f\n", i, elapsedTimes[i]*1000);
	fclose(outFile);



}


void TestRewardCalibration()
{
	Picto::PictoBoxXPRewardController rewardController(4);
	Picto::RewardCalibration rewCal(&rewardController);
	rewCal.RunCalibration(10,1000);
}


void TestEventGeneration()
{
	Picto::PictoBoxXPEventCodeGenerator eventGen;

	LARGE_INTEGER ticksPerSec;
	LARGE_INTEGER tick, tock;
	double elapsedTime;

	int eventCounter = 0;

	 while(1)
	{
		eventGen.sendEvent(eventCounter);
		if(eventCounter >= 127 )
			eventCounter = 0;
		else
			eventCounter++;

		//wait for 250us (the stobe line goes high for 250 us, so this
		//should give us a nices square wave with period 500 us
		QueryPerformanceFrequency(&ticksPerSec);
		QueryPerformanceCounter(&tick);
		do
		{
			QueryPerformanceCounter(&tock);
			elapsedTime = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerSec.LowPart);
		}
		while(elapsedTime < 0.250);

	}
}
#endif