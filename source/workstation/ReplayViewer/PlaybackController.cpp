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

#if defined WIN32 || defined WINCE
#include <Windows.h>
#endif

#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a PlaybackController object.
 *	\details Along with variable initialization, this function creates a new playback QThread, starts its event loop and
 *	moves this object to that thread.  This means that the Session playback/loading will occur in a different, non-gui
 *	thread.  This is good because it will prevent long processes like loading of session data from tying up the UI.
 */
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

/*! \brief Generates a new PlaybackCommand directing the PlaybackController to preload all of the files with paths in the
 *	input list.
 *	\details Preload loads meta data about the file without loading the large quantities of actual Session data.
 *	These will be loaded later if Run from a file are selected for playback and run.
 */
QString PlaybackController::preLoadSessions(QStringList filenames)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::PreLoad,filenames));
	return "";
}

//! Creates names for potentially imported Analyses so that the TaskConfig can idenfify them before they are first run.
QStringList PlaybackController::precacheAnalysisNames(QSharedPointer<DesignRoot> import)
{
	Q_ASSERT(import);
	QStringList resultList;

	for (int i = 0; i<import->getNumAnalyses(); i++)
	{
		//Try to export it.
		QSharedPointer<AssetExportImport> exportImport(new AssetExportImport());
		QString exportText = exportImport->exportToText((QList<QSharedPointer<Asset>>() << import->getExperiment()),
			dynamic_cast<AssociateRootHost*>(import->getAnalysis(i).data()));

		if (!cachedAnalysisNames_.contains(exportText))
		{
			//Create a new name and cache it.
			cachedAnalysisNames_[exportText] = QUuid::createUuid().toString();
		}
		resultList << cachedAnalysisNames_[exportText];
	}

	return resultList;
}

/*! \brief Activates the input Analyses in the Experiment so that Analysis will occur during playback.
 *	@param analysisData This can either be a list of exported analysis strings or a list of Analysis
 *		AssocateIDs.  If the list includes exported analysis strings, the analyses will be imported
 *		into the Experiment and (if successful) activated.  If the list includes Analysis Associate IDs
 *		they are assumed to be local Analyses.  The loaded Design will be checked for the Analyses and they
 *		will be activated.
 *	\design If a problem occurs with the activation, a non-empty string will be returned with a description
 *	of the problem.
 */
