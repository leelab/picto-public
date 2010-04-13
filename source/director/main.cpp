#include <QApplication>
#include <QWidget>
#include <QDesktopWidget>
#include <QPainter>
#include <QTranslator>
#include <QSharedPointer>
#include <QTime>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>

#include "../common/globals.h"

#include "../common/network/ServerDiscoverer.h"
#include "HardwareSetup.h"


#include "../common/engine/PictoEngine.h"

int main(int argc, char *argv[])
{

	///////////////////////////////////////
	// Setup app
	///////////////////////////////////////
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
	// Open Configuration database, and configure as needed
	///////////////////////////////////////
    QSqlDatabase configDb = QSqlDatabase::addDatabase("QSQLITE","PictoDirectorConfigDatabase");
	configDb.setDatabaseName(QCoreApplication::applicationDirPath() + "/PictoDirector.config");
    configDb.open();

	QSqlQuery query(configDb);

	if(!configDb.tables().contains("directorinfo"))
	{
		query.exec("CREATE TABLE directorinfo "
						"(id int, "
						"key TEXT, "
						"value TEXT)");
		query.exec("INSERT INTO directorinfo (key,value) VALUES ('Name','unnamed')");
		QString test = query.lastError().text();
	}

	//possibly reset our name...
	QStringList args = app.arguments();
	int nameArgIdx = args.indexOf("-name");
	if(nameArgIdx > 0)
	{
		QString newName = args[nameArgIdx+1];

		query.prepare("UPDATE directorinfo SET value=:value WHERE key='Name'");
		query.bindValue(":value",newName);
		query.exec();
	}

	//figure out our name
	query.exec("SELECT value FROM directorinfo WHERE key='Name'");
	Q_ASSERT(query.next());
	QString name = query.value(0).toString();
	engine->setName(name);
	
	///////////////////////////////////////
	// Setup hardware
	///////////////////////////////////////
	HardwareSetup hwSetup(engine);
	
	if(!hwSetup.setupRenderingTargets(HardwareSetup::Pixmap)) 
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
		notFound.setText("No PictoServer found on the network.  Try again.?");
		notFound.setIcon(QMessageBox::Critical);
		notFound.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		int ret = notFound.exec();
		if(ret == QMessageBox::Cancel)
			return 0;
		else
			serverDiscoverer.discover();
	}

	QHostAddress serverAddr = serverDiscoverer.getAddress();
	quint16 port = serverDiscoverer.getPort();

	serverChannel->connectToServer(serverAddr, port);

	Q_ASSERT(serverChannel->getChannelStatus() == Picto::CommandChannel::connected);

	engine->setCommandChannel(serverChannel);

	///////////////////////////////////////
	// Run event loop
	///////////////////////////////////////

	//Put up our splash screen
	QList<QSharedPointer<Picto::RenderingTarget> > renderingTargets = engine->getRenderingTargets();
	foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
	{
		target->updateStatus("PictoDirector started");
		target->showSplash();
	}

	//Once we have the command channel set up, we'll sit in a tight loop sending
	//DIRECTORUPDATE commands at 1Hz and processing the responses forever.
	QSharedPointer<Picto::ProtocolResponse> updateResponse;
	QString updateCommandStr = "DIRECTORUPDATE "+engine->getName()+":idle PICTO/1.0";
	QSharedPointer<Picto::ProtocolCommand> updateCommand(new Picto::ProtocolCommand(updateCommandStr));
	forever
	{
		updateResponse = engine->sendCommand(updateCommand,5000);

		if(updateResponse.isNull() || updateResponse->getResponseType() != "OK")
		{
			/*QMessageBox netError;
			netError.setText("Network error.  PictoDirector will exit.");
			netError.setIcon(QMessageBox::Critical);
			netError.exec();*/
			break;
		}

		QString statusDirective = updateResponse->getDecodedContent();

		if(statusDirective.startsWith("OK"))
		{
			//We don't need to do anything if the status is "OK"
		}
		else if(statusDirective.startsWith("NEWSESSION"))
		{
			QString sessionIdStr = statusDirective.mid(statusDirective.indexOf(' ')+1);
			engine->setSessionId(QUuid(sessionIdStr));
		}
		else if(statusDirective.startsWith("LOADEXP"))
		{
			//Extract the XML from the response
			int xmlIdx = statusDirective.indexOf('\n');
			statusDirective.remove(0,xmlIdx+1);

			//Load the experiment
			QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(statusDirective.toUtf8()));
			QSharedPointer<Picto::Experiment> experiment(new Picto::Experiment);

			xmlReader->readNext();
			while(!xmlReader->atEnd() && xmlReader->name().toString() != "Experiment")
			{
				QString test = xmlReader->name().toString();
				xmlReader->readNext();
			}

			if(!experiment->deserializeFromXml(xmlReader) || !engine->loadExperiment(experiment))
			{
				foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
				{
					target->updateStatus(QString("Error loading experiment: %1").arg(experiment->getErrors()));
					target->showSplash();
				}
			}
			else
			{
				foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
				{
					target->updateStatus("Loaded experiment, Session ID: " + engine->getSessionId().toString());
					target->showSplash();
				}

			}


		}
		else if(statusDirective.startsWith("START"))
		{
			//Start running a task
			QString taskName = statusDirective.mid(statusDirective.indexOf(' ')+1);
			
			foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
			{
				target->updateStatus("Starting task: " + taskName);
				target->showSplash();
			}

			//Before we actually start running the task, we need to send a DIRECTORUPDATE
			//command to let the server know that we have changed status to running
			//This also lets Workstation know that we received the command that they
			//initiated.  Note that if there is a directive in the response, it will be
			//ignored.  I don't think this is a problem, since there would be no reason
			//to send a second directive immediately after the first.
			QString runningUpdateCommandStr = "DIRECTORUPDATE "+name+":running PICTO/1.0";
			QSharedPointer<Picto::ProtocolCommand> runningUpdateCommand(new Picto::ProtocolCommand(runningUpdateCommandStr));
			QSharedPointer<Picto::ProtocolResponse> runningUpdateResponse;
			runningUpdateResponse = engine->sendCommand(runningUpdateCommand,5000);

			//start running our task
			engine->runTask(taskName);

			//upon completion return to splash screen
			foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
			{
				target->updateStatus("Completed Task: " + taskName);
				target->showSplash();
			}
		}
		else
		{
			foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
			{
				target->updateStatus("Unrecognized directive: " + statusDirective);;
				target->showSplash();
			}
			Q_ASSERT(false);
		}

		//Pause for 1 second
		QEventLoop pauseLoop;
		QTimer pauseTimer;
		pauseTimer.setSingleShot(true);
		pauseTimer.setInterval(1000);
		QObject::connect(&pauseTimer, SIGNAL(timeout()), &pauseLoop, SLOT(quit()));
		//! \todo figure out how to handle quitting...
		pauseTimer.start();
		pauseLoop.exec();

		//if all of the visual targets are closed, it's time for us to exit
		bool exit = true;
		foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
		{
			if(/*!target->getVisualTarget().isNull() &&*/
				target->getVisualTarget()->isVisible())
				exit = false;
		}
		if(exit)
			break;
	}

	///////////////////////////////////////
	// Clean up
	///////////////////////////////////////
	//Close all of our visual targets (which are actually QWidgets)
	foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
	{
		if(target->getVisualTarget()->isVisible())
			target->getVisualTarget()->close();
	}

	configDb.close();

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
