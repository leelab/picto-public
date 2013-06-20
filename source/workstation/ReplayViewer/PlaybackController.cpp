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
#include "../../common/statemachine/Scene.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

PlaybackController::PlaybackController()
{
	pixmapVisualTarget_ = QSharedPointer<Picto::PixmapVisualTarget>(new Picto::PixmapVisualTarget(true,800,600));
	outSigControllers_.push_back(QSharedPointer<Picto::VirtualOutputSignalController>(new VirtualOutputSignalController("BNC0")));
	outSigControllers_.push_back(QSharedPointer<Picto::VirtualOutputSignalController>(new VirtualOutputSignalController("PAR0")));
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	renderingTarget_ = QSharedPointer<Picto::RenderingTarget>(new Picto::RenderingTarget(pixmapVisualTarget_, pcmAuralTarget));

	playbackThread_ = QSharedPointer<PlaybackThread>(new PlaybackThread());
	connect(playbackThread_.data(),SIGNAL(init()),this,SLOT(setup()));
	connect(playbackThread_.data(),SIGNAL(update()),this,SLOT(update()));
	playbackThread_->start();
	moveToThread(playbackThread_.data());
}

PlaybackController::~PlaybackController()
{
}

QString PlaybackController::loadSession(QString filename)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::Load,filename));
	return "";
}

void PlaybackController::setEnabledAnalyses(QList<QUuid> analysisList)
{
	data_.setEnabledAnalyses(analysisList);
}

void PlaybackController::setRunSpeed(double value)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeSpeed,value));
}

void PlaybackController::setUserToOperator()
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeUserType,PlaybackCommand::Operator));
}

void PlaybackController::setUserToSubject()
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeUserType,PlaybackCommand::TestSubject));
}

QSharedPointer<Picto::VisualTarget> PlaybackController::getVisualTarget()
{
	return pixmapVisualTarget_;
}

QVector<QSharedPointer<Picto::VirtualOutputSignalController>> PlaybackController::getOutputSignalControllers()
{
	return outSigControllers_;
}

QSharedPointer<Picto::RenderingTarget> PlaybackController::getRenderingTarget()
{
	return renderingTarget_;
}

QSharedPointer<Picto::DesignRoot> PlaybackController::getDesignRoot()
{
	return designRoot_;
}

double PlaybackController::getRunLength()
{
	return data_.getRunLength();
}

void PlaybackController::aboutToQuit()
{
	stop();
	playbackThread_->quit();
	//If the Scene render function is waiting in the playback thread for the GUI thread to render a scene, but the GUI
	//thread is here trying to quit, the playback thread will never end if we call wait on this thread.  To get
	//around this, we tell all scenes to closeRenderLoops(), ie.  Stop waiting.  Then they will end and we will
	//be able to finish this function.
	Picto::Scene::closeRenderLoops();
	playbackThread_->wait();
}

void PlaybackController::play()
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::Play));
}

void PlaybackController::pause()
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::Pause));
}

void PlaybackController::stop()
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::Stop));
}

void PlaybackController::jumpToTime(double time)
{
	if(time > getRunLength() || time < 0)
		return;
	data_.pushCommand(PlaybackCommand(PlaybackCommand::Jump,time));
}

void PlaybackController::selectRun(int index)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeRun,index));
	//playbackUpdater_->loadRun(index);
}

void PlaybackController::newRunLength(double length)
{
	data_.setRunLength(length);
}

void PlaybackController::setCurrTime(double time)
{
	data_.setCurrTime(time);
	emit timeChanged(time);
}