QString PlaybackController::activateAnalyses(QStringList analysisData)
{
	QList<QUuid> localAnalyses;
	//import analyses that are in the input string list and put them in the activated analysis list.
	QList<QUuid> importedAnalysisIds;
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
	QStringList::iterator itrImportAnalysis = importAnalyses.begin();
	while (itrImportAnalysis != importAnalyses.end())
	{
		if (cachedAnalysis_.contains(*itrImportAnalysis))
		{
			//Append AnalysisId and remove from import list if it's cached
			importedAnalysisIds.append(cachedAnalysis_[*itrImportAnalysis]);
			itrImportAnalysis = importAnalyses.erase(itrImportAnalysis);
		}
		else
		{
			//Continue if it's not cached
			++itrImportAnalysis;
		}

	}

	//Clear unused cached analysis.
	int savedAnalyses = 0;
	while (numImportedAnalyses_ - savedAnalyses > 0)
	{
		int nextAnalysisIndex = getDesignRoot()->getNumAnalyses() - (1 + savedAnalyses);
		QUuid nextAnalysisId = getDesignRoot()->getAnalysis(nextAnalysisIndex).staticCast<Analysis>()->getAssociateId();
		if (importedAnalysisIds.contains(nextAnalysisId))
		{
			//We are reimporting this analysis - don't delete, continue
			savedAnalyses++;
		}
		else
		{
			//We are deleting this analysis
			getDesignRoot()->getExperiment().staticCast<DataStore>()->ClearAssociateDescendants(nextAnalysisId);
			cachedAnalysis_.remove(cachedAnalysis_.key(nextAnalysisId));
			if (!getDesignRoot()->cullAnalysis(nextAnalysisIndex))
				Q_ASSERT(false);
			numImportedAnalyses_--;
		}
	}

	//We should only have new analyses left in the importAnalyses list
	foreach(QString analysis,importAnalyses)
	{
		//Create a new analysis to put the exported analysis in.
		QSharedPointer<Analysis> newAnalysis = getDesignRoot()->importAnalysis("<Analysis/>").staticCast<Analysis>();

		QString newName;
		if (cachedAnalysisNames_.contains(analysis))
		{
			//Reassign the old name if an analysis identical to this one was imported.
			newName = cachedAnalysisNames_[analysis];
		}
		else
		{
			//Otherwise create a new name and cache it.
			newName = QUuid::createUuid().toString();
			cachedAnalysisNames_[analysis] = newName;
		}

		//Make the name unique
		newAnalysis->setName(newName);
		QUuid newAnalysisId = newAnalysis->getAssociateId();


		bool linkResult = newAnalysis->LinkToAsset(getDesignRoot()->getExperiment(), QString(), true);
		IGNORED_PARAMETER(linkResult);
		Q_ASSERT(linkResult);
		//Create UI Data for the new Analysis and attach it
		AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(newAnalysis.data());
		Q_ASSERT(assocRootHost);
		QUuid hostId = assocRootHost->getHostId();
		QSharedPointer<Asset> newUIData = newAnalysis->getParentAsset().staticCast<DataStore>()->createChildAsset("UIData", QString(), QString());
		QString feedback;
		newUIData.staticCast<AssociateRoot>()->LinkToAsset(newAnalysis, feedback);
		Q_ASSERT(newAnalysis);

		//Lets try to import the analysis contents into our new analysis from the exportText.
		QSharedPointer<AssetExportImport> exportImport(new AssetExportImport());
		int result = exportImport->importFromText(getDesignRoot()->getExperiment(),
				analysis,
				dynamic_cast<AssociateRootHost*>(getDesignRoot()->getExperiment().data()),
				QPoint(0, 0),
				dynamic_cast<AssociateRootHost*>(newAnalysis.data()));

		switch (result)
		{
		case AssetExportImport::IMPORT_SUCCEEDED:
			break;
			//case AssetExportImport::IMPORT_SUCCEEDED_WITH_WARNINGS:
		default:
			//Import failed, remove analysis from design
			getDesignRoot()->getExperiment().staticCast<DataStore>()->ClearAssociateDescendants(newAnalysisId);

			int analysisIndexToRemove = getDesignRoot()->getNumAnalyses() - 1;
			if (!getDesignRoot()->cullAnalysis(analysisIndexToRemove))
				Q_ASSERT(false);
			return exportImport->getLatestMessage();
		}

		//Add cached analysis to our hash table
		cachedAnalysis_[analysis] = newAnalysisId;
		importedAnalysisIds.append(newAnalysisId);
		numImportedAnalyses_++;
	}
	
	data_.setEnabledBuiltInAnalyses(localAnalyses);
	data_.setEnabledImportedAnalyses(importedAnalysisIds);
	return "";
}

/*! \brief Generates a new PlaybackCommand directing the PlaybackController to change the 
 *	playback speed to the input value.
 *	@param value A factor by which playback speed is multiplied (ie. 2 implies double the speed).
 */
void PlaybackController::setRunSpeed(double value)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeSpeed,value));
}

/*! \brief Generates a new PlaybackCommand directing the PlaybackController to change the current
 *	playback user type to Operator.  This will cause VisualElement objects with OperatorView
 *	set to be visible during playback.
 */
void PlaybackController::setUserToOperator()
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeUserType,PlaybackCommand::Operator));
}

/*! \brief Generates a new PlaybackCommand directing the PlaybackController to change the current
 *	playback user type to Subject.  This will cause VisualElement objects with SubjectView
 *	set to be visible during playback.
 */
void PlaybackController::setUserToSubject()
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeUserType,PlaybackCommand::TestSubject));
}

/*! \brief Generates a new PlaybackCommand directing the PlaybackController to enable/disable loading
 *	of LFP data.
 *	\details Disabling LFP data can have a significant effect in improving Session load times. 
 */
void PlaybackController::enableLFPLoad(bool enable)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::EnableLFP,enable));
}

/*! \brief Returns the Picto::VisualTarget that will be used to render VisualElements in the played back
 *	Session.
 */
QSharedPointer<Picto::VisualTarget> PlaybackController::getVisualTarget()
{
	return pixmapVisualTarget_;
}

/*! \brief Returns the Picto::VirtualOutputSignalController that will be used to drive output signals in the played back
 *	Session.
 */
QVector<QSharedPointer<Picto::VirtualOutputSignalController>> PlaybackController::getOutputSignalControllers()
{
	return outSigControllers_;
}

/*! \brief Returns the Picto::RenderingTarget on which VisualElements will be rendered in the played back
 *	Session.
 */
QSharedPointer<Picto::RenderingTarget> PlaybackController::getRenderingTarget()
{
	return renderingTarget_;
}

