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
#include "../../common/storage/AssetExportImport.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

PlaybackController::PlaybackController()
{
	pixmapVisualTarget_ = QSharedPointer<Picto::PixmapVisualTarget>(new Picto::PixmapVisualTarget(true,800,600));
	outSigControllers_.push_back(QSharedPointer<Picto::VirtualOutputSignalController>(new VirtualOutputSignalController("BNC0")));
	outSigControllers_.push_back(QSharedPointer<Picto::VirtualOutputSignalController>(new VirtualOutputSignalController("PAR0")));
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	renderingTarget_ = QSharedPointer<Picto::RenderingTarget>(new Picto::RenderingTarget(pixmapVisualTarget_, pcmAuralTarget));
	currRun_ = 0;

	//This allows preloaded session data to be used in signal and slots
	qRegisterMetaType<PreloadedSessionData>();

	playbackThread_ = QSharedPointer<PlaybackThread>(new PlaybackThread());
	connect(playbackThread_.data(),SIGNAL(init()),this,SLOT(setup()));
	connect(playbackThread_.data(),SIGNAL(update()),this,SLOT(update()));
	playbackThread_->start();
	moveToThread(playbackThread_.data());
}

PlaybackController::~PlaybackController()
{
}

QString PlaybackController::preLoadSessions(QStringList filenames)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::PreLoad,filenames));
	return "";
}

//void PlaybackController::setEnabledAnalyses(QList<QUuid> analysisList)
//{
//	data_.setEnabledAnalyses(analysisList);
//}

QString PlaybackController::activateAnalyses(QStringList analysisData)
{
	QList<QUuid> localAnalyses;
	QStringList importAnalyses;
	foreach(QString analysisDataUnit,analysisData)
	{
		if(analysisDataUnit.contains("<"))
			importAnalyses.append(analysisDataUnit);
		else
			localAnalyses.append(QUuid(analysisDataUnit));
	}

	data_.clearEnabledBuiltInAnalyses();
	data_.clearEnabledImportedAnalyses();
	//Delete any analyses that were previously in the import list
	while(numImportedAnalyses_ > 0)
	{
		int lastAnalysisIndex = getDesignRoot()->getNumAnalyses()-1;
		QUuid lastAnalysisId = getDesignRoot()->getAnalysis(lastAnalysisIndex).staticCast<Analysis>()->getAssociateId();
		getDesignRoot()->getExperiment().staticCast<DataStore>()->ClearAssociateDescendants(lastAnalysisId);
		if(!getDesignRoot()->removeAnalysis(lastAnalysisIndex))
				Q_ASSERT(false);
		numImportedAnalyses_--;
	}

	//import analyses that are in the input string list and put them in the activated analysis list.
	QList<QUuid> importedAnalysisIds;
	foreach(QString analysis,importAnalyses)
	{
		//Create a new analysis to put the exported analysis in.
		QSharedPointer<Analysis> newAnalysis = getDesignRoot()->importAnalysis("<Analysis/>").staticCast<Analysis>();
		newAnalysis->setName(QUuid::createUuid().toString());	//Name doesn't matter.  Just make it unique
		QUuid newAnalysisId = newAnalysis->getAssociateId();
		bool linkResult = newAnalysis->LinkToAsset(getDesignRoot()->getExperiment(),QString());
		Q_ASSERT(linkResult);
		//Create UI Data for the new Analysis and attach it
		AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(newAnalysis.data());
		Q_ASSERT(assocRootHost);
		QUuid hostId = assocRootHost->getHostId();
		QSharedPointer<Asset> newUIData = newAnalysis->getParentAsset().staticCast<DataStore>()->createChildAsset("UIData",QString(),QString());
		QString feedback;
		newUIData.staticCast<AssociateRoot>()->LinkToAsset(newAnalysis,feedback);
		Q_ASSERT(newAnalysis);

		//Lets try to import the analysis contents into our new analysis from the exportText.
		QSharedPointer<AssetExportImport> exportImport(new AssetExportImport());
		int result = exportImport->importFromText(getDesignRoot()->getExperiment()
												,analysis
												,dynamic_cast<AssociateRootHost*>(getDesignRoot()->getExperiment().data())
												,QPoint(0,0)
												,dynamic_cast<AssociateRootHost*>(newAnalysis.data()));
		switch(result)
		{
		case AssetExportImport::IMPORT_SUCCEEDED:
			break;
		//case AssetExportImport::IMPORT_SUCCEEDED_WITH_WARNINGS:
		default:
			//Import failed, remove analysis from design
			getDesignRoot()->getExperiment().staticCast<DataStore>()->ClearAssociateDescendants(newAnalysisId);
	
			int analysisIndexToRemove = getDesignRoot()->getNumAnalyses()-1;
			if(!getDesignRoot()->removeAnalysis(analysisIndexToRemove))
				Q_ASSERT(false);
			return exportImport->getLatestMessage();
		}
		importedAnalysisIds.append(newAnalysisId);
		numImportedAnalyses_++;
	}
	
	data_.setEnabledBuiltInAnalyses(localAnalyses);
	data_.setEnabledImportedAnalyses(importedAnalysisIds);
	return "";
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

void PlaybackController::enableLFPLoad(bool enable)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::EnableLFP,enable));
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

