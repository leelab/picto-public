#include <QMutexLocker>
#include <QSharedPointer>

#include "PlaybackController.h"

#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/engine/XYSignalChannel.h"
#include "../../common/engine/MouseInputPort.h"
#include "../../common/parameter/OperatorClickParameter.h"
#include "../../common/compositor/OutputSignalWidget.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

PlaybackController::PlaybackController(QWidget *parent) :
status_(Stopped),
newStatus_(Stopped),
expRunning_(false),
currTime_(0.0)
{
	//set up the engine
	engine_ = QSharedPointer<Picto::Engine::PictoEngine>(new Picto::Engine::PictoEngine);
	engine_->setExclusiveMode(false);
	engine_->setOperatorAsUser(true);
	engine_->syncInitPropertiesForSlave(false);
	connect(engine_.data(),SIGNAL(slaveTimeChanged(double)),this,SLOT(setCurrTime(double)));

	//Setup playback update system
	playbackUpdater_ = QSharedPointer<PlaybackStateUpdater>(new PlaybackStateUpdater());
	engine_->setStateUpdater(playbackUpdater_);
	connect(playbackUpdater_.data(),SIGNAL(loading(bool)),this,SIGNAL(loading(bool)));
	connect(playbackUpdater_.data(),SIGNAL(finishedPlayback()),this,SLOT(stop()));
	connect(playbackUpdater_.data(),SIGNAL(finishedPlayback()),this,SIGNAL(finishedPlayback()));


	//Set up the rendering target
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	pixmapVisualTarget_ = QSharedPointer<Picto::PixmapVisualTarget>(new Picto::PixmapVisualTarget(true,800,600));
	renderingTarget_ = QSharedPointer<Picto::RenderingTarget>(new Picto::RenderingTarget(pixmapVisualTarget_, pcmAuralTarget));
	engine_->addRenderingTarget(renderingTarget_);

	//set up position signal channel
	QSharedPointer<Picto::XYSignalChannel> mouseChannel(new Picto::XYSignalChannel("Position"));
	engine_->addSignalChannel(mouseChannel);

	//Set up output signal generator
	outSigControllers_.push_back(QSharedPointer<Picto::VirtualOutputSignalController>(new VirtualOutputSignalController("BNC0")));
	outSigControllers_.push_back(QSharedPointer<Picto::VirtualOutputSignalController>(new VirtualOutputSignalController("PAR0")));
	foreach(QSharedPointer<Picto::VirtualOutputSignalController> cont,outSigControllers_)
	{
		engine_->setOutputSignalController(cont->getPort(),cont);
	}

	//Set up event code generator
	QSharedPointer<Picto::EventCodeGenerator> nullGenerator;
	nullGenerator = QSharedPointer<Picto::EventCodeGenerator>(new Picto::NullEventCodeGenerator());
	engine_->setEventCodeGenerator(nullGenerator);

	//set up reward controller
	QSharedPointer<Picto::RewardController> rewardController;
	rewardController = QSharedPointer<Picto::RewardController>(new Picto::AudioRewardController());
	engine_->setRewardController(rewardController);

	renderingTarget_->showSplash();
}

PlaybackController::~PlaybackController()
{
}

QString PlaybackController::loadSession(QString filename)
{
	stop();
	playbackUpdater_->setFile(filename);
	QSharedPointer<DesignRoot> newDesignRoot = QSharedPointer<DesignRoot>(playbackUpdater_->getDesignRoot());
	if(!newDesignRoot)
		return "Failed to load session design.  This could happen if the design did not include any runs.";
	QSharedPointer<Design> design = newDesignRoot->getDesign("Experiment",0);
	if(!design)
	{
		return "Failed to load experiment from session.";
	}
	if(!design->compiles())
	{
		return "Session's experiment does not compile.";
	}
	designRoot_ = newDesignRoot;
	emit runsUpdated(playbackUpdater_->getRuns());
	return "";
}

void PlaybackController::setRunSpeed(double value)
{
	playbackUpdater_->setPlaybackSpeed(value);
}

QSharedPointer<Picto::VisualTarget> PlaybackController::getVisualTarget()
{
	return pixmapVisualTarget_;
}

QVector<QSharedPointer<Picto::VirtualOutputSignalController>> PlaybackController::getOutputSignalControllers()
{
	return outSigControllers_;
}

double PlaybackController::getRunLength()
{
	return playbackUpdater_->getRunLength();
}

void PlaybackController::aboutToQuit()
{
	if(expRunning_)
	{
		engine_->stop();
	}
}

void PlaybackController::play()
{
	playbackUpdater_->play();
	if(status_ == Stopped)
	{
		QTimer::singleShot(0,this,SLOT(runExperiment()));
	}
	else if(status_ == Paused)
	{
		engine_->play();
	}
}

void PlaybackController::pause()
{
	playbackUpdater_->pause();
}

void PlaybackController::stop()
{
	engine_->stop();
	playbackUpdater_->stop();
	pixmapVisualTarget_->clear();
	renderingTarget_->showSplash();
	setCurrTime(0.0);
}

void PlaybackController::jumpToTime(double time)
{
	if(time > getRunLength() || time < 0)
		return;
	if(time < currTime_)
	{
		engine_->stop();
		QTimer::singleShot(0,this,SLOT(runExperiment()));
	}
	playbackUpdater_->jumpToTime(time);
}

void PlaybackController::selectRun(int index)
{
	playbackUpdater_->loadRun(index);
}

void PlaybackController::setCurrTime(double time)
{
	currTime_ = time;
	emit timeChanged(currTime_);
}

void PlaybackController::runExperiment()
{
	if(expRunning_)
	{
		QTimer::singleShot(0,this,SLOT(runExperiment()));
		return;
	}
	QSharedPointer<Design> design = designRoot_->getDesign("Experiment",0);
	QSharedPointer<Picto::Experiment> currExp = design->getRootAsset().staticCast<Experiment>();
	if(!currExp || !currExp->getTaskNames().size())
	{
		return;
	}
	experiment_ = currExp;
	experiment_->setEngine(engine_);
	expRunning_ = true;
	experiment_->runTask(experiment_->getTaskNames()[0]);
	expRunning_ = false;
}