/*! \brief Returns the Picto::DesignRoot of the Design in the currently loaded Session.
 */
QSharedPointer<Picto::DesignRoot> PlaybackController::getDesignRoot()
{
	return designRoot_;
}

/*! \brief Returns the length in seconds of the currently loaded run.
 */
double PlaybackController::getRunLength()
{
	return data_.getRunLength();
}

/*! \brief Returns the FilePath of the Session file for the currently loaded Session.
 */
QString PlaybackController::getLoadedFilePath()
{
	return data_.getLoadedFilePath();
}

/*! \brief Called when Picto is about to quit.  Stops playback, quits the playback thread, waits for rendering to end
 * in the GUI thread (see Picto::Scene::closeRenderLoops()), waits for the playback thread to stop then returns.
 */
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

/*! \brief Generates a new PlaybackCommand directing the PlaybackController to start playback of the
 *	currently selected run.
 *	@param activeAnalyses A list of Analysis Associate Ids that are included in the active Session's
 *		design and should be activated during playback.
 *	@param importAnalyses A list of exported analysis code that should be imported into the active Session
 *		for analysis during playback.
 *	\sa activateAnalyses()
 */
void PlaybackController::play(QList<QUuid> activeAnalyses,QStringList importAnalyses)
{
	QStringList analysisList;
	foreach(QUuid analysisId,activeAnalyses)
		analysisList.append(analysisId.toString());
	data_.pushCommand(PlaybackCommand(PlaybackCommand::Play,analysisList+importAnalyses));
}

/*! \brief Generates a new PlaybackCommand directing the PlaybackController to pause playback of the
 *	currently selected run.
 *	@param activeAnalyses A list of Analysis Associate Ids that are included in the active Session's
 *		design and should be activated during playback.
 *	@param importAnalyses A list of exported analysis code that should be imported into the active Session
 *		for analysis during playback.
 *	\note Runs can be loaded and started by pressing play, in which case playback starts, or pause, in which
 *	case the Run loads and playback moves to time zero and then waits for play to be pressed.  
 *	The reason that we include these two input parameters is for the case where the Run is started
 *	with a pause.
 *	\sa activateAnalyses()
 */
void PlaybackController::pause(QList<QUuid> activeAnalyses,QStringList importAnalyses)
{
	QStringList analysisList;
	foreach(QUuid analysisId,activeAnalyses)
		analysisList.append(analysisId.toString());
	data_.pushCommand(PlaybackCommand(PlaybackCommand::Pause,analysisList+importAnalyses));
}

/*! \brief Generates a new PlaybackCommand directing the PlaybackController to stop playback of the
 *	currently selected run.
 */
void PlaybackController::stop()
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::Stop));
}

/*! \brief Generates a new PlaybackCommand directing the PlaybackController to jump the playback time to the
 *	input time (in seconds).
 *	\note Jumping backwards in time results in playback starting from zero and moving forward to that time.  This
 *	is to make sure that analyses that can only be run during a fast forward, not a rewind, will be valid.
 */
void PlaybackController::jumpToTime(double time)
{
	if(time > getRunLength() || time < 0)
		return;
	data_.pushCommand(PlaybackCommand(PlaybackCommand::Jump,time));
}

/*! \brief Generates a new PlaybackCommand directing the PlaybackController to select a new Session file for playback.
 */
void PlaybackController::selectFile(QString filePath)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeFile,filePath));
}

/*! \brief Generates a new PlaybackCommand directing the PlaybackController to select the run at the input index of the 
 *	current Session file for playback.
 */
void PlaybackController::selectRun(int index)
{
	data_.pushCommand(PlaybackCommand(PlaybackCommand::ChangeRun,index));
}

/*! \brief Updates the length of the current run according to the PlaybackStateUpdater::newRun() signal.*/
void PlaybackController::newRunLength(double length)
{
	data_.setRunLength(length);
}

/*! \brief Updates the current time according to the PlaybackStateUpdater::framePresented() signal.
 */
void PlaybackController::setCurrTime(double time)
{
	data_.setCurrTime(time);
	emit timeChanged(time);
}

/*! \brief Called when the PlaybackStateUpdater::finishedPlayback() signal is emitted.  Causes
 *	the PlaybackController::finishedPlayback() signal to be emitted and stops the PlaybackController.
 */
void PlaybackController::playbackEnded()
{
	emit finishedPlayback();
	stop();
}

