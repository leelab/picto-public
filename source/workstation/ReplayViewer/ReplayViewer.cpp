#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QXmlStreamReader>
#include <QMenu>
#include <QFileDialog>
#include <QSlider>
#include <QApplication>
#include <QProgressBar>

#include "ReplayViewer.h"

#include "SaveOutputDialog.h"
#include "../../common/storage/AssetExportImport.h"
#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/designer/propertyframe.h"
#include "../../common/engine/XYSignalChannel.h"
#include "../../common/engine/MouseInputPort.h"
#include "../../common/parameter/OperatorClickParameter.h"
#include "../../common/compositor/OutputSignalWidget.h"
#include "../../common/iodevices/BufferFileGenerator.h"
#include "../../common/playback/FileSessionLoader.h"


#include "../../common/memleakdetect.h"


ReplayViewer::ReplayViewer(QWidget *parent) :
	Viewer(parent)
{
	//Create the Playback Controller that handles all Experiment playback
	playbackController_ = QSharedPointer<PlaybackController>(new PlaybackController());
	connect(playbackController_.data(),SIGNAL(statusChanged(int)),this,SLOT(playbackStatusChanged(int)));

	//Setup the user interface
	setupUi();
}

//! Called just before displaying the viewer
void ReplayViewer::init()
{
	jumpDownRequested_ = false;
	analysisSelector_->setDesignRootForImport(designRoot_);
}

//!Called just before hiding the viewer
void ReplayViewer::deinit()
{
	stop();
	emit deinitComplete();
}

//! \brief Called when the application is about to quit.  Takes care of closing this windows resources
bool ReplayViewer::aboutToQuit()
{
	playbackController_->aboutToQuit();
	return true;
}