QString PlaybackController::getLoadedFilePath()
{
	return data_.getLoadedFilePath();
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

void PlaybackController::play(QList<QUuid> activeAnalyses,QStringList importAnalyses)
{
	QStringList analysisList;
	foreach(QUuid analysisId,activeAnalyses)
		analysisList.append(analysisId.toString());
	data_.pushCommand(PlaybackCommand(PlaybackCommand::Play,analysisList+importAnalyses));
}

void PlaybackController::pause(QList<QUuid> activeAnalyses,QStringList importAnalyses)
{
	QStringList analysisList;
	foreach(QUuid analysisId,activeAnalyses)
	analysisList.append(analysisId.toString());
	data_.pushCommand(PlaybackCommand(PlaybackCommand::Pause,analysisList+importAnalyses));
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

void PlaybackController::selectFile(QString filePath)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeFile,filePath));
}

void PlaybackController::selectRun(int index)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeRun,index));
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

void PlaybackController::playbackEnded()
{
	emit finishedPlayback();
	stop();
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
	engine_->setSlaveMode(true);
	engine_->syncInitPropertiesForSlave(false);

	//Setup playback update system
	double playbackSpeed = 1.0;
	if(playbackUpdater_)
		playbackSpeed = playbackUpdater_->getPlaybackSpeed();
	playbackUpdater_ = QSharedPointer<PlaybackStateUpdater>(new PlaybackStateUpdater());
	playbackUpdater_->enableLfp(data_.getLfpEnabled());
	playbackUpdater_->setPlaybackSpeed(playbackSpeed);
	connect(playbackUpdater_.data(),SIGNAL(framePresented(double)),this,SLOT(setCurrTime(double)));
	connect(playbackUpdater_.data(),SIGNAL(loading(bool)),this,SIGNAL(loading(bool)));
	connect(playbackUpdater_.data(),SIGNAL(newRun(double)),this,SLOT(newRunLength(double)));
	connect(playbackUpdater_.data(),SIGNAL(finishedPlayback()),this,SLOT(playbackEnded()));
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

	numImportedAnalyses_ = 0;
	data_.clearEnabledBuiltInAnalyses();
	data_.clearEnabledImportedAnalyses();
	data_.setAsSetup();
}