/*! \brief Sets up the PlaybackController for playback of a new Task Run.
 */
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

	QSharedPointer<Picto::GenericInput> genericInputs(new Picto::GenericInput("GenericInputs"));
	engine_->addSignalChannel(genericInputs);

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

/*! \brief Updates the PlaybackController's StateMachine.
 *	\details Each time this is called, the next command from the PlaybackControllerData object is taken from the queue.
 *	Depending on the current state, the command is handled in different ways.  Sometimes it even causes the  
 *	state (called status here) to change and is pushed back onto the front of the command list so that the new state
 *	can handle it the next time update() is called.  Whenever the state does change, the change is reported with
 *	the statusChanged() signal.
 *	\note One of the major issues in the current playback system is that the entire active Session's data is stored in
 *	RAM.  This causes some memory issues.  There is some complex code in the part of this function handling the
 *	"Loading" state that attempts to make sure that only one session is ever in RAM at a given time and tries to
 *	defragment the RAM (when we're in Windows) before loading the session.  Don't let this code scare you.  That's 
 *	all it does.  In the future, we should spend some time working on new methods for improving RAM usage like storing
 *	parts of the Session data lists in simple serial files until they are needed.
 */
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

					//Both SlaveExperimentDriver and Playback Updater contain pointers to the SessionState which is where
					//all the session data is stored in RAM.  By clearing the slaveExpDriver_ here, we can be sure that
					//RAM that was allocated for a previous playback will be freed up for loading the new file.  If	
					//we didn't do this, we would effectively be cutting our maximum RAM capacity in half because when 
					//switching from one session to the next, only half of the maximum RAM available to the application
					//is available for the new session.
					slaveExpDriver_.clear();
					//Experiment also uses up a lot of RAM.
					experiment_.clear();
					//Design root point to the experiment too.
					designRoot_.clear();

					//Clear the hashtables for cached analyses
					cachedAnalysis_.clear();