void PlaybackController::setup()
{
	//set up the engine
	bool operatorWasUser = true;
	if(engine_)
		operatorWasUser = engine_->operatorIsUser();
	engine_ = QSharedPointer<Picto::Engine::PictoEngine>(new Picto::Engine::PictoEngine);
	engine_->setExclusiveMode(false);
	engine_->setOperatorAsUser(operatorWasUser);
	engine_->syncInitPropertiesForSlave(false);

	//Setup playback update system
	double playbackSpeed = 1.0;
	if(playbackUpdater_)
		playbackSpeed = playbackUpdater_->getPlaybackSpeed();
	playbackUpdater_ = QSharedPointer<PlaybackStateUpdater>(new PlaybackStateUpdater());
	playbackUpdater_->setPlaybackSpeed(playbackSpeed);
	connect(playbackUpdater_.data(),SIGNAL(framePresented(double)),this,SLOT(setCurrTime(double)));
	connect(playbackUpdater_.data(),SIGNAL(loadedTo(double,double)),this,SIGNAL(loadedTo(double,double)));
	connect(playbackUpdater_.data(),SIGNAL(loading(bool)),this,SIGNAL(loading(bool)));
	connect(playbackUpdater_.data(),SIGNAL(newRun(double)),this,SLOT(newRunLength(double)));
	connect(playbackUpdater_.data(),SIGNAL(finishedPlayback()),this,SLOT(stop()));
	connect(playbackUpdater_.data(),SIGNAL(finishedPlayback()),this,SIGNAL(finishedPlayback()));
	connect(playbackUpdater_.data(),SIGNAL(percentLoaded(double)),this,SIGNAL(percentLoaded(double)));


	//Set up the rendering target
	//QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	//renderingTarget_ = QSharedPointer<Picto::RenderingTarget>(new Picto::RenderingTarget(pixmapVisualTarget_, pcmAuralTarget));
	engine_->addRenderingTarget(renderingTarget_);

	//set up position signal channel
	QSharedPointer<Picto::XYSignalChannel> mouseChannel(new Picto::XYSignalChannel("Position"));
	engine_->addSignalChannel(mouseChannel);

	//Set up output signal generator
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
	rewardController->discardOverlapingRewards(true);
	engine_->setRewardController(rewardController);

	data_.setAsSetup();
}

void PlaybackController::update()
{
	PlaybackControllerData::Status status = data_.getStatus();
	PlaybackCommand cmd = data_.getNextCommand();
	switch(status)
	{
	case PlaybackControllerData::None:
		data_.setNextStatus(PlaybackControllerData::Idle);
		break;
	case PlaybackControllerData::Idle:
		switch(cmd.commandType)
		{
		case PlaybackCommand::Load:
			{
				setup();
				if(!playbackUpdater_->setFile(cmd.commandData.toString()))
				{
					emit loadError("Failed to load session design.  This often means that the session contains no experimental data.");
					break;
				}
				QSharedPointer<DesignRoot> newDesignRoot = QSharedPointer<DesignRoot>(playbackUpdater_->getDesignRoot());
				if(!newDesignRoot)
				{
					emit loadError("Failed to load session design.");
					break;
				}
				if(!newDesignRoot->compiles())
				{
					emit loadError("Session's experiment does not compile.");
					break;
				}
				designRoot_ = newDesignRoot;
				designRoot_->enableRunMode(true);
				data_.setRunLength(playbackUpdater_->getRunLength());
				emit runsUpdated(playbackUpdater_->getRuns(),playbackUpdater_->getSavedRuns());

				//Set up SlaveExperimentDriver to connect StateUpdater and Experiment
				QSharedPointer<Picto::Experiment> currExp = designRoot_->getExperiment().staticCast<Experiment>();
				if(!currExp || !currExp->getTaskNames().size())
				{
					emit loadError("Failed to load experiment from session.");
					break;
				}
				experiment_ = currExp;
				experiment_->setEngine(engine_);
				slaveExpDriver_ = QSharedPointer<SlaveExperimentDriver>(new SlaveExperimentDriver(experiment_,playbackUpdater_));
				emit designRootChanged();
				data_.setNextStatus(PlaybackControllerData::Stopped);
			}
			break;
		case PlaybackCommand::ChangeSpeed:
			playbackUpdater_->setPlaybackSpeed(cmd.commandData.toDouble());
			break;
		case PlaybackCommand::ChangeUserType:
			engine_->setOperatorAsUser(cmd.commandData.toInt() == PlaybackCommand::Operator);
			break;
		}
		break;
	case PlaybackControllerData::Stopped:
		switch(cmd.commandType)
		{
		case PlaybackCommand::Play:
			designRoot_->getExperiment()->getDesignConfig()->setActiveAnalysisIds(data_.getEnabledAnalyses());
			playbackUpdater_->play();
			data_.setNextStatus(PlaybackControllerData::Running);
			break;
		case PlaybackCommand::Pause:
			designRoot_->getExperiment()->getDesignConfig()->setActiveAnalysisIds(data_.getEnabledAnalyses());
			playbackUpdater_->play();
			data_.setNextStatus(PlaybackControllerData::Running);
			data_.pushCommand(cmd,false);
			break;
		case PlaybackCommand::Load:
			data_.setNextStatus(PlaybackControllerData::Idle);
			data_.pushCommand(cmd,false);
			break;
		case PlaybackCommand::ChangeRun:
			playbackUpdater_->loadRun(cmd.commandData.toInt());
			data_.setRunLength(playbackUpdater_->getRunLength());
			setCurrTime(0.0);
			break;
		case PlaybackCommand::ChangeSpeed:
			playbackUpdater_->setPlaybackSpeed(cmd.commandData.toDouble());
			break;
		case PlaybackCommand::ChangeUserType:
			engine_->setOperatorAsUser(cmd.commandData.toInt() == PlaybackCommand::Operator);
			break;
		}
		break;
	case PlaybackControllerData::Running:
	case PlaybackControllerData::Paused:
		switch(cmd.commandType)
		{
		case PlaybackCommand::Play:
			if(status == PlaybackControllerData::Paused)
			{
				playbackUpdater_->play();
				data_.setNextStatus(PlaybackControllerData::Running);
			}
			break;
		case PlaybackCommand::Pause:
			if(status == PlaybackControllerData::Running)
			{
				playbackUpdater_->pause();
				data_.setNextStatus(PlaybackControllerData::Paused);
			}
			break;
		case PlaybackCommand::Stop:
			playbackUpdater_->stop();
			data_.setCurrTime(0.0);
			data_.setNextStatus(PlaybackControllerData::Stopped);
			break;
		case PlaybackCommand::Jump:
			playbackUpdater_->jumpToTime(cmd.commandData.toDouble());
			data_.setNextStatus(status);
			break;
		case PlaybackCommand::Load:
			playbackUpdater_->stop();
			data_.setCurrTime(0.0);
			data_.setNextStatus(PlaybackControllerData::Idle);
			data_.pushCommand(cmd,false);
			break;
		case PlaybackCommand::ChangeRun:
			playbackUpdater_->stop();
			data_.setCurrTime(0.0);
			data_.setNextStatus(PlaybackControllerData::Stopped);
			data_.pushCommand(cmd,false);
			break;
		case PlaybackCommand::ChangeSpeed:
			playbackUpdater_->setPlaybackSpeed(cmd.commandData.toDouble());
			break;
		case PlaybackCommand::ChangeUserType:
			engine_->setOperatorAsUser(cmd.commandData.toInt() == PlaybackCommand::Operator);
			break;
		}
		playbackUpdater_->updateState();
		break;
	}
	PlaybackControllerData::Status nextStatus = data_.getNextStatus();
	if(status != nextStatus)
	{
		data_.setStatus(data_.getNextStatus());
		emit statusChanged(nextStatus);
	}
}







