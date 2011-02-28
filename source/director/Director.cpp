#include <QApplication>
#include "Director.h"
Director::Director(QString name,
		HardwareSetup::SignalChannelType sigChannel,
		HardwareSetup::VisualTargetType visualTarget,
		HardwareSetup::RewardControllerType rewardController,
		HardwareSetup::EventCodeGeneratorType eventCodeGenerator):
	sigChannel_(sigChannel),
	visualTarget_(visualTarget),
	rewardController_(rewardController),
	eventCodeGenerator_(eventCodeGenerator)
{
	//! \TODO Set up random number generator?

	///////////////////////////////////////
	// Open Configuration database, and configure as needed
	///////////////////////////////////////
    configDb_ = QSqlDatabase::addDatabase("QSQLITE","PictoDirectorConfigDatabase");
	configDb_.setDatabaseName(QCoreApplication::applicationDirPath() + "/PictoDirector.config");
    configDb_.open();

	QSqlQuery query(configDb_);

	if(!configDb_.tables().contains("directorinfo"))
	{
		query.exec("CREATE TABLE directorinfo "
						"(id int, "
						"key TEXT, "
						"value TEXT)");
		query.exec("INSERT INTO directorinfo (key,value) VALUES ('Name','unnamed')");
		QString test = query.lastError().text();
	}

	//possibly reset our name...
	if(name != "")
	{
		query.prepare("UPDATE directorinfo SET value=:value WHERE key='Name'");
		query.bindValue(":value",name);
		query.exec();
	}
	//figure out our name
	query.exec("SELECT value FROM directorinfo WHERE key='Name'");
	Q_ASSERT(query.next());
	name_ = query.value(0).toString();

}
Director::~Director()
{
	configDb_.close();
}

QString Director::componentType()
{
	return "DIRECTOR";
}

QString Director::name()
{
	return name_;
}
int Director::openDevice()
{
	///////////////////////////////////////
	// Setup hardware
	///////////////////////////////////////
	engine_ = QSharedPointer<Picto::Engine::PictoEngine>(new Picto::Engine::PictoEngine());
	engine_->setName(name_);
	engine_->setExclusiveMode(true);

	HardwareSetup hwSetup(engine_);
	
	//If there is a command argument of "-pixmap", we should use a pixmpa rendering
	//otherwise use d3d.
	if(!hwSetup.setupRenderingTargets(visualTarget_)) 
		return 1;
	if(!hwSetup.setupSignalChannel(sigChannel_)) 
		return 1;
	if(!hwSetup.setupRewardController(rewardController_)) 
		return 1;
	if(!hwSetup.setupEventCodeGenerator(eventCodeGenerator_))
		return 1;
	if(!hwSetup.isSetup())
		return 1;

	//Put up our splash screen
	updateSplashStatus("PictoDirector started");
	return 0;
}

int Director::closeDevice()
{
	QList<QSharedPointer<Picto::RenderingTarget> > renderingTargets = engine_->getRenderingTargets();
	foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
	{
		if(target->getVisualTarget()->isVisible())
			target->getVisualTarget()->close();
	}
	return 0;
}

bool Director::continueRunning()
{
	return engine_->hasVisibleRenderingTargets();
}

int Director::reportServerConnectivity(bool connected)
{
	updateSplashStatus((connected)?"Connected to server":"Not connected to server");
	if(connected)
	{
		if(engine_->getDataCommandChannel().isNull())
			engine_->setDataCommandChannel(dataCommandChannel_);
		if(engine_->getUpdateCommandChannel().isNull())
			engine_->setUpdateCommandChannel(serverUpdateChannel_);
	}
	return 0;
}
int Director::startSession(QUuid sessionID)
{
	engine_->setSessionId(sessionID);
	return 0;
}
int Director::loadExp(QString expXML)
{
	//Load the experiment
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(expXML.toUtf8()));
	experiment_ = QSharedPointer<Picto::Experiment>(new Picto::Experiment);

	xmlReader->readNext();
	while(!xmlReader->atEnd() && xmlReader->name().toString() != "Experiment")
	{
		xmlReader->readNext();
	}

	if(!experiment_->deserializeFromXml(xmlReader))
	{
		updateSplashStatus(QString("Error loading experiment: %1").arg(experiment_->getErrors()));
	}
	else
	{
		updateSplashStatus("Loaded experiment, Session ID: " + engine_->getSessionId().toString());
	}
	return 0;
}
int Director::startExp(QString taskName)
{
	updateSplashStatus("Starting task: " + taskName);
	//start running our task
	if(experiment_)
		experiment_->runTask(taskName, engine_);
	//upon completion return to splash screen
	updateSplashStatus("Completed Task: " + taskName);
	setStatus(stopped);	// If we're here then our status is stopped
	return 0;
}
int Director::endSession()
{
	engine_->setSessionId(QUuid());
	experiment_ = QSharedPointer<Picto::Experiment>();
	updateSplashStatus("Session ended");
	return 0;
}

int Director::reward(int channel)
{
	engine_->giveReward(channel);
	return 0;
}

int Director::reportErrorDirective()
{
	updateSplashStatus("ERROR");
	return 0;
}

int Director::reportUnsupportedDirective(QString directive)
{
	updateSplashStatus("Unrecognized directive: " + directive);
	return 0;
}

void Director::updateSplashStatus(QString status)
{
	QList<QSharedPointer<Picto::RenderingTarget> > renderingTargets = engine_->getRenderingTargets();
	foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
	{
		target->updateStatus(status);
		target->showSplash();
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