//! Sets up the user interface portions of the GUI
void ReplayViewer::setupUi()
{
	//Load Session Actions
	loadSessionAction_ = new QAction(tr("Open Session"),this);
	loadSessionAction_->setIcon(QIcon(":/icons/sessionopen.png"));
	connect(loadSessionAction_, SIGNAL(triggered()),this, SLOT(loadSession()));
	loadSessionAction_->setEnabled(true);

	runs_ = new TaskSelectorWidget();
	connect(runs_,SIGNAL(runSelectionChanged()),this,SLOT(runSelectionChanged()));
	//connect(playbackController_.data(),SIGNAL(runsUpdated(QStringList,QStringList)),this,SLOT(updateRunsList(QStringList,QStringList)));

	///play/pause/stop actions and toolbar
	playAction_ = new QAction(tr("&Start task"),this);
	playAction_->setIcon(QIcon(":/icons/play.png"));
	playAction_->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
	playAction_->setToolTip("Run (Ctrl+R)");
	connect(playAction_,SIGNAL(triggered()),this, SLOT(runNormal()));
	playing_ = false;	//True when an experiment is playing
	paused_ = false;	//True when experiment is paused
	useRunToEnd_ = false;
	startingRun_ = false;
	calledPlayNotPause_ = false;
	latestRun_ = PlayRunInfo();

	///playall
	runToEndAction_ = new QAction(tr("&Run to end"),this);
	runToEndAction_->setIcon(QIcon(":/icons/playall.png"));
	//runToEndAction_->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
	runToEndAction_->setToolTip("Run to end");
	connect(runToEndAction_,SIGNAL(triggered()),this, SLOT(runToEnd()));

	speed_ = new SpeedWidget(100,0.01,0.01,1.0);
	connect(speed_,SIGNAL(speedChanged(double)),playbackController_.data(),SLOT(setRunSpeed(double)));

	pauseAction_ = new QAction(tr("&Pause task"),this);
	pauseAction_->setIcon(QIcon(":/icons/pause.png"));
	pauseAction_->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_P));
	pauseAction_->setToolTip("Pause (Ctrl+P)");
	connect(pauseAction_,SIGNAL(triggered()),this, SLOT(pause()));
	pauseAction_->setEnabled(false);
	pausedFromJump_ = false;

	stopAction_ = new QAction(tr("S&top task"),this);
	stopAction_->setIcon(QIcon(":/icons/stop.png"));
	stopAction_->setToolTip("Stop");
	connect(stopAction_,SIGNAL(triggered()),this, SLOT(stop()));
	connect(playbackController_.data(),SIGNAL(finishedPlayback()),this,SLOT(finishedPlayback()));
	stopAction_->setEnabled(false);

	userType_ = new QComboBox();
	userType_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	userType_->setToolTip("Select a User View");
	userType_->addItem("Operator",0);
	userType_->addItem("Test Subject",1);
	connect(userType_,SIGNAL(currentIndexChanged(int)),this,SLOT(setUserType(int)));

	progress_ = new ProgressWidget();
	progress_->setMaximum(1);
	progress_->setHighlightColor(0,QColor("#88f"));
	progress_->setHighlightColor(1,QColor("#f88"));
	connect(playbackController_.data(),SIGNAL(timeChanged(double)),this,SLOT(updateTime(double)));
	connect(playbackController_.data(),SIGNAL(loadedTo(double,double)),this,SLOT(updateLoadTimes(double,double)));
	connect(progress_,SIGNAL(valueRequested(double)),this,SLOT(jumpRequested(double)));
	connect(progress_,SIGNAL(userAction(bool)),this,SLOT(userChoosingJump(bool)));
	
	loadProgress_ = new QProgressBar();
	loadProgress_->setMaximum(100);
	loadProgress_->setMinimum(0);
	loadProgress_->setValue(0);
	connect(playbackController_.data(),SIGNAL(percentLoaded(double)),this,SLOT(percentLoaded(double)));
	connect(playbackController_.data(),SIGNAL(loadError(QString)),this,SLOT(loadError(QString)));
	
	toggleRecord_ = new QAction(tr("&Toggle Record"),this);
	toggleRecord_->setIcon(QIcon(":/icons/red_led_off.svg"));
	connect(toggleRecord_, SIGNAL(triggered()),this, SLOT(toggleRecording()));
	toggleRecord_->setEnabled(false);
	recordModeOn_ = false;	//Indicates if the toggleRecord button is set for record mode or not

	recordTime_ = new QLCDNumber(6);
	recordTime_->setSegmentStyle(QLCDNumber::Flat);
	setRecordTime(0);

	restartRecord_ = new QAction(tr("&Restart Record"),this);
	restartRecord_->setIcon(QIcon(":/icons/restart.svg"));
	connect(restartRecord_, SIGNAL(triggered()),this, SLOT(restartRecording()));
	restartRecord_->setEnabled(false);

	saveRecording_ = new QAction(tr("&Save Recording"),this);
	saveRecording_->setIcon(QIcon(":/icons/savevideo.svg"));
	connect(saveRecording_, SIGNAL(triggered()),this, SLOT(saveRecording()));
	saveRecording_->setEnabled(false);

	testToolbar_ = new QToolBar(this);
	testToolbar_->addAction(loadSessionAction_);
	testToolbar_->addSeparator();
	testToolbar_->addAction(playAction_);
	testToolbar_->addAction(runToEndAction_);
	testToolbar_->addAction(pauseAction_);
	testToolbar_->addAction(stopAction_);
	testToolbar_->addSeparator();
	testToolbar_->addWidget(userType_);
	testToolbar_->addSeparator();
	testToolbar_->addAction(toggleRecord_);
	testToolbar_->addWidget(recordTime_);
	testToolbar_->addAction(restartRecord_);
	testToolbar_->addAction(saveRecording_);
	testToolbar_->addSeparator();

	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(testToolbar_);
	toolbarLayout->addStretch();

	QVBoxLayout *stimulusLayout = new QVBoxLayout;
	//Set up the visual target host
	//This exists because QSharedPointer<QWidget> results in multiple delete call, which 
	//gives us memory exceptions.
	visualTargetHost_ = new RecordingVisualTargetHost();
	visualTargetHost_->setVisualTarget(playbackController_->getVisualTarget());
	stimulusLayout->addWidget(visualTargetHost_);
	connect(visualTargetHost_,SIGNAL(updateRecordingTime(double)),this,SLOT(setRecordTime(double)));

	//Setup Output Signal Widgets
	foreach(QSharedPointer<Picto::VirtualOutputSignalController> cont
		,playbackController_->getOutputSignalControllers())
	{
		cont->moveToThread(QApplication::instance()->thread());
		outputSignalsWidgets_.push_back(new OutputSignalWidget(cont));
		stimulusLayout->addWidget(outputSignalsWidgets_.back());
	}
	stimulusLayout->addWidget(progress_);

	//Setup analysis widgets
	QTabWidget* analysisTabs = new QTabWidget();

	analysisSelector_ = new Picto::AnalysisSelectorWidget();
	analysisSelector_->setEnabled(false);
	connect(playbackController_.data(),SIGNAL(designRootChanged()),this,SLOT(designRootChanged()));
	outputWidgetHolder_ = new OutputWidgetHolder();
	analysisTabs->addTab(runs_,"Select Runs");
	analysisTabs->addTab(analysisSelector_,"Select Analyses");
	analysisTabs->addTab(outputWidgetHolder_,"Analysis Output");
	connect(playbackController_.data(),SIGNAL(analysesImportFailed(QString)),this,SLOT(analysesImportedFailed(QString)));

	QVBoxLayout *infoLayout = new QVBoxLayout();
	infoLayout->addWidget(analysisTabs);
	QHBoxLayout *progressLayout = new QHBoxLayout();
	progressLayout->addWidget(new QLabel("Load Progress:"));
	progressLayout->addWidget(loadProgress_);
	infoLayout->addLayout(progressLayout);

	QHBoxLayout *operationLayout = new QHBoxLayout;
	operationLayout->addLayout(infoLayout);
	operationLayout->addLayout(stimulusLayout);
	operationLayout->addWidget(speed_);
	operationLayout->addStretch();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	mainLayout->addLayout(operationLayout);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}

