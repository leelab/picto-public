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

PlaybackController::PlaybackController()
{
	expRunning_ = false;
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
	//stop();
	//playbackUpdater_->setFile(filename);
	//QSharedPointer<DesignRoot> newDesignRoot = QSharedPointer<DesignRoot>(playbackUpdater_->getDesignRoot());
	//if(!newDesignRoot)
	//	return "Failed to load session design.  This could happen if the design did not include any runs.";
	//QSharedPointer<Design> design = newDesignRoot->getDesign("Experiment",0);
	//if(!design)
	//{
	//	return "Failed to load experiment from session.";
	//}
	//if(!design->compiles())
	//{
	//	return "Session's experiment does not compile.";
	//}
	//designRoot_ = newDesignRoot;
	//emit runsUpdated(playbackUpdater_->getRuns());
	return "";
}

void PlaybackController::setRunSpeed(double value)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeSpeed,value));
	//playbackUpdater_->setPlaybackSpeed(runSpeed_);
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

double PlaybackController::getRunLength()
{
	return data_.getRunLength();
}

void PlaybackController::aboutToQuit()
{
	stop();
	playbackThread_->quit();
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

bool PlaybackController::waitingForTransition()
{
	PlaybackControllerData::Status status = data_.getStatus();
	PlaybackControllerData::Status nextStatus = data_.getNextStatus();
	if(status == nextStatus)
		return false;
	switch(status)
	{
	case PlaybackControllerData::None:
		switch(nextStatus)
		{
		case PlaybackControllerData::Idle:
			break;
		default:
			Q_ASSERT_X(false,"PlaybackController",QString("Transition from: %1 to %2 not defined.").arg(status).arg(nextStatus).toAscii());
		}
		break;
	case PlaybackControllerData::Idle:
		switch(nextStatus)
		{
		case PlaybackControllerData::Stopped:
			if(expRunning_)
				return true;
			break;
		default:
			Q_ASSERT_X(false,"PlaybackController",QString("Transition from: %1 to %2 not defined.").arg(status).arg(nextStatus).toAscii());
		}
		break;
	case PlaybackControllerData::Stopped:
		switch(nextStatus)
		{
		case PlaybackControllerData::Idle:
			break;
		case PlaybackControllerData::Running:
		case PlaybackControllerData::Paused:
			if(!expRunning_)
				return true;
			break;
		default:
			Q_ASSERT_X(false,"PlaybackController",QString("Transition from: %1 to %2 not defined.").arg(status).arg(nextStatus).toAscii());
		}
		break;
	case PlaybackControllerData::Running:
	case PlaybackControllerData::Paused:
		switch(nextStatus)
		{
		case PlaybackControllerData::Idle:
		case PlaybackControllerData::Stopped:
			if(expRunning_)
				return true;
			break;
		case PlaybackControllerData::Running:
		case PlaybackControllerData::Paused:
			if(!expRunning_)
				return true;
			break;
		default:
			Q_ASSERT_X(false,"PlaybackController",QString("Transition from: %1 to %2 not defined.").arg(status).arg(nextStatus).toAscii());
		}
		break;
	}
	data_.setStatus(nextStatus);
	emit statusChanged(nextStatus);
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
	engine_ = QSharedPointer<Picto::Engine::PictoEngine>(new Picto::Engine::PictoEngine);
	engine_->setExclusiveMode(false);
	engine_->setOperatorAsUser(true);
	engine_->syncInitPropertiesForSlave(false);
	connect(engine_.data(),SIGNAL(slaveTimeChanged(double)),this,SLOT(setCurrTime(double)));

	//Setup playback update system
	playbackUpdater_ = QSharedPointer<PlaybackStateUpdater>(new PlaybackStateUpdater());
	engine_->setStateUpdater(playbackUpdater_);
	connect(playbackUpdater_.data(),SIGNAL(loadedTo(double,double)),this,SIGNAL(loadedTo(double,double)));
	connect(playbackUpdater_.data(),SIGNAL(loading(bool)),this,SIGNAL(runLoading(bool)));
	connect(playbackUpdater_.data(),SIGNAL(newRun(double)),this,SIGNAL(newRunLength(double)));
	connect(playbackUpdater_.data(),SIGNAL(finishedPlayback()),this,SLOT(stop()));
	connect(playbackUpdater_.data(),SIGNAL(finishedPlayback()),this,SIGNAL(finishedPlayback()));


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
	engine_->setRewardController(rewardController);

	//renderingTarget_->showSplash();
	data_.setAsSetup();
}

void PlaybackController::update()
{
	if(waitingForTransition())
		return;
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
				playbackUpdater_->setFile(cmd.commandData.toString());
				QSharedPointer<DesignRoot> newDesignRoot = QSharedPointer<DesignRoot>(playbackUpdater_->getDesignRoot());
				if(!newDesignRoot)
				{
					emit error("Failed to load session design.  This could happen if the design did not include any runs.");
					break;
				}
				QSharedPointer<Design> design = newDesignRoot->getDesign("Experiment",0);
				if(!design)
				{
					emit error("Failed to load experiment from session.");
					break;
				}
				if(!design->compiles())
				{
					emit error("Session's experiment does not compile.");
					break;
				}
				designRoot_ = newDesignRoot;
				data_.setRunLength(playbackUpdater_->getRunLength());
				emit runsUpdated(playbackUpdater_->getRuns());
				data_.setNextStatus(PlaybackControllerData::Stopped);
			}
			break;
		case PlaybackCommand::ChangeSpeed:
			playbackUpdater_->setPlaybackSpeed(cmd.commandData.toDouble());
			break;
		}
		break;
	case PlaybackControllerData::Stopped:
		switch(cmd.commandType)
		{
		case PlaybackCommand::Play:
			playbackUpdater_->play();
			QTimer::singleShot(0,this,SLOT(runExperiment()));
			data_.setNextStatus(PlaybackControllerData::Running);
			break;
		case PlaybackCommand::Pause:
			playbackUpdater_->pause();
			QTimer::singleShot(0,this,SLOT(runExperiment()));
			data_.setNextStatus(PlaybackControllerData::Paused);
			break;
		case PlaybackCommand::Jump:
			playbackUpdater_->jumpToTime(cmd.commandData.toDouble());
			playbackUpdater_->pause();
			QTimer::singleShot(0,this,SLOT(runExperiment()));
			data_.setNextStatus(PlaybackControllerData::Paused);
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
		}
		break;
	case PlaybackControllerData::Running:
	case PlaybackControllerData::Paused:
		switch(cmd.commandType)
		{
		case PlaybackCommand::Play:
			playbackUpdater_->play();
			engine_->play();
			data_.setNextStatus(PlaybackControllerData::Running);
			break;
		case PlaybackCommand::Pause:
			playbackUpdater_->pause();
			data_.setNextStatus(PlaybackControllerData::Paused);
			break;
		case PlaybackCommand::Stop:
			engine_->stop();
			playbackUpdater_->stop();
			data_.setCurrTime(0.0);
			data_.setNextStatus(PlaybackControllerData::Stopped);
			break;
		case PlaybackCommand::Jump:
			if(cmd.commandData.toDouble() < data_.getCurrTime())
			{
				engine_->stop();
				QTimer::singleShot(0,this,SLOT(runExperiment()));
			}
			playbackUpdater_->jumpToTime(cmd.commandData.toDouble());
			data_.setNextStatus(status);
			break;
		case PlaybackCommand::Load:
			engine_->stop();
			playbackUpdater_->stop();
			data_.setCurrTime(0.0);
			data_.setNextStatus(PlaybackControllerData::Idle);
			data_.pushCommand(cmd,false);
			break;
		case PlaybackCommand::ChangeRun:
			engine_->stop();
			playbackUpdater_->stop();
			data_.setCurrTime(0.0);
			data_.setNextStatus(PlaybackControllerData::Stopped);
			data_.pushCommand(cmd,false);
			break;
		case PlaybackCommand::ChangeSpeed:
			playbackUpdater_->setPlaybackSpeed(cmd.commandData.toDouble());
			break;
		}
		break;
	}
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







