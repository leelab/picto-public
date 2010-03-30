#include <QApplication>
#include <QWidget>
#include <QDesktopWidget>
#include <QPainter>
#include <QTranslator>
#include <QSharedPointer>
#include <QTime>
#include <QMessageBox>
#include <QNetworkInterface>

#include "../common/globals.h"

#include "../common/network/ServerDiscoverer.h"
#include "HardwareSetup.h"


#include "../common/engine/PictoEngine.h"

int main(int argc, char *argv[])
{

	///////////////////////////////////////
	// Setup app
	///////////////////////////////////////
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

	QSharedPointer<Picto::Engine::PictoEngine> engine(new Picto::Engine::PictoEngine());

	//! \TODO Set up random number generator?
	
	///////////////////////////////////////
	// Setup hardware
	///////////////////////////////////////
	HardwareSetup hwSetup(engine);
	
	hwSetup.setupFactories();
	if(!hwSetup.setupRenderingTargets(HardwareSetup::D3D)) 
		return -1;
	if(!hwSetup.setupSignalChannel(HardwareSetup::Mouse)) 
		return -1;
	if(!hwSetup.setupRewardController(HardwareSetup::NullReward)) 
		return -1;
	if(!hwSetup.setupEventCodeGenerator(HardwareSetup::NullGen)) 
		return -1;

	if(!hwSetup.isSetup())
		return -1;

	///////////////////////////////////////
	// Setup networking
	///////////////////////////////////////
	//Find a server and open a command channel to it
	Picto::ServerDiscoverer serverDiscoverer;
	QSharedPointer<Picto::CommandChannel> serverChannel(new Picto::CommandChannel);
	serverChannel->pollingMode(true);

	serverDiscoverer.discover();

	while(!serverDiscoverer.waitForDiscovered(10000))
	{
		QMessageBox notFound;
		notFound.setText("No PictoServer found on the network.  PictoDirector will try again.");
		notFound.setIcon(QMessageBox::Critical);
		notFound.exec();
	}

	QHostAddress serverAddr = serverDiscoverer.getAddress();
	quint16 port = serverDiscoverer.getPort();

	serverChannel->connectToServer(serverAddr, port);

	Q_ASSERT(serverChannel->getChannelStatus() == Picto::CommandChannel::connected);

	engine->setCommandChannel(serverChannel);


	//Figure out our own address
	QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();
	QHostAddress directorAddress;

	//Use the first IPv4 address that isn't localhost
	//This will probably be a valid ip address, but there could still be issues...
	foreach(QHostAddress addr, hostAddresses)
	{
		QString blah = addr.toString();
		if(addr.protocol() == QAbstractSocket::IPv4Protocol && addr != QHostAddress::LocalHost)
		{
			directorAddress.setAddress(addr.toIPv4Address());
			break;
		}
	}

	///////////////////////////////////////
	// Run event loop
	///////////////////////////////////////

	//Once we have the command channel set up, we'll sit in a tight loop sending
	//STATUS commands at 1Hz and processing the responses forever.
	QString statusCommandStr = "DIRECTORUPDATE idle PICTO/1.0";
	QSharedPointer<Picto::ProtocolCommand> statusCommand(new Picto::ProtocolCommand(statusCommandStr));
	QSharedPointer<Picto::ProtocolResponse> statusResponse;
	forever
	{
		statusResponse = engine->sendCommand(statusCommand,100);

		if(statusResponse.isNull() || statusResponse->getResponseType() != "OK")
		{
			continue;
		}

		QString statusDirective = statusResponse->getDecodedContent();

		if(statusDirective.startsWith("OK"))
		{
			//We don't need to do anything if the status is "OK"
		}
		else if(statusDirective.startsWith("LOADEXP"))
		{
			//Extract the XML from the response
			int xmlIdx = statusDirective.indexOf('\n');
			statusDirective.remove(0,xmlIdx);

			//Load the experiment
			QSharedPointer<QXmlStreamReader> xmlStreamReader(new QXmlStreamReader(statusDirective));
			
			QSharedPointer<Picto::Experiment> experiment;
			experiment->deserializeFromXml(xmlStreamReader);

			engine->loadExperiment(experiment);

		}
		else if(statusDirective.startsWith("START"))
		{
			//Start running a task
		}
		else
		{
			Q_ASSERT(false);
		}

		//Pause for 1 second
		QTime pauseTimer;
		pauseTimer.start();
		while(pauseTimer.elapsed() < 1000);
	}



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