//Updates the Recording Visual Target Host to record or not depending
//on the current run and record mode states.
void ReplayViewer::updateRecordingTarget()
{
	if(recordModeOn_ && playing_)
	{
		if(!visualTargetHost_->isRecording())
			visualTargetHost_->toggleRecording();
	}
	else
	{
		if(visualTargetHost_->isRecording())
			visualTargetHost_->toggleRecording();
	}
}

QList<QUuid> ReplayViewer::getSelectedLocalAnalyses()
{
	return analysisSelector_->getSelectedAnalysisIds();
}

QStringList ReplayViewer::getAnalysesToImport()
{
	QList<QUuid> importAnalysisIds = analysisSelector_->getSelectedAnalysisIdsForImport();
	QStringList resultList;
	for(int i=0;i<designRoot_->getNumAnalyses();i++)
	{
		//If this analysis is in our import list
		if(importAnalysisIds.contains(designRoot_->getAnalysis(i).staticCast<Analysis>()->getAssociateId()))
		{
			//Try to export it.
			QSharedPointer<AssetExportImport> exportImport(new AssetExportImport());
			QString exportText = exportImport->exportToText((QList<QSharedPointer<Asset>>() << designRoot_->getExperiment())
				,dynamic_cast<AssociateRootHost*>(designRoot_->getAnalysis(i).data()));
			resultList.append(exportText);
		}
	}
	return resultList;
}

