#include <QApplication>
#include "Director.h"
#include "network/DirectorStatusManager.h"
#include "protocol/DirectorNewSessionResponseHandler.h"
#include "protocol/DirectorEndSessionResponseHandler.h"
#include "protocol/DirectorErrorResponseHandler.h"
#include "protocol/DirectorLoadExpResponseHandler.h"
#include "protocol/DirectorPauseResponseHandler.h"
#include "protocol/DirectorResumeResponseHandler.h"
#include "protocol/DirectorRewardResponseHandler.h"
#include "protocol/DirectorStartResponseHandler.h"
#include "protocol/DirectorStopResponseHandler.h"
#include "protocol/DirectorParameterResponseHandler.h"
#include "protocol/DirectorClickResponseHandler.h"
#include "../common/memleakdetect.h"
using namespace Picto;

Director::Director(QString name,
		HardwareSetup::SignalChannelType sigChannel,
		HardwareSetup::VisualTargetType visualTarget,
		HardwareSetup::RewardControllerType rewardController,
		HardwareSetup::EventCodeGeneratorType eventCodeGenerator,
		int xEyeChannel,
		int yEyeChannel,
		int xDiamChannel,
		int yDiamChannel,
		int posSampPer,
		int diamSampPer):
	ComponentInterface("DIRECTOR"),
	sigChannel_(sigChannel),
	visualTarget_(visualTarget),
	rewardController_(rewardController),
	eventCodeGenerator_(eventCodeGenerator),
	xChannel_(xEyeChannel),
	yChannel_(yEyeChannel),
	xDiamChannel_(xDiamChannel),
	yDiamChannel_(yDiamChannel),
	posSampPer_(posSampPer),
	diamSampPer_(diamSampPer)
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
		query.exec(QString("INSERT INTO directorinfo (key,value) VALUES ('Name','Director%1')").arg(componentId_.toString()).toAscii());
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
	bool rc = query.next();
	Q_ASSERT(rc);
	name_ = query.value(0).toString();

}
Director::~Director()
{
	configDb_.close();
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
	if(!hwSetup.setupRenderingTargets(visualTarget_,sigChannel_ != HardwareSetup::Mouse)) 
		return 1;
	if(!hwSetup.setXYChannelNums(xChannel_,yChannel_))
		return 1;
	if(!hwSetup.setXYDiamChannelNums(xDiamChannel_,yDiamChannel_))
		return 1;
	if(!hwSetup.setSamplePeriods(posSampPer_,diamSampPer_))
		return 1;
	if(!hwSetup.setupSignalChannel(sigChannel_)) 
		return 1;
	if(!hwSetup.setupRewardController(rewardController_)) 
		return 1;
	if(!hwSetup.setupEventCodeGenerator(eventCodeGenerator_))
		return 1;
	if(!hwSetup.isSetup())
		return 1;

	statusManager_ = QSharedPointer<ComponentStatusManager>(new DirectorStatusManager());
	statusManager_.staticCast<DirectorStatusManager>()->setEngine(engine_);
	statusManager_->setName(name());

	dataCommandChannel_->setStatusManager(statusManager_);
	engine_->setDataCommandChannel(dataCommandChannel_);

	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new DirectorNewSessionResponseHandler(statusManager_.staticCast<DirectorStatusManager>(),dataCommandChannel_)));
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new DirectorEndSessionResponseHandler(statusManager_.staticCast<DirectorStatusManager>(),dataCommandChannel_)));
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new DirectorErrorResponseHandler(statusManager_.staticCast<DirectorStatusManager>())));
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new DirectorLoadExpResponseHandler(statusManager_.staticCast<DirectorStatusManager>())));
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new DirectorPauseResponseHandler(statusManager_.staticCast<DirectorStatusManager>())));
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new DirectorResumeResponseHandler(statusManager_.staticCast<DirectorStatusManager>())));
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new DirectorRewardResponseHandler(statusManager_.staticCast<DirectorStatusManager>())));
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new DirectorStartResponseHandler(statusManager_.staticCast<DirectorStatusManager>())));
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new DirectorStopResponseHandler(statusManager_.staticCast<DirectorStatusManager>())));
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new DirectorParameterResponseHandler(statusManager_.staticCast<DirectorStatusManager>())));
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new DirectorClickResponseHandler(statusManager_.staticCast<DirectorStatusManager>())));


	//Put up our splash screen
	statusManager_.staticCast<DirectorStatusManager>()->setUserInfo("PictoDirector started");
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