PlaybackControllerData::PlaybackControllerData(){isSetup_ = false;currTime_ = 0;runSpeed_ = 0;runLength_ = 0;status_ = None;nextStatus_ = None;}
void PlaybackControllerData::setAsSetup(){QMutexLocker locker(&mutex_);isSetup_=true;}
bool PlaybackControllerData::isSetup(){QMutexLocker locker(&mutex_);return isSetup_;}
void PlaybackControllerData::setCurrTime(double val){QMutexLocker locker(&mutex_);currTime_=val;}
double PlaybackControllerData::getCurrTime(){QMutexLocker locker(&mutex_);return currTime_;}
void PlaybackControllerData::setRunSpeed(double val){QMutexLocker locker(&mutex_);runSpeed_ = val;}
double PlaybackControllerData::getRunSpeed(){QMutexLocker locker(&mutex_);return runSpeed_;}
void PlaybackControllerData::setRunLength(double val){QMutexLocker locker(&mutex_);runLength_ = val;}
double PlaybackControllerData::getRunLength(){QMutexLocker locker(&mutex_);return runLength_;}
void PlaybackControllerData::setStatus(Status val){QMutexLocker locker(&mutex_);status_ = val;}
PlaybackControllerData::Status PlaybackControllerData::getStatus(){QMutexLocker locker(&mutex_);return status_;}
void PlaybackControllerData::setNextStatus(Status val){QMutexLocker locker(&mutex_);nextStatus_ = val;}
PlaybackControllerData::Status PlaybackControllerData::getNextStatus(){QMutexLocker locker(&mutex_);return nextStatus_;}
void PlaybackControllerData::pushCommand(PlaybackCommand cmd, bool toBack){QMutexLocker locker(&mutex_);if(toBack)cmds_.push_back(cmd);else cmds_.push_front(cmd);}
PlaybackCommand PlaybackControllerData::getNextCommand(){QMutexLocker locker(&mutex_);PlaybackCommand returnVal; if(cmds_.size()){returnVal = cmds_.front();cmds_.pop_front();}return returnVal;}