//bool ReplayViewer::activateSelectedAnalyses()
//{
//	QList<QUuid> importAnalysisIds = analysisSelector_->getSelectedAnalysisIdsForImport();
//	QList<QUuid> finalAnalysisIdList = analysisSelector_->getSelectedAnalysisIds();	//The import analysis ids will change when we import them to new analyses in the session file.
//	for(int i=0;i<designRoot_->getNumAnalyses();i++)
//	{
//		//If this analysis is in our import list
//		if(importAnalysisIds.contains(designRoot_->getAnalysis(i).staticCast<Analysis>()->getAssociateId()))
//		{
//			//Try to import it.
//			QSharedPointer<AssetExportImport> exportImport(new AssetExportImport());
//			QString exportText = exportImport->exportToText((QList<QSharedPointer<Asset>>() << designRoot_->getExperiment())
//				,dynamic_cast<AssociateRootHost*>(designRoot_->getAnalysis(i).data()));
//			if(exportText.isEmpty())
//			{
//				QMessageBox::warning(0,"Could not import Analysis",exportImport->getLatestMessage());
//				return false;
//			}
//
//			//Create a new analysis to put the exported analysis in.
//			QSharedPointer<Analysis> newAnalysis = playbackController_->getDesignRoot()->importAnalysis("<Analysis/>").staticCast<Analysis>();
//			newAnalysis->setName(designRoot_->getAnalysis(i)->getName());
//			QUuid newAnalysisId = newAnalysis->getAssociateId();
//			bool linkResult = newAnalysis->LinkToAsset(playbackController_->getDesignRoot()->getExperiment(),QString());
//			Q_ASSERT(linkResult);
//			//Create UI Data for the new Analysis and attach it
//			AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(newAnalysis.data());
//			Q_ASSERT(assocRootHost);
//			QUuid hostId = assocRootHost->getHostId();
//			QSharedPointer<Asset> newUIData = newAnalysis->getParentAsset().staticCast<DataStore>()->createChildAsset("UIData",QString(),QString());
//			QString feedback;
//			newUIData.staticCast<AssociateRoot>()->LinkToAsset(newAnalysis,feedback);
//			Q_ASSERT(newAnalysis);
//
//			//Lets try to import the analysis contents into our new analysis from the exportText.
//			int result = exportImport->importFromText(playbackController_->getDesignRoot()->getExperiment()
//													,exportText
//													,dynamic_cast<AssociateRootHost*>(playbackController_->getDesignRoot()->getExperiment().data())
//													,QPoint(0,0)
//													,dynamic_cast<AssociateRootHost*>(newAnalysis.data()));
//			switch(result)
//			{
//			case AssetExportImport::IMPORT_SUCCEEDED:
//				break;
//			//case AssetExportImport::IMPORT_SUCCEEDED_WITH_WARNINGS:
//			default:
//				//Import failed, remove analysis from design
//				playbackController_->getDesignRoot()->getExperiment().staticCast<DataStore>()->ClearAssociateDescendants(newAnalysisId);
//	
//				int analysisIndexToRemove = playbackController_->getDesignRoot()->getNumAnalyses()-1;
//				if(!playbackController_->getDesignRoot()->removeAnalysis(analysisIndexToRemove))
//					Q_ASSERT(false);
//				QMessageBox::warning(0,"Could not import Analysis",exportImport->getLatestMessage());
//				return false;
//			}
//			finalAnalysisIdList.append(newAnalysisId);
//		}
//	}
//
//	//If we got here, all import analyses were imported.  Enabled them.
//	playbackController_->setEnabledAnalyses(finalAnalysisIdList);
//	return true;
//}

void ReplayViewer::prepareForSessionLoad()
{
	runs_->setEnabled(false);
	loadProgress_->setMaximum(0);
	pauseAction_->setEnabled(false);
	stopAction_->setEnabled(false);
	playAction_->setEnabled(false);
	runToEndAction_->setEnabled(false);
	toggleRecord_->setEnabled(false);
	loadSessionAction_->setEnabled(false);
	foreach(QWidget * outSigWidg, outputSignalsWidgets_)
	{
		static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
	}
}

void ReplayViewer::runNormal()
{
	useRunToEnd_ = false;
	needsAutoSave_ = false;
	play();
}


