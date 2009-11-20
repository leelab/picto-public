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

#include "SignalChannelTest.h"

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

	/*! \todo this should create a target surface (optionally windowed if a -testing argument is present)
	 *        set it to all black with a logo and status message (the status message should be reflected
	 *        on the LCD display if present).  The director instance should attempt to discover and
	 *        connect a persistent command channel to a picto server.  If this fails, it should retry
	 *        until successful.  In the future we'll allow for configuring a direct connection manually.
	 */

	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget(new Picto::PixmapVisualTarget(bWindowed));
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());

	Picto::RenderingTarget renderingTarget(pixmapVisualTarget, pcmAuralTarget);
	//renderingTarget.showSplash();
	//renderingTarget.updateStatus(QString("Searching for a %1").arg(Picto::Names->serverAppName));
	//renderingTarget.show();

	pixmapVisualTarget->show();

	/*! \todo PictoEngine pictoEngine; */
//////////////////////////////
	QSharedPointer<Picto::CompositingSurface> compositingSurface = 
		renderingTarget.generateCompositingSurface();

	QSharedPointer<Picto::CircleGraphic> circleGraphic(
											new Picto::CircleGraphic(QPoint(10,10),100,QColor(0,255,0,200)));
	circleGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	compositingSurface = renderingTarget.generateCompositingSurface();

	QSharedPointer<Picto::PictureGraphic> pictureGraphic(
											new Picto::PictureGraphic(QPoint(0,0),":/common/images/splash.png"));
	pictureGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
	QRect splashScreenRect = pictureGraphic->getBoundingRect();
	QRect visualTargetRect = pixmapVisualTarget->getDimensions();
	pictureGraphic->setPosition(QPoint((visualTargetRect.width()-splashScreenRect.width())/2,
								       (visualTargetRect.height()-splashScreenRect.height())/2));

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
		new Picto::LineGraphic(QPoint(0,0),points,QColor(255,0,0,255)));
	lineGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	compositingSurface = renderingTarget.generateCompositingSurface();

	QRect dimensions(0,0,200,150);
	QSharedPointer<Picto::BoxGraphic> boxGraphic(
		new Picto::BoxGraphic(QPoint(550,400),dimensions,QColor(0,0,255,255)));
	boxGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);

	pixmapVisualTarget->draw(pictureGraphic->getPosition(), pictureGraphic->getCompositingSurface("Pixmap"));
	pixmapVisualTarget->draw(circleGraphic->getPosition(), circleGraphic->getCompositingSurface("Pixmap"));
	pixmapVisualTarget->draw(lineGraphic->getPosition(), lineGraphic->getCompositingSurface("Pixmap"));
	pixmapVisualTarget->draw(boxGraphic->getPosition(), boxGraphic->getCompositingSurface("Pixmap"));

	pixmapVisualTarget->drawNonExperimentText(QFont("Arial",18),
											  QColor(Qt::white),
											  QRect(0,			  
												   (visualTargetRect.height()-splashScreenRect.height())/2+splashScreenRect.height()+25,
												   visualTargetRect.width(),
												   50),
											  Qt::AlignCenter,
											  "Testing");

	pixmapVisualTarget->present();

	/*************************************************
	 * Testing the signal channel
	 *************************************************/
	SignalChannelTest sigChanTest(pixmapVisualTarget);

//////////////////////////////
/*
void PixmapRenderingTarget::foundServer(QHostAddress serverAddress, quint16 serverPort)
{
	updateStatus(QString("Found server at %1:%2").arg(serverAddress.toString()).arg(serverPort));
}

void PixmapRenderingTarget::discoverServerFailed()
{
	updateStatus("Server discover failed");
}
*/

	Picto::ServerDiscoverer serverDiscoverer;
	//QObject::connect(&serverDiscoverer, SIGNAL(foundServer(QHostAddress, quint16)), &renderingTarget, SLOT(foundServer(QHostAddress, quint16)));
	//QObject::connect(&serverDiscoverer, SIGNAL(discoverFailed()), &renderingTarget, SLOT(discoverServerFailed()));

	serverDiscoverer.discover();

	/*! \todo DirectorCommandChannel directorCommandChannel; */
	
	int result = app.exec();

	Picto::CloseLib();

	return result;
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