void PlaybackController::update()
{
	PlaybackControllerData::Status status = data_.getStatus();
	PlaybackCommand cmd = data_.getNextCommand();
	QString errorMsg;
	switch(status)
	{
	case PlaybackControllerData::None:
		data_.setNextStatus(PlaybackControllerData::Idle);
		break;
	case PlaybackControllerData::Idle:
		switch(cmd.commandType)
		{
		case PlaybackCommand::PreLoad:
			data_.setNextStatus(PlaybackControllerData::PreLoading);
			data_.pushCommand(cmd,false);
			break;
		case PlaybackCommand::ChangeSpeed:
			playbackUpdater_->setPlaybackSpeed(cmd.commandData.toDouble());
			break;
		case PlaybackCommand::ChangeUserType:
			engine_->setOperatorAsUser(cmd.commandData.toInt() == PlaybackCommand::Operator);
			break;
		case PlaybackCommand::EnableLFP:
			Q_ASSERT(false);
			break;
		case PlaybackCommand::Stop:
			data_.setNextStatus(PlaybackControllerData::Stopped);
			break;
		case PlaybackCommand::Jump:
			data_.setNextStatus(status);
			break;
		}
		break;
	case PlaybackControllerData::PreLoading:
		switch(cmd.commandType)
		{
		case PlaybackCommand::PreLoad:
			{
				//Preload the first file in the list and send the designRoot to signal
				//listeners.  Then remove the file from the list and re-enter the command
				//until the list is empty.  At that point we can move on to stop.

				//Take first file path from cmd list
				QStringList filePaths = cmd.commandData.toStringList();
				QString filePath = filePaths.takeFirst();

				FileSessionLoader loader;
				if(!loader.setFile(filePath) || !loader.getDesignRoot())
				{
					emit sessionPreloadFailed("The session in file: " + filePath.toLatin1() + " could not be loaded.");
				}
				else
				{
					PreloadedSessionData preloadData;
					preloadData.fileName_ = filePath;
					preloadData.runs_ = loader.getRunNames();
					preloadData.notes_ = loader.getRunNotes();
					preloadData.savedRuns_ = loader.getSavedRunNames();
					QSharedPointer<DesignRoot> designRoot = loader.getDesignRoot();
					for(int i=0;i<designRoot->getNumAnalyses();i++)
					{
						preloadData.analysisIds_.append(designRoot->getAnalysis(i).staticCast<Analysis>()->getAssociateId());
						preloadData.analysisNames_.append(designRoot->getAnalysis(i)->getName());
					}
					emit sessionPreloaded(preloadData);
				}

				//If we're done with all the preloading, move to the stopped state.
				if(filePaths.isEmpty())
				{
					data_.setNextStatus(PlaybackControllerData::Stopped);
					break;
				}
				//If there are more files to preload, attach the updated list
				//to the command and put it back on the command queue.
				cmd.commandData = filePaths;
				data_.pushCommand(cmd);
				break;
			}
		case PlaybackCommand::ChangeSpeed:
			playbackUpdater_->setPlaybackSpeed(cmd.commandData.toDouble());
			break;
		case PlaybackCommand::ChangeUserType:
			engine_->setOperatorAsUser(cmd.commandData.toInt() == PlaybackCommand::Operator);
			break;
		case PlaybackCommand::EnableLFP:
			Q_ASSERT(false);
			break;
		case PlaybackCommand::Stop:
				data_.setNextStatus(PlaybackControllerData::Idle);
			break;
		case PlaybackCommand::Jump:
			data_.setNextStatus(status);
			break;
		}
	break;
	case PlaybackControllerData::Stopped:
		switch(cmd.commandType)
		{
		case PlaybackCommand::Play:
		case PlaybackCommand::Pause:
			data_.setNextStatus(PlaybackControllerData::Loading);
			data_.pushCommand(cmd,false);
			break;
		case PlaybackCommand::PreLoad:
			data_.setNextStatus(PlaybackControllerData::Idle);
			data_.pushCommand(cmd,false);
			break;
		case PlaybackCommand::ChangeFile:
			data_.setNextFilePath(cmd.commandData.toString());
			setCurrTime(0.0);
			break;
		case PlaybackCommand::ChangeRun:
			currRun_ = cmd.commandData.toInt();
			setCurrTime(0.0);
			break;
		case PlaybackCommand::ChangeSpeed:
			playbackUpdater_->setPlaybackSpeed(cmd.commandData.toDouble());
			break;
		case PlaybackCommand::ChangeUserType:
			engine_->setOperatorAsUser(cmd.commandData.toInt() == PlaybackCommand::Operator);
			break;
		case PlaybackCommand::EnableLFP:
			//If the session for the upcoming run is already loaded, set it to the playabck state updater.
			//otherwise this happens automatically during the setup() call used when the session is loaded
			if(data_.getLoadedFilePath() == data_.getNextFilePath())
			{
				if(playbackUpdater_)
					playbackUpdater_->enableLfp(cmd.commandData.toBool());
			}
			data_.enableLfp(cmd.commandData.toBool());
			break;
		case PlaybackCommand::Jump:
			data_.setNextStatus(status);
			break;
		}
		break;
	case PlaybackControllerData::Loading:
		switch(cmd.commandType)
		{
		case PlaybackCommand::Play:	
		case PlaybackCommand::Pause:
			{
				QString nextFilePath = data_.getNextFilePath();
				if(nextFilePath != data_.getLoadedFilePath())
				{
					setup();
					slaveExpDriver_.clear();	//Both SlaveExperimentDriver and Playback Updater contain pointers to the SessionState which is where
												//all the session data is stored in RAM.  By clearing the slaveExpDriver_ here, we can be sure that
												//RAM that was allocated for a previous playback will be freed up for loading the new file.  If	
												//we didn't do this, we would effectively be cutting our maximum RAM capacity in half because when 
												//switching from one session to the next, only half of the maximum RAM available to the application
												//is available for the new session.
					experiment_.clear();		//Experiment also uses up a lot of RAM.
					designRoot_.clear();		//Design root point to the experiment too.
					if(!playbackUpdater_->setFile(nextFilePath))
					{
						emit loadError("Failed to load session design.  This often means that the session contains no experimental data.");
						data_.setNextStatus(PlaybackControllerData::Stopped);
						break;
					}
					QSharedPointer<DesignRoot> newDesignRoot = QSharedPointer<DesignRoot>(playbackUpdater_->getDesignRoot());
					if(!newDesignRoot)
					{
						emit loadError("Failed to load session design.");
						data_.setNextStatus(PlaybackControllerData::Stopped);
						break;
					}
					if(!newDesignRoot->compiles())
					{
						emit loadError("Session's experiment does not compile.");
						data_.setNextStatus(PlaybackControllerData::Stopped);
						break;
					}
					QSharedPointer<Picto::Experiment> currExp = newDesignRoot->getExperiment().staticCast<Experiment>();
					if(!currExp || !currExp->getTaskNames().size())
					{
						emit loadError("Failed to load experiment from session.");
						data_.setNextStatus(PlaybackControllerData::Stopped);
						break;
					}

					if(!playbackUpdater_->loadRun(currRun_))
					{
						emit loadError("Failed to load data from this session run.  This is usually due to RAM issues when running a batch analysis.  Try analyzing this run by itself.");
						data_.setNextStatus(PlaybackControllerData::Stopped);
						break;
					}

					designRoot_ = newDesignRoot;
					experiment_ = currExp;
					experiment_->setEngine(engine_);

					numImportedAnalyses_ = 0;
					data_.clearEnabledBuiltInAnalyses();
					data_.clearEnabledImportedAnalyses();

					data_.setRunLength(playbackUpdater_->getRunLength());

					data_.setLoadedFilePath(nextFilePath);

					//Set up SlaveExperimentDriver to connect StateUpdater and Experiment
					slaveExpDriver_ = QSharedPointer<SlaveExperimentDriver>(new SlaveExperimentDriver(experiment_,playbackUpdater_));
					emit designRootChanged();
				}

				//If we got here, the session is loaded
				//Update to use the current set run
				playbackUpdater_->loadRun(currRun_);
				data_.setRunLength(playbackUpdater_->getRunLength());

				//Activate the necessary analyses
				errorMsg = activateAnalyses(cmd.commandData.toStringList());
				if(!errorMsg.isEmpty())
				{
					emit loadError(errorMsg);
					data_.setNextStatus(PlaybackControllerData::Stopped);
					break;
				}

				//Now that the analyses have been added, we need to reinitialize scripting for the experiment
				//Since a whole lot of memory is allocated at this point, the script engine allocation occuring
				//during the initScripting may fail, so we put this in a try catch block.
				try{
					if(!designRoot_->getExperiment().staticCast<Experiment>()->initScripting(false))
					{
						emit loadError("Failed to initialize the loaded session.  This may result from RAM issues when running batch analysis.  Try analyzing this run by itself.");
						data_.setNextStatus(PlaybackControllerData::Stopped);
						break;
					}
				}
				catch(...)
				{
					emit loadError("Failed to initialize the loaded session.  This may have resulted from RAM issues when running batch analysis.  Try analyzing this run by itself.");
					data_.setNextStatus(PlaybackControllerData::Stopped);
					break;
				}
				designRoot_->getExperiment()->getDesignConfig()->setActiveAnalysisIds(data_.getEnabledAnalyses());

				designRoot_->enableRunMode(true);//We do this here so that we're sure all the analyses are attached first
				playbackUpdater_->play();
				data_.setNextStatus(PlaybackControllerData::Running);
				if(cmd.commandType == PlaybackCommand::Pause)
					data_.pushCommand(cmd,false);
			}
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
		case PlaybackCommand::PreLoad:
			playbackUpdater_->stop();
			data_.setCurrTime(0.0);
			data_.setNextStatus(PlaybackControllerData::Idle);
			data_.pushCommand(cmd,false);
			break;
		case PlaybackCommand::ChangeFile:
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
		case PlaybackCommand::EnableLFP:
			Q_ASSERT(false);
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







PlaybackControllerData::PlaybackControllerData(){isSetup_ = false;currTime_ = 0;runSpeed_ = 0;runLength_ = 0;status_ = None;nextStatus_ = None;filePath_ = "";nextFilePath_ = "";lfpEnabled_ = true;}
void PlaybackControllerData::setAsSetup(){QMutexLocker locker(&mutex_);isSetup_=true;}
bool PlaybackControllerData::isSetup(){QMutexLocker locker(&mutex_);return isSetup_;}
void PlaybackControllerData::setCurrTime(double val){QMutexLocker locker(&mutex_);currTime_=val;}
double PlaybackControllerData::getCurrTime(){QMutexLocker locker(&mutex_);return currTime_;}
void PlaybackControllerData::setRunSpeed(double val){QMutexLocker locker(&mutex_);runSpeed_ = val;}
double PlaybackControllerData::getRunSpeed(){QMutexLocker locker(&mutex_);return runSpeed_;}
void PlaybackControllerData::setRunLength(double val){QMutexLocker locker(&mutex_);runLength_ = val;}
double PlaybackControllerData::getRunLength(){QMutexLocker locker(&mutex_);return runLength_;}
void PlaybackControllerData::setLoadedFilePath(QString filePath){QMutexLocker locker(&mutex_);filePath_ = filePath;}
QString PlaybackControllerData::getLoadedFilePath(){QMutexLocker locker(&mutex_);return filePath_;}
void PlaybackControllerData::setNextFilePath(QString filePath){QMutexLocker locker(&mutex_);nextFilePath_ = filePath;}
QString PlaybackControllerData::getNextFilePath(){QMutexLocker locker(&mutex_);return nextFilePath_;}
void PlaybackControllerData::enableLfp(bool enabled){QMutexLocker locker(&mutex_);lfpEnabled_ = enabled;}
bool PlaybackControllerData::getLfpEnabled(){QMutexLocker locker(&mutex_);return lfpEnabled_;}
void PlaybackControllerData::setEnabledBuiltInAnalyses(QList<QUuid> analysisList){QMutexLocker locker(&mutex_);enabledAnalyses_=analysisList;}
void PlaybackControllerData::setEnabledImportedAnalyses(QList<QUuid> analysisList){QMutexLocker locker(&mutex_);enabledImportedAnalyses_=analysisList;}
void PlaybackControllerData::clearEnabledBuiltInAnalyses(){QMutexLocker locker(&mutex_);enabledAnalyses_.clear();}
void PlaybackControllerData::clearEnabledImportedAnalyses(){QMutexLocker locker(&mutex_);enabledImportedAnalyses_.clear();}
QList<QUuid> PlaybackControllerData::getEnabledAnalyses(){QMutexLocker locker(&mutex_);return enabledAnalyses_ + enabledImportedAnalyses_ ;}
void PlaybackControllerData::setStatus(Status val){QMutexLocker locker(&mutex_);status_ = val;}
PlaybackControllerData::Status PlaybackControllerData::getStatus(){QMutexLocker locker(&mutex_);return status_;}
void PlaybackControllerData::setNextStatus(Status val){QMutexLocker locker(&mutex_);nextStatus_ = val;}
PlaybackControllerData::Status PlaybackControllerData::getNextStatus(){QMutexLocker locker(&mutex_);return nextStatus_;}
void PlaybackControllerData::pushCommand(PlaybackCommand cmd, bool toBack){QMutexLocker locker(&mutex_);if(toBack)cmds_.push_back(cmd);else cmds_.push_front(cmd);}
PlaybackCommand PlaybackControllerData::getNextCommand(){QMutexLocker locker(&mutex_);PlaybackCommand returnVal; if(cmds_.size()){returnVal = cmds_.front();cmds_.pop_front();}return returnVal;}