void ReplayViewer::runToEnd()
{
	needsAutoSave_ = false;
	if(!paused_)
	{
		QList<QUuid> importAnalysisIds = analysisSelector_->getSelectedAnalysisIdsForImport();
		QList<QUuid> finalAnalysisIdList = analysisSelector_->getSelectedAnalysisIds();	//The import analysis ids will change when we import them to new analyses in the session file.
		if(importAnalysisIds.size() || finalAnalysisIdList.size())
		{
			QMessageBox::StandardButton result = QMessageBox::question(this,"Save analysis results","Would you like to Picto to automatically save analysis results when this run is complete?",QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,QMessageBox::StandardButton::Yes);
			if(result == QMessageBox::StandardButton::Yes)
			{
				QString dirName = ".";
				bool useSeperateSubDirs = true;
				SaveOutputDialog saveDialog(this,dirName,useSeperateSubDirs);
				saveDialog.showDialog();
				dirName = saveDialog.getSelectedDir();
				if(dirName.isEmpty())
					return;
				useSeperateSubDirs = saveDialog.useSeperateSubDirs();
				outputWidgetHolder_->setSaveParameters(dirName,useSeperateSubDirs);
				needsAutoSave_ = true;
			}
		}
	}
	useRunToEnd_ = true;
	if(!play())
	{
		return;
	}
}

bool ReplayViewer::play()
{
	qDebug()<<"Play slot";
	if(!paused_)
	{
		if(runQueue_.isEmpty())
		{
			QMessageBox::information(this,"Playback Complete","Playback of the selected runs is complete");
			runs_->resetAllRunStatus();
			runQueue_ = getSelectedPlayRunInfo();
			if	(	runQueue_.size() 
					&& (runQueue_.first().filePath_ != latestRun_.filePath_)
				)
			{
				loadProgress_->setValue(0);
			}
			return false;
		}
		PlayRunInfo latestPlayRunInfo = runQueue_.first();
		stop();
		playbackController_->loadSession(latestPlayRunInfo.filePath_);
		playbackController_->selectRun(latestPlayRunInfo.runIndex_);

		analysisSelector_->setEnabled(false);
		latestRun_ = runQueue_.first();
	}
	if(loadProgress_->value() == 0)	//If we haven't loaded the session data yet, put loadProgress into initial state
	{
		prepareForSessionLoad();
	}
	startingRun_ = true;
	calledPlayNotPause_ = true;
	playbackController_->play(getSelectedLocalAnalyses(),getAnalysesToImport());
	return true;
}

void ReplayViewer::pause()
{
	qDebug()<<"Pause slot";
	if(!playing_)
	{
		PlayRunInfo latestPlayRunInfo = runQueue_.first();
		stop();
		playbackController_->loadSession(latestPlayRunInfo.filePath_);
		playbackController_->selectRun(latestPlayRunInfo.runIndex_);
		analysisSelector_->setEnabled(false);
	}
	if(loadProgress_->value() == 0)	//If we haven't loaded the session data yet, put loadProgress into initial state
	{
		prepareForSessionLoad();
	}
	calledPlayNotPause_ = false;
	playbackController_->pause(getSelectedLocalAnalyses(),getAnalysesToImport());
}

void ReplayViewer::stop()
{
	qDebug()<<"Stop slot";
	playbackController_->stop();
}

QList<ReplayViewer::PlayRunInfo> ReplayViewer::getSelectedPlayRunInfo()
{
	QList<PlayRunInfo> newRunQueue;
	foreach(QString filePath,runs_->getSelectedFilePaths())
	{
		foreach(int runIndex,runs_->getSelectedRuns(filePath))
		{
			newRunQueue.append(PlayRunInfo(filePath,runIndex));
		}
	}
	return newRunQueue;
}

