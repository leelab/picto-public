#include <QApplication>
#include <QDir>
#include <QSettings>

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
#include "../common/update/UpdateDownloader.h"
#include "../common/memleakdetect.h"
using namespace Picto;

/*! \brief When using a front panel, this is the name of the executable that should be started to handle the 
 *	front panel hardware.
 */
#if defined(QT_DEBUG)
#define FRONTPANELEXECUTABLE "EmbeddedFrontPanel_debug"
#else
#define FRONTPANELEXECUTABLE "EmbeddedFrontPanel"
#endif

/*! \brief Constructs a Director object.
 *	@param name The name of this Director
 *	@param sigChannel The HardwareSetup::SignalChannelType of signal channels that will be used by this Director (ie. Mouse, legacy eye, Pictobox eye)
 *	@param visualTarget	The HardwareSetup::VisualTargetType that will be used by this Director (ie. Pixmap or Direct3d)
 *	@param rewardController The HardwareSetup::RewardControllerType that will be used by this Director (ie. Pictobox Solenoid, audio, etc).
 *	@param outSigController The HardwareSetup::OutputSignalControllerType that will be used by this Director (ie. Pictobox, none)
 *	@param eventCodeGenerator The HardwareSetup::EventCodeGeneratorType that will be used to deliver event codes (ie. Pictobox, legacy, none)
 *	@param xEyeChannel The input channel on which x eye position data will be delivered
 *	@param yEyeChannel The input channel on which y eye position data will be delivered
 *	@param xDiamChannel The input channel on which x eye diameter data will be delivered
 *	@param yDiamChannel The input channel on which y eye diameter data will be delivered
 *	@param posSampPer The number of milliseconds per reading of position data (position sample period)
 *	@param diamSampPer The number of milliseconds per reading of diameter data (diameter sample period)
 *	\details Stores the input data and writes it to a configuration file.
 */