#if defined WIN32 || defined WINCE
					//Since we're creating lots of giant data structures on the heap, we found that there were some heap
					//	fragmentation issues.  ie.  'new' commands would start to fail wbile creating analysis data
					//	structures after we analyzed multiple other sessions, presumably since there weren't large enough
					//	holes in memory for the new data structures.  For this reason, we are defragmenting the heap each
					//	time we start loading a new session.  This code for heap deframentation is
					//	from: http://www.cplusplus.com/forum/windows/8010/
					{	
					  const int MaxNumHeaps= 5000;
					  HANDLE HeapHandles[MaxNumHeaps];
					  int NumUsedHeaps= GetProcessHeaps(MaxNumHeaps,HeapHandles);
					  for (int i=0;i<NumUsedHeaps;i++)
					  {
						HeapCompact(HeapHandles[i],0);
					  }
					}
					///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

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
					connect(slaveExpDriver_.data(), SIGNAL(taskChanged(QString)), this, SIGNAL(taskChanged(QString)));
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
				try
				{
					if (!designRoot_->getExperiment().staticCast<Experiment>()->deepReinitScripting(false))
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
/*! \brief Marks the PlaybackController as having been setup.*/
void PlaybackControllerData::setAsSetup(){QMutexLocker locker(&mutex_);isSetup_=true;}
/*! \brief Returns true if the PlaybackController was setup.*/
bool PlaybackControllerData::isSetup(){QMutexLocker locker(&mutex_);return isSetup_;}
/*! \brief Sets the current PlaybackController's current playback time.*/
void PlaybackControllerData::setCurrTime(double val){QMutexLocker locker(&mutex_);currTime_=val;}
/*! \brief Returns the PlaybackController's current time.*/
double PlaybackControllerData::getCurrTime(){QMutexLocker locker(&mutex_);return currTime_;}
/*! \brief Sets the PlaybackController's run speed as a factor (2.0 means double speed).*/
void PlaybackControllerData::setRunSpeed(double val){QMutexLocker locker(&mutex_);runSpeed_ = val;}
/*! \brief Returns the PlaybackController's run speed.
 *	\sa setRunSpeed()
 */
double PlaybackControllerData::getRunSpeed(){QMutexLocker locker(&mutex_);return runSpeed_;}
/*! \brief Sets the length in seconds of the PlaybackController's current Task Run.*/
void PlaybackControllerData::setRunLength(double val){QMutexLocker locker(&mutex_);runLength_ = val;}
/*! \brief Gets the length in seconds of the PlaybackContrller's current task Run.*/
double PlaybackControllerData::getRunLength(){QMutexLocker locker(&mutex_);return runLength_;}
/*! \brief Sets the file path of the currently loaded Session.*/
void PlaybackControllerData::setLoadedFilePath(QString filePath){QMutexLocker locker(&mutex_);filePath_ = filePath;}
QString PlaybackControllerData::getLoadedFilePath(){QMutexLocker locker(&mutex_);return filePath_;}
/*! \brief Sets the file path of the next Session that should be loaded.*/
void PlaybackControllerData::setNextFilePath(QString filePath){QMutexLocker locker(&mutex_);nextFilePath_ = filePath;}
/*! \brief Gets the file path of the next Session that should be loaded.*/
QString PlaybackControllerData::getNextFilePath(){QMutexLocker locker(&mutex_);return nextFilePath_;}
/*! \brief Sets LFP loading enabled for the PlaybackController.*/
void PlaybackControllerData::enableLfp(bool enabled){QMutexLocker locker(&mutex_);lfpEnabled_ = enabled;}
/*! \brief Returns whether LFP loading is enabled for the PlaybackController.*/
bool PlaybackControllerData::getLfpEnabled(){QMutexLocker locker(&mutex_);return lfpEnabled_;}
/*! \brief Sets a list of Analysis Associate IDs which should be marked as active in the currently loaded Session.*/
void PlaybackControllerData::setEnabledBuiltInAnalyses(QList<QUuid> analysisList){QMutexLocker locker(&mutex_);enabledAnalyses_=analysisList;}
/*! \brief Sets a list of exported Analysis code that should be imported into the currently loaded Session and marked as active.*/
void PlaybackControllerData::setEnabledImportedAnalyses(QList<QUuid> analysisList){QMutexLocker locker(&mutex_);enabledImportedAnalyses_=analysisList;}
/*! \brief Clears the list of Analysis Associate IDs that are supposed to be actived in the currently loaded Session.*/
void PlaybackControllerData::clearEnabledBuiltInAnalyses(){QMutexLocker locker(&mutex_);enabledAnalyses_.clear();}
/*! \brief Clears the list of exported Analysesthat are supposed to be imported and actived in the currently loaded Session.*/
void PlaybackControllerData::clearEnabledImportedAnalyses(){QMutexLocker locker(&mutex_);enabledImportedAnalyses_.clear();}
/*! \brief Returns a list of Analysis Associate IDs that are active in the currently loaded Session.*/
QList<QUuid> PlaybackControllerData::getEnabledAnalyses(){QMutexLocker locker(&mutex_);return enabledAnalyses_ + enabledImportedAnalyses_ ;}
/*! \brief Sets the PlaybackController's current Status.
 *	\note What is referred to here as Status is actually a state of the PlaybackController state machine.
 */
void PlaybackControllerData::setStatus(Status val){QMutexLocker locker(&mutex_);status_ = val;}
/*! \brief Gets the PlaybackController's current Status.
 *	\note What is referred to here as Status is actually a state of the PlaybackController state machine.
 */
PlaybackControllerData::Status PlaybackControllerData::getStatus(){QMutexLocker locker(&mutex_);return status_;}
/*! \brief Sets the PlaybackController's next Status.
 *	\note What is referred to here as Status is actually a state of the PlaybackController state machine.
 */
void PlaybackControllerData::setNextStatus(Status val){QMutexLocker locker(&mutex_);nextStatus_ = val;}
/*! \brief Gets the PlaybackController's next Status.
 *	\note What is referred to here as Status is actually a state of the PlaybackController state machine.
 */
PlaybackControllerData::Status PlaybackControllerData::getNextStatus(){QMutexLocker locker(&mutex_);return nextStatus_;}
/*! \brief Pushes the input PlaybackCommand onto the PlaybackCommand queue for retrieval in getNextCommand().
 *	@param cmd The PlaybackCommand to be added to the queue.
 *	@param toBack If true (the default), the command is added to the back of the list.  If false it is added to the front of the list and will be 
 *	returned when getNextCommand() is called next.
 */
void PlaybackControllerData::pushCommand(PlaybackCommand cmd, bool toBack){QMutexLocker locker(&mutex_);if(toBack)cmds_.push_back(cmd);else cmds_.push_front(cmd);}
/*! \brief Returns and removes the first command in the command queue.*/
PlaybackCommand PlaybackControllerData::getNextCommand(){QMutexLocker locker(&mutex_);PlaybackCommand returnVal; if(cmds_.size()){returnVal = cmds_.front();cmds_.pop_front();}return returnVal;}