void ReplayViewer::playbackStatusChanged(int status)
{
	int numSelectedRuns = runs_->getNumSelectedRuns();
	bool runIsSelected = numSelectedRuns > 0;
	switch(status)
	{
	case PlaybackControllerData::Idle:
			progress_->setHighlightRange(0,0,0);
			progress_->setHighlightRange(1,0,0,true);
			progress_->setSliderProgress(0);
			runs_->setEnabled(false);
			pauseAction_->setEnabled(false);
			stopAction_->setEnabled(false);
			playAction_->setEnabled(false);
			runToEndAction_->setEnabled(false);
			toggleRecord_->setEnabled(false);
			playbackController_->getRenderingTarget()->showSplash();
			playing_ = false;
			paused_ = false;
		break;
		case PlaybackControllerData::Stopped:
			runs_->setEnabled(true);
			loadProgress_->setMaximum(100);
			progress_->setHighlightRange(0,0,0);
			progress_->setHighlightRange(1,0,0,true);
			progress_->setSliderProgress(0);
			pauseAction_->setEnabled(runIsSelected);
			stopAction_->setEnabled(false);
			playAction_->setEnabled(runIsSelected);
			runToEndAction_->setEnabled(runIsSelected);
			toggleRecord_->setEnabled(runIsSelected);
			loadSessionAction_->setEnabled(true);
			foreach(QWidget * outSigWidg, outputSignalsWidgets_)
			{
				static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
			}
			playbackController_->getVisualTarget()->clear();
			playbackController_->getRenderingTarget()->showSplash();
			analysisSelector_->setEnabled(true);
			playing_ = false;
			paused_ = false;
		break;
		case PlaybackControllerData::Running:
			runs_->setEnabled(true);
			loadProgress_->setMaximum(100);
			pauseAction_->setEnabled(true);
			stopAction_->setEnabled(true);
			playAction_->setEnabled(false);
			runToEndAction_->setEnabled(false);
			toggleRecord_->setEnabled(true);
			loadSessionAction_->setEnabled(true);
			foreach(QWidget * outSigWidg, outputSignalsWidgets_)
			{
				static_cast<OutputSignalWidget*>(outSigWidg)->enable(true);
			}
			playing_ = true;
			paused_ = false;
			Q_ASSERT(runQueue_.size());
			runs_->setRunStatus(runQueue_.first().filePath_,runQueue_.first().runIndex_,TaskSelectorWidget::INPROGRESS);	//Set currently playing run to green
		break;
		case PlaybackControllerData::Paused:
			runs_->setEnabled(true);
			loadProgress_->setMaximum(100);
			pauseAction_->setEnabled(false);
			stopAction_->setEnabled(true);
			playAction_->setEnabled(true);
			runToEndAction_->setEnabled(true);
			loadSessionAction_->setEnabled(true);
			playing_ = false;
			paused_ = true;
		break;
	}
	//Each time the playback status changes, update the Visual Target Host's record mode
	updateRecordingTarget();
	latestStatus_ = status;
}

void ReplayViewer::loadSession()
{
	QStringList filenames = QFileDialog::getOpenFileNames(	this,
															tr("Select sessions"),
															".",
															"Picto Sessions(*.sqlite)");
	if(!filenames.size())
		return;
	loadSessionAction_->setEnabled(false);
	needsAutoSave_ = false;
	useRunToEnd_ = false;
	startingRun_ = false;
	calledPlayNotPause_ = false;
	latestRun_ = PlayRunInfo();
	latestStatus_ = PlaybackControllerData::Idle;
	loadProgress_->setMaximum(0);
	loadProgress_->setValue(0);

	runQueue_.clear();

	runs_->clear();
	filenames.sort();
	foreach(QString filename,filenames)
	{
		//Load Sqlite Database
		filename = filename.replace("\\","/");
		Q_ASSERT(filename.lastIndexOf("/") >=0);
		Q_ASSERT(filename.lastIndexOf(".") >=0);
		FileSessionLoader loader;
		if(!loader.setFile(filename))
		{
			QMessageBox::warning(0,"Session could not be loaded","The session in file: " + filename.toLatin1() + " could not be loaded.");
		}
		//Set up task selector widget
		QStringList runs = loader.getRunNames();
		QStringList notes = loader.getRunNotes();
		QStringList savedRuns = loader.getSavedRunNames();
		for(int i=0;i<runs.size();i++)
		{
			runs_->addRun(savedRuns.contains(runs[i]),filename,runs[i],i,notes[i]);
		}
		//Set up analysis selector widget
		analysisSelector_->setLocalDesignRoot(filename,loader.getDesignRoot());
		//analysisSelector_->setDesignRootForImport(designRoot_);
	}
	analysisSelector_->setCurrentFile("");
	stop();
	paused_ = false;
}