Director::Director(QString name,
		HardwareSetup::SignalChannelType sigChannel,
		HardwareSetup::VisualTargetType visualTarget,
		HardwareSetup::RewardControllerType rewardController,
		HardwareSetup::OutputSignalControllerType outSigController,
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
	outSigController_(outSigController),
	eventCodeGenerator_(eventCodeGenerator),
	xChannel_(xEyeChannel),
	yChannel_(yEyeChannel),
	xDiamChannel_(xDiamChannel),
	yDiamChannel_(yDiamChannel),
	posSampPer_(posSampPer),
	diamSampPer_(diamSampPer),
	useFrontPanel_(rewardController == HardwareSetup::PictoBoxXpReward),
	directorData_(new DirectorData())
{

	///////////////////////////////////////
	// Open Configuration database, and configure as needed
	///////////////////////////////////////
    configDb_ = QSqlDatabase::addDatabase("QSQLITE","PictoDirectorConfigDatabase");
	QString configPath = QCoreApplication::applicationDirPath()+"/../config";
	QDir configDir(configPath);
	if(!configDir.exists())
	{
		configDir.mkpath(configPath);
		configDir = QDir(configPath);
	}
	configDb_.setDatabaseName(configDir.canonicalPath() + "/" + "PictoDirector.config");
    configDb_.open();

	QSqlQuery query(configDb_);

	if(!configDb_.tables().contains("directorinfo"))
	{
		query.exec("CREATE TABLE directorinfo "
						"(id int, "
						"key TEXT, "
						"value TEXT)");
		query.exec(QString("INSERT INTO directorinfo (key,value) VALUES ('Name','Director%1')").arg(componentId_.toString()).toLatin1());
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

	//Figure out reward durations
	QSettings settings("Block Designs", Picto::Names->frontPanelAppName);
	if(settings.contains("RewardDurations"))
	{
		rewardDurs_ = settings.value("RewardDurations").toString().split(":",QString::SkipEmptyParts);
	}
	else
	{
		rewardDurs_ << "100" << "100" << "100" << "100" << "100";
		settings.setValue("RewardDurations",rewardDurs_.join(":"));
	}
	//rewardDurs_ = query.exec("SELECT value FROM directorinfo WHERE key='RewardDurations'");
	//if(!query.next())
	//{
	//	query.exec("INSERT INTO directorinfo (key,value) VALUES ('RewardDurations','')");
	//	for(int i=0;i<=4;i++)
	//		changeRewardDuration(i,100);
	//	query.exec("SELECT value FROM directorinfo WHERE key='RewardDurations'");
	//	Q_ASSERT(query.next());
	//}
	//rewardDurs_ = query.value(0).toString().split(":",QString::SkipEmptyParts);

	//Figure out flush durations
	if(settings.contains("FlushDurations"))
	{
		flushDurs_ = settings.value("FlushDurations").toString().split(":",QString::SkipEmptyParts);
	}
	else
	{
		flushDurs_ << "300" << "300" << "300" << "300" << "300";
		settings.setValue("FlushDurations",flushDurs_.join(":"));
	}
	//query.exec("SELECT value FROM directorinfo WHERE key='FlushDurations'");
	//if(!query.next())
	//{
	//	query.exec("INSERT INTO directorinfo (key,value) VALUES ('FlushDurations','')");
	//	for(int i=0;i<=4;i++)
	//		changeFlushDuration(i,300);
	//	query.exec("SELECT value FROM directorinfo WHERE key='FlushDurations'");
	//	Q_ASSERT(query.next());
	//}
	//flushDurs_ = query.value(0).toString().split(":",QString::SkipEmptyParts);

}
Director::~Director()
{
	configDb_.close();
}

QString Director::name()
{
	return name_;
}

/*! \brief Implements ComponentInterface::openDevice() to setup the Picto::Engine::PictoEngine,
 *	start a FrontPanel process (if requested in constructor), setup the DirectorStatusManager,
 *	and add ProtocolResponseHandler objects to the Picto::CommandChannel.
 */
int Director::openDevice()
{
	///////////////////////////////////////
	// Setup hardware
	///////////////////////////////////////
	engine_ = QSharedPointer<Picto::Engine::PictoEngine>(new Picto::Engine::PictoEngine());
	engine_->setName(name_);
	engine_->setExclusiveMode(true);
	QObject::connect(engine_.data(),SIGNAL(nameChangeRequest(QString)),this,SLOT(changeName(QString)));
	QObject::connect(engine_.data(),SIGNAL(rewardDurationChanged(int,int)),this,SLOT(changeRewardDuration(int,int)));
	QObject::connect(engine_.data(),SIGNAL(flushDurationChangeRequest(int,int)),this,SLOT(changeFlushDuration(int,int)));

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
	if(!hwSetup.setupOutputSignalController(outSigController_,eventCodeGenerator_)) 
		return 1;
	if(!hwSetup.setupEventCodeGenerator(eventCodeGenerator_))
		return 1;
	if(!hwSetup.isSetup())
		return 1;

	//If running on PictoBox create front panel
	if(useFrontPanel_)
	{
		frontPanelProcess_ = QSharedPointer<QProcess>(new QProcess());
		frontPanelProcess_->start(QCoreApplication::applicationDirPath() + "/" + FRONTPANELEXECUTABLE,QStringList() << "-systemNumber" << QString::number(Picto::portNums->getSystemNumber()));
		if(!frontPanelProcess_->waitForStarted())
			return 1;
		//Assure that the update downloader will kill the front panel process if it needs to restart the application.
		connect(UpdateDownloader::getInstance().data(),SIGNAL(closingProcess()),frontPanelProcess_.data(),SLOT(kill()));
		connect(engine_.data(),SIGNAL(escapePressed()),frontPanelProcess_.data(),SLOT(kill()));
		fpInterface_ = QSharedPointer<FPInterface>(new FPInterface());
		engine_->addControlPanel(fpInterface_);
	}

	for(int i=0;i<rewardDurs_.size();i++)
		engine_->setRewardDuration(i,rewardDurs_[i].toInt());
	for(int i=0;i<flushDurs_.size();i++)
		engine_->setFlushDuration(i,flushDurs_[i].toInt());


	statusManager_ = QSharedPointer<ComponentStatusManager>(new DirectorStatusManager());
	statusManager_.staticCast<DirectorStatusManager>()->setEngine(engine_);
	statusManager_.staticCast<DirectorStatusManager>()->setDirectorData(directorData_);
	//We connect the escape pressed signal to ComponentStatusManager::forceExit() after connecting the front panel kill() signal so that
	//the front panel application has time to close before the director closes.
	connect(engine_.data(),SIGNAL(escapePressed()),statusManager_.data(),SLOT(forceExit()));

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

/*! \brief Implements ComponentInterface::closeDevice() to close all visible Picto::VisualTarget objects.
 */
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

/*! \brief Changes the name of this Director to the input value.
*/
void Director::changeName(QString name)
{
	QSqlQuery query(configDb_);
	query.prepare("UPDATE directorinfo SET value=:value WHERE key='Name'");
	query.bindValue(":value",name);
	query.exec();
}

/*! \brief Changes the default reward duration (milliseconds) on the input reward controller.  
*/
void Director::changeRewardDuration(int controller, int duration)
{
	while(rewardDurs_.size() <= controller)
		rewardDurs_.append("100");
	rewardDurs_[controller] = QString::number(duration);
	directorData_->setReward(controller,duration);
	//QSqlQuery query(configDb_);
	//query.prepare("UPDATE directorinfo SET value=:value WHERE key='RewardDurations'");
	//query.bindValue(":value",rewardDurs_.join(":"));
	//query.exec();
}

/*! \brief Changes the default flush duration (milliseconds) on the input reward controller.  
*/
void Director::changeFlushDuration(int controller, int duration)
{
	while(flushDurs_.size() <= controller)
		flushDurs_.append("300");
	flushDurs_[controller] = QString::number(duration);
	//QSqlQuery query(configDb_);
	//query.prepare("UPDATE directorinfo SET value=:value WHERE key='FlushDurations'");
	//query.bindValue(":value",flushDurs_.join(":"));
	//query.exec();
}