PlaybackControllerData::PlaybackControllerData(){isSetup_ = false;currTime_ = 0;runSpeed_ = 0;runLength_ = 0;status_ = None;nextStatus_ = None;}
void PlaybackControllerData::setAsSetup(){QMutexLocker locker(&mutex_);isSetup_=true;}
bool PlaybackControllerData::isSetup(){QMutexLocker locker(&mutex_);return isSetup_;}
void PlaybackControllerData::setCurrTime(double val){QMutexLocker locker(&mutex_);currTime_=val;}
double PlaybackControllerData::getCurrTime(){QMutexLocker locker(&mutex_);return currTime_;}
void PlaybackControllerData::setRunSpeed(double val){QMutexLocker locker(&mutex_);runSpeed_ = val;}
double PlaybackControllerData::getRunSpeed(){QMutexLocker locker(&mutex_);return runSpeed_;}
void PlaybackControllerData::setRunLength(double val){QMutexLocker locker(&mutex_);runLength_ = val;}
double PlaybackControllerData::getRunLength(){QMutexLocker locker(&mutex_);return runLength_;}
void PlaybackControllerData::setEnabledAnalyses(QList<QUuid> analysisList){QMutexLocker locker(&mutex_);enabledAnalyses_=analysisList;}
QList<QUuid> PlaybackControllerData::getEnabledAnalyses(){QMutexLocker locker(&mutex_);return enabledAnalyses_;}
void PlaybackControllerData::setStatus(Status val){QMutexLocker locker(&mutex_);status_ = val;}
PlaybackControllerData::Status PlaybackControllerData::getStatus(){QMutexLocker locker(&mutex_);return status_;}
void PlaybackControllerData::setNextStatus(Status val){QMutexLocker locker(&mutex_);nextStatus_ = val;}
PlaybackControllerData::Status PlaybackControllerData::getNextStatus(){QMutexLocker locker(&mutex_);return nextStatus_;}
void PlaybackControllerData::pushCommand(PlaybackCommand cmd, bool toBack){QMutexLocker locker(&mutex_);if(toBack)cmds_.push_back(cmd);else cmds_.push_front(cmd);}
PlaybackCommand PlaybackControllerData::getNextCommand(){QMutexLocker locker(&mutex_);PlaybackCommand returnVal; if(cmds_.size()){returnVal = cmds_.front();cmds_.pop_front();}return returnVal;}