void ReplayViewer::updateTime(double time)
{
	double runLength = playbackController_->getRunLength();
	if(runLength < 0)
		runLength = 1;
	progress_->setMaximum(runLength);
	if(startingRun_ && playing_)
	{
		if(useRunToEnd_)
			progress_->jumpToEnd();	//we do this here because the progress widget needs to know how long the run is before it can say how to jump to the end
		startingRun_ = false;
	}
	if(jumpDownRequested_)
	{
		if(time >= progress_->getHighlightMax(0))
			return;	//If a downward jump was requested, stop setting progress values
					//until the request starts being fulfilled.
		jumpDownRequested_ = false;
	}
	progress_->setHighlightMax(0,time);
	if(time > progress_->getSliderProgress())
		progress_->setSliderProgress(time);
}

void ReplayViewer::updateLoadTimes(double maxBehavioral,double)
{
	progress_->setHighlightMax(1,maxBehavioral);	//For now just deal with behavioral time
}

void ReplayViewer::runSelectionChanged()
{
	QList<PlayRunInfo> newRunQueue = getSelectedPlayRunInfo();

	if	(	runQueue_.size() 
			&& newRunQueue.size() 
			&& (newRunQueue.first().filePath_ != runQueue_.first().filePath_)
		)
	{
		loadProgress_->setValue(0);
	}

	if(!playing_ && !paused_)
	{
		pauseAction_->setEnabled(true);
		playAction_->setEnabled(true);
		runToEndAction_->setEnabled(true);
		toggleRecord_->setEnabled(true);
	}
	runQueue_ = newRunQueue;
	//Set up local analysis selector based on currently selected runs
	if(runQueue_.isEmpty())
		analysisSelector_->setCurrentFile("");
	else
	{
		//See if there are runs from more than one file selected.  If so, disable
		//local analyses
		QString filePath = runQueue_.first().filePath_;
		bool enableLocalAnalyses = true;
		foreach(PlayRunInfo runInfo,runQueue_)
		{
			if(runInfo.filePath_ != filePath)
			{
				enableLocalAnalyses = false;
				break;
			}
		}
		if(enableLocalAnalyses)
			analysisSelector_->setCurrentFile(filePath);
		else
			analysisSelector_->setCurrentFile("");
	}
	jumpDownRequested_ = false;
	playbackController_->stop();
	runs_->resetAllRunStatus();
	progress_->setSliderProgress(0);
	progress_->setHighlightRange(0,0,0);
	progress_->setHighlightRange(1,0,0);
}

void ReplayViewer::setUserType(int index)
{
	switch(index)
	{
	case 0:
		playbackController_->setUserToOperator();
		break;
	case 1:
		playbackController_->setUserToSubject();
		break;
	};
}

void ReplayViewer::percentLoaded(double percent)
{
	loadProgress_->setMaximum(100);
	loadProgress_->setValue(percent);
}

void ReplayViewer::jumpRequested(double time)
{
	if(time < progress_->getHighlightMax(0))
		jumpDownRequested_ = true;
	else
		jumpDownRequested_ = false;
	playbackController_->jumpToTime(time);
}

void ReplayViewer::userChoosingJump(bool starting)
{
	if(starting)
	{
		if(pauseAction_->isEnabled())
		{
			pausedFromJump_ = true;
			pauseAction_->trigger();
		}
	}
	else 
	{
		if(!pausedFromJump_)
			return;
		play();
		pausedFromJump_ = false;
	}
}

void ReplayViewer::toggleRecording()
{
	if(!visualTargetHost_)
		return;
	//Toggle record mode
	recordModeOn_ = !recordModeOn_;

	//Update the recording LED according to the new recording state.
	if(recordModeOn_)
		toggleRecord_->setIcon(QIcon(":/icons/red_led_on.svg"));
	else	
		toggleRecord_->setIcon(QIcon(":/icons/red_led_off.svg"));

	updateRecordingTarget();
}

void ReplayViewer::restartRecording()
{
	if(!visualTargetHost_)
		return;
	visualTargetHost_->restartRecording();
}

void ReplayViewer::saveRecording()
{
	QString filename = QFileDialog::getSaveFileName(this,
		tr("Save Video File"),".",QString("Video (*.%1)").arg(visualTargetHost_->getVideoFileType()));
	if(filename.isNull())
		return;
	if(!visualTargetHost_->saveRecordingAs(filename))
	{
		QMessageBox::warning(0,"Failed to Save Video","The recorded video could not be saved at "+filename.toLatin1()+".");
	}
}

//Updates the time recorded in the toolbar label
void ReplayViewer::setRecordTime(double time)
{
	if(recordTime_->value() == time)
		return;
	//If record time is positive, enable restart and saving, otherwise turn them off
	if(recordTime_ > 0)
	{
		restartRecord_->setEnabled(true);
		saveRecording_->setEnabled(true);
	}
	else
	{
		restartRecord_->setEnabled(false);
		saveRecording_->setEnabled(false);
	}
	recordTime_->display(QString::number(time,'f',3));
}

void ReplayViewer::designRootChanged()
{
	//In theory, we don't have to disconnect the old design config when a new one is set because it is done
	//automatically when that designConfig is deleted.
	connect(playbackController_->getDesignRoot()->getExperiment()->getDesignConfig().data(),SIGNAL(runStarted(QUuid)),this,SLOT(runStarted(QUuid)));
}

void ReplayViewer::loadError(QString errorStr)
{
	QMessageBox::critical(0,"Load Error",errorStr);
	loadProgress_->setMaximum(100);
	loadProgress_->setMinimum(0);
	loadProgress_->setValue(0);
	loadSessionAction_->setEnabled(true);
}

void ReplayViewer::runStarted(QUuid runId)
{
	outputWidgetHolder_->newRunStarted(runId);
}

void ReplayViewer::finishedPlayback()
{
	PlayRunInfo latestPlayRunInfo = runQueue_.first();
	runs_->setRunStatus(latestPlayRunInfo.filePath_,latestPlayRunInfo.runIndex_,TaskSelectorWidget::COMPLETE);
	if(needsAutoSave_)
	{
		outputWidgetHolder_->saveOutput();
	}
	runQueue_.pop_front();
	play();
}

void ReplayViewer::analysesImportedFailed(QString errorMsg)
{
	bool callPlayAgain = calledPlayNotPause_;
	PlayRunInfo latestPlayRunInfo = runQueue_.first();
	runQueue_.pop_front();
	QList<PlayRunInfo> selectedRuns = getSelectedPlayRunInfo();
	if(selectedRuns.size() == 1)	//This was the only selected run
	{
		QMessageBox::warning(0,"Could not import Analysis",errorMsg);
		runQueue_ = selectedRuns;
		callPlayAgain = false;
	}
	else
		runs_->setRunStatus(latestPlayRunInfo.filePath_,latestPlayRunInfo.runIndex_,TaskSelectorWidget::ERROROCCURED);
	analysisSelector_->setEnabled(true);
	if(callPlayAgain)
	{
		play();
	}
}