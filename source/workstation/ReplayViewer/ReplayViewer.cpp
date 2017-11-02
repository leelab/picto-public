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
#include "../../common/compositor/OutputSignalWidget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/iodevices/BufferFileGenerator.h"
#include "../../common/designer/propertyframe.h"
#include "../../common/designer/UIHelper.h"
#include "../../common/engine/XYSignalChannel.h"
#include "../../common/engine/MouseInputPort.h"
#include "../../common/parameter/OperatorClickParameter.h"
#include "../../common/playback/FileSessionLoader.h"

#include "../DataViewer/DataViewOrganizer.h"
#include "../DataViewer/DataViewWidget.h"
#include "../DataViewer/PlotViewWidget.h"


#include "../../common/memleakdetect.h"


ReplayViewer::ReplayViewer(QWidget *parent) :
Viewer(parent), selectedChannel_(0), selectedUnit_(0), noCallBack_(false), currentTime_(0), activatePlots_(false)
{
	//Create the Playback Controller that handles all Experiment playback
	playbackController_ = QSharedPointer<PlaybackController>(new PlaybackController());
	connect(playbackController_.data(), SIGNAL(statusChanged(int)), this, SLOT(playbackStatusChanged(int)));
	connect(playbackController_.data(), SIGNAL(taskChanged(QString)), this, SLOT(taskChanged(QString)));
		
	
	//Setup the user interface
	setupUi();
}

/*! \brief Called just before displaying the viewer
 */
void ReplayViewer::init()
{
	jumpDownRequested_ = false;
	QStringList importNames = playbackController_->precacheAnalysisNames(designRoot_);
	analysisSelector_->setDesignRootForImport(designRoot_, importNames);
	if (latestStatus_ == PlaybackControllerData::PreLoading || latestStatus_ == PlaybackControllerData::Stopped)
		analysisSelector_->enableCheckboxes(true);
	else
		analysisSelector_->enableCheckboxes(false);
}

/*! \brief Called just before hiding the viewer
 */
void ReplayViewer::deinit()
{
	stop();
	viewSelectionFrame_->clearPlotHandlers();
	
	//Plots	
	if (activatePlots_)
		viewSelectionFrame_->resetPlotsList(1);	
	enablePlots_->setCheckState(Qt::Unchecked);
	viewSelectionFrame_->updateUI(false);
	activatePlots_ = false;
	//end Plots
	emit deinitComplete();
}

//! Called when the application is about to quit.  Takes care of closing this windows resources
bool ReplayViewer::aboutToQuit()
{
	playbackController_->aboutToQuit();
	return true;
}

/*! \brief  Sets up the user interface portions of the GUI
 */
void ReplayViewer::setupUi()
{
	//Load Session Actions
	loadSessionAction_ = new QAction(tr("Open Session"),this);
	loadSessionAction_->setIcon(QIcon(":/icons/sessionopen.png"));
	connect(loadSessionAction_, SIGNAL(triggered()), this, SLOT(loadSession()));
	loadSessionAction_->setEnabled(true);

	runs_ = new RunSelectorWidget();
	connect(runs_,SIGNAL(runSelectionChanged()),this,SLOT(runSelectionChanged()));
	connect(playbackController_.data(),SIGNAL(sessionPreloaded(PreloadedSessionData)),this,SLOT(sessionPreloaded(PreloadedSessionData)));
	connect(playbackController_.data(),SIGNAL(sessionPreloadFailed(QString)),this,SLOT(preloadError(QString)));

	//Plots	
		connect(playbackController_.data(), SIGNAL(sessionLoaded(LoadedSessionData)), this, SLOT(sessionLoaded(LoadedSessionData)));
	//end Plots

	///play/pause/stop actions and toolbar
	playAction_ = new QAction(tr("&Start task"),this);
	playAction_->setIcon(QIcon(":/icons/play.png"));
	playAction_->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
	playAction_->setToolTip("Run (Ctrl+R)");
	connect(playAction_,SIGNAL(triggered()),this, SLOT(runNormal()));
	latestStatus_ = PlaybackControllerData::Idle;
	useRunToEnd_ = false;
	startingRun_ = false;
	calledPlayNotPause_ = false;

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

	lfpRequirements_ = new QComboBox();
	lfpRequirements_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	lfpRequirements_->setToolTip("Set LFP requirements");
	lfpRequirements_->addItem("Load LFP",0);
	lfpRequirements_->addItem("Ignore LFP",1);
	connect(lfpRequirements_,SIGNAL(currentIndexChanged(int)),this,SLOT(setLFPRequirements(int)));

	progress_ = new ProgressWidget();
	progress_->setMaximum(1);
	progress_->setHighlightColor(0,QColor("#88f"));
	progress_->setHighlightColor(1,QColor("#f88"));
	connect(playbackController_.data(),SIGNAL(timeChanged(double)),this,SLOT(updateTime(double)));
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

	//Indicates if the toggleRecord button is set for record mode or not
	recordModeOn_ = false;	

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

	channelBox_ = new QComboBox();
	channelBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	channelBox_->setToolTip("Select a Channel");
	channelBox_->addItem("All Channels", 0);
	channelBox_->setCurrentIndex(0);
	connect(channelBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedChannelChanged()));

	unitBox_ = new QComboBox(this);
	unitBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	unitBox_->setToolTip("Select a Unit");
	unitBox_->addItem("All Units", 0);
	unitBox_->setCurrentIndex(0);
	connect(unitBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedUnitChanged()));

//Plots
	enablePlots_ = new QCheckBox("Plots",this);
	enablePlots_->setCheckState(Qt::Unchecked);
	connect(enablePlots_, SIGNAL(clicked(bool)), this, SLOT(checkClicked(bool)));
//end Plots

	testToolbar_ = new QToolBar(this);
	testToolbar_->addAction(loadSessionAction_);
	testToolbar_->addSeparator();
	testToolbar_->addAction(playAction_);
	testToolbar_->addAction(runToEndAction_);
	testToolbar_->addAction(pauseAction_);
	testToolbar_->addAction(stopAction_);
	testToolbar_->addSeparator();	
	testToolbar_->addWidget(enablePlots_);
	testToolbar_->addSeparator();
	testToolbar_->addWidget(userType_);
	testToolbar_->addWidget(lfpRequirements_);
	testToolbar_->addSeparator();
	testToolbar_->addAction(toggleRecord_);
	testToolbar_->addWidget(recordTime_);
	testToolbar_->addAction(restartRecord_);
	testToolbar_->addAction(saveRecording_);
	testToolbar_->addSeparator();
	testToolbar_->addWidget(channelBox_);
	testToolbar_->addWidget(unitBox_);	
	testToolbar_->addSeparator();

	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(testToolbar_);
	toolbarLayout->addStretch();

	QVBoxLayout *stimulusLayout = new QVBoxLayout;
	
	dataViewOrganizer_ = new DataViewOrganizer();
	

	//Set up the visual target host
	//This exists because QSharedPointer<QWidget> results in multiple delete call, which gives us memory exceptions.
	visualTargetHost_ = new RecordingVisualTargetHost();
	visualTargetHost_->setVisualTarget(playbackController_->getVisualTarget());

	stimulusLayout->addWidget(dataViewOrganizer_);

	//connect(playbackController_.data(), SIGNAL(framePresented(double)), visualTargetHost_, SLOT(updateFrameTime(double)));
	connect(visualTargetHost_, SIGNAL(updateRecordingTime(double)), this, SLOT(setRecordTime(double)));

	//Setup Output Signal Widgets
	foreach(QSharedPointer<Picto::VirtualOutputSignalController> cont
		,playbackController_->getOutputSignalControllers())
	{
		cont->moveToThread(QApplication::instance()->thread());
		outputSignalsWidgets_.push_back(new OutputSignalWidget(cont));
		stimulusLayout->addWidget(outputSignalsWidgets_.back());
	}
	stimulusLayout->addWidget(progress_);
	stimulusLayout->setMargin(0);
	stimulusLayout->setContentsMargins(0, 0, 0, 0);

	//Setup analysis widgets
	QTabWidget* analysisTabs = new QTabWidget();

	analysisSelector_ = new Picto::AnalysisSelectorWidget();
	analysisSelector_->setEnabled(false);
	connect(analysisSelector_, SIGNAL(analysisWidgetChanged()), this, SLOT(analysisWidgetChanged()));
	connect(playbackController_.data(),SIGNAL(designRootChanged()),this,SLOT(designRootChanged()));
	outputWidgetHolder_ = new OutputWidgetHolder();
	analysisTabs->addTab(runs_,"Select Runs");
	analysisTabs->addTab(analysisSelector_,"Select Analyses");
	analysisTabs->addTab(outputWidgetHolder_,"Analysis Output");


	viewSelectionFrame_ = new ViewSelectionFrame(true);	
	viewSelectionFrame_->setVisualTargetHost(visualTargetHost_);
	viewSelectionFrame_->connectToViewerLayout(dataViewOrganizer_);
	viewSelectionFrame_->setStyleSheet("ViewSelectionFrame { border: 1px solid gray }");
	viewSelectionFrame_->rebuild();

	connect(analysisSelector_, SIGNAL(notifyAnalysisSelection(const QString&, bool, bool)),
		viewSelectionFrame_, SLOT(notifyAnalysisSelection(const QString&, bool, bool)));

	connect(analysisSelector_, SIGNAL(clearAnalysisSelection(bool)),
		viewSelectionFrame_, SLOT(clearAnalysisSelection(bool)));

	QHBoxLayout *viewLayout = new QHBoxLayout();
	viewLayout->setMargin(0);
	viewLayout->setContentsMargins(0, 0, 0, 0);
	viewLayout->addWidget(viewSelectionFrame_, 0, Qt::AlignCenter);

	QVBoxLayout *infoLayout = new QVBoxLayout();
	infoLayout->addLayout(viewLayout);
	infoLayout->addWidget(analysisTabs);
	infoLayout->setContentsMargins(0, 11, 0, 0);
	infoLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	infoLayout->setAlignment(Qt::AlignTop);

	QHBoxLayout *progressLayout = new QHBoxLayout();
	progressLayout->addWidget(new QLabel("Load Progress:"));
	progressLayout->addWidget(loadProgress_);
	infoLayout->addLayout(progressLayout);

	QWidget *container = nullptr;
	QSplitter *operationLayout = new QSplitter(Qt::Horizontal);

	container = new QWidget(operationLayout);
	container->setLayout(infoLayout);
	operationLayout->addWidget(container);
	operationLayout->setStretchFactor(0, 5);

	//Plots - keep even when no plots (tabs)	
	viewTabs_ = new QTabWidget();
	viewTabs_->setTabsClosable(true);
	QWidget *tabStimulusWidget = new QWidget();
	tabStimulusWidget->setLayout(stimulusLayout);
	viewTabs_->addTab(tabStimulusWidget, "Task");
	viewTabs_->setCurrentIndex(0);
	operationLayout->addWidget(viewTabs_);
	
	connect(viewSelectionFrame_, SIGNAL(openInNewTab(QWidget*)), this, SLOT(openInNewTab(QWidget*)));
	connect(viewTabs_, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	//end Plots

	operationLayout->setStretchFactor(1, 10);
	Picto::UIHelper::addSplitterLine(operationLayout->handle(1), 200);

	operationLayout->addWidget(speed_);
	operationLayout->setStretchFactor(2, 0);
	Picto::UIHelper::addSplitterLine(operationLayout->handle(2), 200);

	operationLayout->setHandleWidth(11);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout,0);
	mainLayout->addWidget(operationLayout,1);
	setLayout(mainLayout);
}
void ReplayViewer::selectedChannelChanged()
{
	if (noCallBack_)
		return;

	if (selectedChannel_ < 0)
		selectedChannel_ = 0;

	if (selectedUnit_ < 0)
		selectedUnit_ = 0;

	QString selectedStr = channelBox_->itemData(channelBox_->currentIndex()).toString(); //Channel 5
	if (selectedStr.split(QRegExp("\\s")).last().toInt())
		selectedChannel_ = selectedStr.split(QRegExp("\\s")).last().toInt(); //5

	//selectedChannel_ = channelBox_->itemData(channelBox_->currentIndex()).toInt();

	unitBox_->clear();
	selectedUnit_ = 0;

	if (currentTaskConfig_)
		currentTaskConfig_->setSelectedNeural(selectedChannel_, selectedUnit_);

	QList<int> unitList = ChannelsUnits_.value(selectedChannel_);
	unitBox_->addItem("All Units", 0);
	foreach(int unit, unitList)
	{
		unitBox_->addItem(QString("Unit %1").arg(unit), unit);
	}

	//This will take care of resetting the spike plot for the case where channel changes but unit index doesn't change.
	selectedUnitChanged();
}
void ReplayViewer::selectedUnitChanged()
{
	if (noCallBack_)
		return;

	if (selectedChannel_ < 0)
		selectedChannel_ = 0;

	if (selectedUnit_ < 0)
		selectedUnit_ = 0;


	//selectedUnit_ = unitBox_->itemData(unitBox_->currentIndex()).toInt();
	QString selectedStr = unitBox_->itemData(unitBox_->currentIndex()).toString();
	if (selectedStr.split(QRegExp("\\s")).last().toInt())
		selectedUnit_ = selectedStr.split(QRegExp("\\s")).last().toInt();
	
	if (currentTaskConfig_)
		currentTaskConfig_->setSelectedNeural(selectedChannel_, selectedUnit_);
}
/*! \brief Updates the Recording Visual Target Host to record or not depending on the current run and record mode states.
 */
void ReplayViewer::updateRecordingTarget()
{
	if(recordModeOn_ && (latestStatus_ == PlaybackControllerData::Running))
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

/*! \brief Returns the Uuids of all selected Analyses that are included in the loaded Session file's design.
 */
QList<QUuid> ReplayViewer::getSelectedLocalAnalyses()
{
	return analysisSelector_->getSelectedAnalysisIds();
}

/*! \brief Returns the exported designs of all selected Analyses that should be imported into the Session file's design.
 */
QStringList ReplayViewer::getAnalysesToImport()
{
	QList<QUuid> importAnalysisIds = analysisSelector_->getSelectedAnalysisIdsForImport();
	QStringList resultList;
	for (int i = 0; i<designRoot_->getNumAnalyses(); i++)
	{
		//If this analysis is in our import list
		if (importAnalysisIds.contains(designRoot_->getAnalysis(i).staticCast<Analysis>()->getAssociateId()))
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
/*! \brief Removes the first Run in the Run queue that we are iterating through (to analyze all selected Runs).
 *	\details If we end up emptying the Run queue and some of the Runs could not be properly analyzed, we ask the user if
 *	they would like to re-analyze the failed Runs.  Otherwise we just tell the user that all Runs have been succesfully
 *	analyzed.
 *	
 *	Returns false if the runQueue ended up being emptied and reinitialized to contain all selected Runs.  Whenever we finish
 *	the runQueue, if we don't change the selection to the runs that failed, we always reinitialize the runQueue to contain
 *	all selected runs so that the user can just push play to start Analysis again.
 */
bool ReplayViewer::popRunQueueFront()
{
	bool returnValue = true;
	runQueue_.pop_front();
	if(runQueue_.isEmpty())
	{
		//Playback is complete.
		QList<PlayRunInfo> selectedRuns = getSelectedPlayRunInfo();
		//Check if any of the run playbacks failed for the currently selected runs
		QList<PlayRunInfo> failedRuns;
		foreach(PlayRunInfo runInfo,selectedRuns)
		{
			if(runs_->getRunStatus(runInfo.filePath_,runInfo.runIndex_) == RunSelectorWidget::ERROROCCURED)
			{
				failedRuns.append(runInfo);
			}
		}
		//If a playback failed
		if(failedRuns.size())
		{
			//Tell the user we're done and offer to re-run failed playbacks.
			QMessageBox::StandardButton result = QMessageBox::question(this,"Playback Complete with Errors","Playback is complete; however, some runs could not successfully be played.  This is most likely due to limited system memory.  Try closing other applications and then select \"Retry\" to retry playback on the failed runs only.  If playback still doesn't work, try restarting Picto.",QMessageBox::StandardButton::Retry | QMessageBox::StandardButton::Cancel,QMessageBox::StandardButton::Cancel);
			//If the user wants to run failed playbacks again
			if(result == QMessageBox::Retry)
			{
				//Update the runs_ dialog and selectedRuns list, then set return value to true (for continue).
				runs_->deselectAll();
				foreach(PlayRunInfo runInfo,failedRuns)
				{
					runs_->selectRun(runInfo.filePath_,runInfo.runIndex_);
				}
				selectedRuns = getSelectedPlayRunInfo();
				returnValue = true;
			}
			else
			{
				//Don't continue
				returnValue = false;
			}
		}
		else
		{
			//All playbacks were succesfull.  Tell the user
			QMessageBox::information(this,"Playback Complete","All selected runs completed successfully");
			returnValue = false;
		}
		//Update the runQueue_ to contain all the selected runs.
		setRunQueue(selectedRuns);
	}
	return returnValue;
}

/*! \brief Sets the stored Run queue (used to playback/analyze batched Runs one after another) to the input value.
 */
void ReplayViewer::setRunQueue(QList<PlayRunInfo> newRunQueue)
{
	//Update the runQueue_ to contain all the input runs.
	runQueue_ = newRunQueue;
	//If we haven't already loaded the session for the first run in the list, set load progress back to zero.
	if	(	runQueue_.size() 
			&& (runQueue_.first().filePath_ != playbackController_->getLoadedFilePath())
		)
	{
		loadProgress_->setValue(0);
	}
}

/*! \brief Causes playback/analysis to begin in a normal way.  The user will be able to pause playback, fast forward, etc.
 *	\sa runToEnd()
 */
void ReplayViewer::runNormal()
{
	useRunToEnd_ = false;
	needsAutoSave_ = false;
	runs_->resetAllRunStatus();
	play();
}

/*! \brief Causes playback/analysis to run completely from the beginning to the end of each selected run at
 *	maximum speed, no rendering, and without any opportunity for user interaction during the course of playback.
 *	\details This is meant to be used when only Analysis is needed, and it is not important to be able to watch
 *	the Session on screen.  When this function is called and Analysis objects are active, this function asks
 *	if the user would like to automatically save Analysis outputs when each Run is complete.  This is important
 *	because otherwise in batch analysis each new Run Analysis would overwrite previous Analysis outputs and the 
 *	user would only be able to save Analysis output of the last Run in the batch.
 */
void ReplayViewer::runToEnd()
{
	needsAutoSave_ = false;
	if(latestStatus_ != PlaybackControllerData::Paused)
	{
		QList<QUuid> importAnalysisIds = analysisSelector_->getSelectedAnalysisIdsForImport();
		//The import analysis ids will change when we import them to new analyses in the session file.
		QList<QUuid> finalAnalysisIdList = analysisSelector_->getSelectedAnalysisIds();
		if(importAnalysisIds.size() || finalAnalysisIdList.size())
		{
			QMessageBox::StandardButton result = QMessageBox::question(this,"Save analysis results","Would you like to Picto to automatically save analysis results when this run is complete?",QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No | QMessageBox::StandardButton::Cancel,QMessageBox::StandardButton::Yes);
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
			else if(result != QMessageBox::StandardButton::No)
			{
				return;
			}
		}
	}
	useRunToEnd_ = true;
	runs_->resetAllRunStatus();
	if(!play())
	{
		return;
	}
}

/*! \brief Causes playback to begin or resume.  If the current Run's Session has not yet been loaded, it will first load
 *	and then the Run will begin playback.
 */
bool ReplayViewer::play()
{
	qDebug()<<"Play slot";
	if((latestStatus_ != PlaybackControllerData::Paused))
	{
		Q_ASSERT(!runQueue_.isEmpty());
		PlayRunInfo latestPlayRunInfo = runQueue_.first();
		playbackController_->selectFile(latestPlayRunInfo.filePath_);
		playbackController_->selectRun(latestPlayRunInfo.runIndex_);
	}
	startingRun_ = true;
	calledPlayNotPause_ = true;
	playbackController_->play(getSelectedLocalAnalyses(),getAnalysesToImport());
	return true;
}

/*! \brief Pauses the current Run, or starts the selected Run in a paused state (at time zero).
 *	\details If the function causes a Run to start, and that Run's Session has not yet been loaded, this will cause the
 *	Session to load and then the "start by pausing" will be enacted.
 */
void ReplayViewer::pause()
{
	qDebug()<<"Pause slot";
	if(latestStatus_ != PlaybackControllerData::Running)
	{
		PlayRunInfo latestPlayRunInfo = runQueue_.first();
		stop();
		playbackController_->selectFile(latestPlayRunInfo.filePath_);
		playbackController_->selectRun(latestPlayRunInfo.runIndex_);
	}
	calledPlayNotPause_ = false;
	playbackController_->pause(getSelectedLocalAnalyses(),getAnalysesToImport());
}

/*! \brief Stops playback/analysis of the current Run and reinitializes everything for the next playback.
 */
void ReplayViewer::stop()
{
	qDebug()<<"Stop slot";
	//Whenever stop is pressed, the runQueue_ should be reinitialized to everything that's in the run selector
	QList<PlayRunInfo> selectedRuns = getSelectedPlayRunInfo();
	setRunQueue(selectedRuns);
	//Plots
	if (activatePlots_)
		viewSelectionFrame_->resetPlotsList(1);
	//end Plots
	playbackController_->stop();
}

/*! \brief Returns a list of PlayRunInfo structs including information about all of the currently selected Runs from the
 *	RunSelectorWidget.
 */
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

/*! \brief Called whenever the PlaybackController's State changes.  status is the new State.
 *	\details This function updates the ReplayViewer UI to reflect the current PlaybackController state.  We update the UI as
 *	a result of PlaybackController state changes rather than updatingit when actions are triggered in the ReplayViewer
 *	because the PlaybackController state machineruns in a separate thread, so there may be some lag between when a
 *	ReplayViewer action is triggered (ie. Play) and when the action actually takes affect
 *	(ie. PlaybackControllerData::Running state).
 *	\note Since this object is set as running in the UI thread, signals from the PlaybackController thread don't actually
 *	take effect until the UI thread's event loop runs.  That is how this how this function can be triggered by changes that
 *	occur in the PlaybackController thread without being in the thread itself.
 */
void ReplayViewer::playbackStatusChanged(int status)
{
	bool runIsSelected = runs_->getNumSelectedRuns();
	bool analysisIsSelected = analysisSelector_->hasSelectedIds();
	switch(status)
	{
	case PlaybackControllerData::Idle:
			progress_->setHighlightRange(0,0,0);
			progress_->setHighlightRange(1,0,0,true);
			progress_->setSliderProgress(0);
			progress_->setEnabled(false);
			runs_->enableCheckBoxes(false);
			runs_->setEnabled(false);
			pauseAction_->setEnabled(false);
			stopAction_->setEnabled(false);
			playAction_->setEnabled(false);
			runToEndAction_->setEnabled(false);
			toggleRecord_->setEnabled(false);
			lfpRequirements_->setEnabled(false);
			analysisSelector_->setEnabled(false);
			analysisSelector_->enableCheckboxes(false);
			enablePlots_->setEnabled(true);
			foreach(QWidget * outSigWidg, outputSignalsWidgets_)
			{
				static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
			}
			playbackController_->getRenderingTarget()->showSplash();
		break;
		case PlaybackControllerData::PreLoading:
			progress_->setHighlightRange(0,0,0);
			progress_->setHighlightRange(1,0,0,true);
			progress_->setSliderProgress(0);
			progress_->setEnabled(false);
			runs_->enableCheckBoxes(false);
			runs_->setEnabled(false);
			pauseAction_->setEnabled(false);
			stopAction_->setEnabled(false);
			playAction_->setEnabled(false);
			runToEndAction_->setEnabled(false);
			toggleRecord_->setEnabled(false);
			loadProgress_->setMaximum(0);
			loadSessionAction_->setEnabled(false);
			lfpRequirements_->setEnabled(false);
			analysisSelector_->setEnabled(true);
			analysisSelector_->enableCheckboxes(true);
			enablePlots_->setEnabled(true);
			foreach(QWidget * outSigWidg, outputSignalsWidgets_)
			{
				static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
			}
			playbackController_->getRenderingTarget()->showSplash();
		break;
		case PlaybackControllerData::Stopped:
			runs_->enableCheckBoxes(true);
			runs_->setEnabled(true);
			loadProgress_->setMaximum(100);
			progress_->setHighlightRange(0,0,0);
			progress_->setHighlightRange(1,0,0,true);
			progress_->setSliderProgress(0);
			progress_->setEnabled(false);
			pauseAction_->setEnabled(runIsSelected);
			stopAction_->setEnabled(false);
			playAction_->setEnabled(runIsSelected);
			runToEndAction_->setEnabled(runIsSelected && analysisIsSelected);
			toggleRecord_->setEnabled(runIsSelected);
			loadSessionAction_->setEnabled(true);
			lfpRequirements_->setEnabled(true);
			foreach(QWidget * outSigWidg, outputSignalsWidgets_)
			{
				static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
			}
			playbackController_->getVisualTarget()->clear();
			playbackController_->getRenderingTarget()->showSplash();
			analysisSelector_->setEnabled(true);
			analysisSelector_->enableCheckboxes(true);
			enablePlots_->setEnabled(true);
		break;
		case PlaybackControllerData::Loading:
			progress_->setHighlightRange(0,0,0);
			progress_->setHighlightRange(1,0,0,true);
			progress_->setSliderProgress(0);
			progress_->setEnabled(false);
			runs_->enableCheckBoxes(false);
			runs_->setEnabled(true);
			pauseAction_->setEnabled(false);
			stopAction_->setEnabled(false);
			playAction_->setEnabled(false);
			runToEndAction_->setEnabled(false);
			toggleRecord_->setEnabled(false);
			loadSessionAction_->setEnabled(false);
			lfpRequirements_->setEnabled(false);
			analysisSelector_->setEnabled(true);
			analysisSelector_->enableCheckboxes(false);
			enablePlots_->setEnabled(false);
			loadProgress_->setMaximum(0);
			foreach(QWidget * outSigWidg, outputSignalsWidgets_)
			{
				static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
			}
			playbackController_->getRenderingTarget()->showSplash();
		break;
		case PlaybackControllerData::Running:
			runs_->enableCheckBoxes(true);
			runs_->setEnabled(true);
			loadProgress_->setMaximum(100);
			loadProgress_->setValue(100);
			pauseAction_->setEnabled(true);
			stopAction_->setEnabled(true);
			playAction_->setEnabled(false);
			progress_->setEnabled(true);
			runToEndAction_->setEnabled(false);
			toggleRecord_->setEnabled(true);
			loadSessionAction_->setEnabled(true);
			lfpRequirements_->setEnabled(false);
			analysisSelector_->setEnabled(true);
			analysisSelector_->enableCheckboxes(false);
			enablePlots_->setEnabled(false);
			foreach(QWidget * outSigWidg, outputSignalsWidgets_)
			{
				static_cast<OutputSignalWidget*>(outSigWidg)->enable(true);
			}
			Q_ASSERT(runQueue_.size());
			runs_->setRunStatus(runQueue_.first().filePath_,runQueue_.first().runIndex_,RunSelectorWidget::INPROGRESS);	//Set currently playing run to green
		break;
		case PlaybackControllerData::Paused:
			runs_->enableCheckBoxes(true);
			runs_->setEnabled(true);
			loadProgress_->setMaximum(100);
			loadProgress_->setValue(100);
			pauseAction_->setEnabled(false);
			stopAction_->setEnabled(true);
			playAction_->setEnabled(true);
			progress_->setEnabled(true);
			runToEndAction_->setEnabled(analysisIsSelected);
			toggleRecord_->setEnabled(true);
			loadSessionAction_->setEnabled(true);
			lfpRequirements_->setEnabled(false);
			analysisSelector_->setEnabled(true);
			analysisSelector_->enableCheckboxes(false);
			enablePlots_->setEnabled(false);
			foreach(QWidget * outSigWidg, outputSignalsWidgets_)
			{
				static_cast<OutputSignalWidget*>(outSigWidg)->enable(true);
			}
			//Set currently playing run to green
			runs_->setRunStatus(runQueue_.first().filePath_, runQueue_.first().runIndex_, RunSelectorWidget::INPROGRESS);
			break;
	}
	//Each time the playback status changes, update the Visual Target Host's record mode
	updateRecordingTarget();
	latestStatus_ = status;
}

/*! \brief Triggred when the user presses the load session button.  Opens a File dialog, gets the user's selected Sessions,
 *	updates all the UI for new Session info, and then tells the PlaybackController to preload the Session files
 *	(PlaybackController::preLoadSessions()).
 */
void ReplayViewer::loadSession()
{
	QStringList filenames = QFileDialog::getOpenFileNames(	this,
															tr("Select sessions"),
															".",
															"Picto Sessions(*.sqlite)");
	if(!filenames.size())
		return;
	stop();
	loadSessionAction_->setEnabled(false);
	needsAutoSave_ = false;
	useRunToEnd_ = false;
	startingRun_ = false;
	calledPlayNotPause_ = false;
	latestStatus_ = PlaybackControllerData::Idle;
	loadProgress_->setMaximum(0);
	loadProgress_->setValue(0);
	runQueue_.clear();
	runs_->clear();
	filenames.sort();
	QStringList sessionFiles;
	foreach(QString filename,filenames)
	{
		//Load Sqlite Database
		filename = filename.replace("\\","/");
		Q_ASSERT(filename.lastIndexOf("/") >=0);
		Q_ASSERT(filename.lastIndexOf(".") >=0);
		sessionFiles.append(filename);
	}
	analysisSelector_->setCurrentFile("");
	playbackController_->preLoadSessions(sessionFiles);
}

/*! \brief Called when playback time changes in the PlaybackController.  Updates the position of the ProgressWidget slider
 *	accordingly.
 *	\details This function includes a bunch of logic that deals with cases where the slider was used to change something in
 *	the PlaybackController.  For example, if the slider was used to tell the PlaybackController to fast forward to a time
 *	100 seconds after its current position, we shouldn't move the slider back as soon as that command starts taking affect,
 *	instead, while the fast forward is taking affect, the slider remains stationary, only restarting once the fastforward
 *	reaches its time.  Similar logic is in place if the slider is moved to a position below the current PlaybackController
 *	time, in which case playback restarts from zero and fast forwards to the new slider time.
 */
void ReplayViewer::updateTime(double time)
{
	double runLength = playbackController_->getRunLength();
	if(runLength < 0)
		runLength = 1;
	progress_->setMaximum(runLength);
	if(startingRun_ && (latestStatus_ == PlaybackControllerData::Running))
	{
		if (useRunToEnd_)
		{
			//we do this here because the progress widget needs to know how long the run is before it can say how to
			//jump to the end
			progress_->jumpToEnd();
		}
		startingRun_ = false;
	}
	if(jumpDownRequested_)
	{
		if (time >= progress_->getHighlightMax(0))
		{
			return;
			//If a downward jump was requested, stop setting progress values until the request starts being fulfilled.
		}
		jumpDownRequested_ = false;
	}
	progress_->setHighlightMax(0,time);
	if(time > progress_->getSliderProgress())
		progress_->setSliderProgress(time);
}

/*! \brief Called whenever the selected Runs change in the RunSelectorWidget.  Updates the Run queue for batch
 *	playback/analysis along with other parts of the UI.
 *	\details If the change causes Runs to be selected from more than one Session, local Analyses are disabled.  For multiple
 *	Sessions, only imported Analyses are allowed.  Other UI channges need to be made as well depending on things like
 *	whether the first run in the queue has changed, etc.
 */
void ReplayViewer::runSelectionChanged()
{
	QList<PlayRunInfo> newRunQueue = getSelectedPlayRunInfo();

	//If the first run in the new run queue isn't part of the same file that we last loaded, set the load progress to zero
	if	(	runQueue_.size() 
			&& newRunQueue.size() 
			&& (newRunQueue.first().filePath_ != runQueue_.first().filePath_)
		)
	{
		loadProgress_->setValue(0);
	}

	//If we're not currently running
	if(latestStatus_ == PlaybackControllerData::Stopped)
	{
		//If nothing is selected, disable the commands that let you start playback
		if(newRunQueue.isEmpty())
		{
			pauseAction_->setEnabled(false);
			playAction_->setEnabled(false);
			runToEndAction_->setEnabled(false);
			toggleRecord_->setEnabled(false);
		}
		else
		{	//Otherwise enable them
			pauseAction_->setEnabled(true);
			playAction_->setEnabled(true);
			runToEndAction_->setEnabled(analysisSelector_->hasSelectedIds());
			toggleRecord_->setEnabled(true);
		}
	}
	runQueue_ = newRunQueue;
	//Set up local analysis selector based on currently selected runs
	if(runQueue_.isEmpty())
		analysisSelector_->setCurrentFile("");
	else
	{
		//See if there are runs from more than one file selected.  If so, disable local analyses
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
	stopAction_->trigger();
	progress_->setSliderProgress(0);
	progress_->setHighlightRange(0,0,0);
	progress_->setHighlightRange(1,0,0);
}

/*! \brief Called when the AnalysisSelectorWidget selection changes.  Enables/disables the "run to end" button depending on
 *	whether any Analyses are selected.
 */
void ReplayViewer::analysisWidgetChanged()
{
	if(latestStatus_ != PlaybackControllerData::Stopped)
		return;
	QList<PlayRunInfo> selectedRuns = getSelectedPlayRunInfo();
	runToEndAction_->setEnabled(analysisSelector_->hasSelectedIds() && selectedRuns.size());
}

/*! \brief Called when the User Type dropdown value changes.  Changes the user type in the PlaybackController accordingly.
 *	\note The active user, Operator or Subject, determines which graphics are shown in the display depending on each
 *	graphics OperatorView/SubjectView Properties.
 */
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

/*! \brief Called when the LFP enable dropdown value changes.  Changes whether or not LFP data will be loaded during
 *	playback for the purpose of runtime optimization.
 */
void ReplayViewer::setLFPRequirements(int index)
{
	switch(index)
	{
	case 0:
		playbackController_->enableLFPLoad(true);
		break;
	case 1:
		playbackController_->enableLFPLoad(false);
		break;
	};
}

/*! \brief Called when the PlaybackController reports a load progress change.  Updates the load progress bar.
 */
void ReplayViewer::percentLoaded(double percent)
{
	loadProgress_->setMaximum(100);
	loadProgress_->setValue(percent);
}

/*! \brief Called when the ProgressWidget reports a jump time being requested.  Determines whether the jump is up or down
 *	for the purpose of handling slider updating (see updateTime()) and tells the PlaybackController to enact the jump
 *	(PlaybackController::jumpToTime()).
 *	\sa userChoosingJump()
 */
void ReplayViewer::jumpRequested(double time)
{
	if(time < progress_->getHighlightMax(0))
		jumpDownRequested_ = true;
	else
		jumpDownRequested_ = false;
	playbackController_->jumpToTime(time);
}

/*! \brief Called when the ProgressWidget reports a user starting or ending a jump choice.  Pauses playback when a jump
 *	starts and resumes it when it ends (unless playback was already paused before the jump).
 */
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

/*! \brief Called when the user toggles the record button.
 *	\details Handles changes to the Icon, and the recording state.
 *	\sa updateRecordingTarget()
 */
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

/*! \brief Called when the restart recording button is pressed.  Enacts the request.
 */
void ReplayViewer::restartRecording()
{
	if(!visualTargetHost_)
		return;
	visualTargetHost_->restartRecording();
}

/*! \brief Called when the save recording button is pressed. Shows a dialog box allowing the user to choose a save file
 *	path, saves the file at that path or shows a warning dialog if it doesn't work.
 */
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

/*! \brief Updates the record time in the toolbar label to the latest value.  Also deals with UI changes as a function of
 *	whether the record time is positive or not.
 */
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

/*! \brief Called when the PlaybackController's DesignRoot changes.  Handles hooking its runStarted() signal back up to our
 *	own.
 */
void ReplayViewer::designRootChanged()
{
	//In theory, we don't have to disconnect the old design config when a new one is set because it is done
	//automatically when that designConfig is deleted.
	connect(playbackController_->getDesignRoot()->getExperiment()->getDesignConfig().data(),SIGNAL(runStarted(QUuid)),this,SLOT(runStarted(QUuid)));
}

/*! \brief Called when a preloadError occurs in the PlaybackController.  Presents the input errorStr message in an error
 *	message box.
 */
void ReplayViewer::preloadError(QString errorStr)
{
	QMessageBox::critical(0,"Load Error",errorStr);
	loadProgress_->setValue(0);
}

/*! \brief Called when the run with the input runId starts.  Calls OutputWidgetHolder::newRunStarted().
 */
void ReplayViewer::runStarted(QUuid runId)
{
	outputWidgetHolder_->newRunStarted(runId);
}

/*! \brief Called when playback of the current Run finishes.  Saves Analysis Output automatically if set to do so, updates
 *	the displayed run status in the RunSelectorWidget and starts the next Run if Run playbacks are batched.
 */
void ReplayViewer::finishedPlayback()
{
	//Perform autosave if this was requested
	if(needsAutoSave_)
	{
		outputWidgetHolder_->saveOutput();
	}
	//Update the latest run's color in its widget
	PlayRunInfo latestPlayRunInfo = runQueue_.first();
	runs_->setRunStatus(latestPlayRunInfo.filePath_,latestPlayRunInfo.runIndex_,RunSelectorWidget::COMPLETE);

	//Update the run queue, and play the next run if there's one left
	if(popRunQueueFront())
		play();
}

/*! \brief Called when a loadError occurs in the PlaybackController.  Presents the input errorStr message in an error
 *	message box, updates the Run status in the RunSelectorWidget, and starts playback of the next Run if Runs are batched.
 */
void ReplayViewer::loadError(QString errorMsg)
{
	//Mark the run as an error
	PlayRunInfo latestPlayRunInfo = runQueue_.first();
	runs_->setRunStatus(latestPlayRunInfo.filePath_,latestPlayRunInfo.runIndex_,RunSelectorWidget::ERROROCCURED,errorMsg);

	//If only a single run was begin played, inform the user of the analysis import error
	if(getSelectedPlayRunInfo().size() == 1)
	{
		QMessageBox::warning(0,"Load Error",errorMsg);
	}

	//If there are more runs in the queue, and this run was started with a play (not a pause), play the next run
	if(popRunQueueFront() && calledPlayNotPause_)
	{	
		play();
	}
}

/*! \brief Called when the PlaybackController successfully preloads a Session file.  Adds the preloaded data to the
 *	RunSelectorWidget and AnalysisSelectorWidget.
 */
void ReplayViewer::sessionPreloaded(PreloadedSessionData sessionData)
{
	//A session was preloaded, add its runs to the runs_ widget
	for(int i=0;i<sessionData.runs_.size();i++)
	{
		runs_->addRun(sessionData.savedRuns_.contains(sessionData.runs_[i]),
			sessionData.fileName_,
			sessionData.runs_[i],
			i,
			sessionData.notes_[i]);
	}
	//Add the session's local analyses to the analysisSelector_ widget
	analysisSelector_->setLocalDesignAnalyses(sessionData.fileName_,sessionData.analysisIds_,sessionData.analysisNames_);
}
void ReplayViewer::rewarded(int quantity)
{
	visualTargetHost_->rewarded(quantity);
}
void ReplayViewer::spikeAdded(int channel, int unit, double time)
{
	if (!ChannelsUnits_.contains(channel))
	{
		QList<int> unitList;
		unitList.append(unit);
		ChannelsUnits_.insert(channel, unitList);
		updateNeuralUI();
	}
	else
	{
		QList<int> unitList = ChannelsUnits_.value(channel);
		if (!unitList.contains(unit))
		{
			unitList.append(unit);
			ChannelsUnits_[channel] = unitList;
			//Update the Combo box for channel/unit
			updateNeuralUI();
		}
	}

	//Tell the Visual Target Host to encode audio for the spike, when recording a Picto movie
	visualTargetHost_->spikeAdded(time);

}
void ReplayViewer::updateNeuralUI()
{
	noCallBack_ = true;
	int currCh = channelBox_->itemData(channelBox_->currentIndex()).toInt();
	int currUnit = unitBox_->itemData(unitBox_->currentIndex()).toInt();
	channelBox_->clear();
	unitBox_->clear();

	channelBox_->addItem("All Channels", 0);
	foreach(int channel, ChannelsUnits_.keys())
	{
		channelBox_->addItem(QString("Channel %1").arg(channel), channel);
	}
	//Reset channel to what it was before
	for (int i = 0; i<channelBox_->count(); i++)
	{
		if (channelBox_->itemData(i).toInt() == currCh)
		{
			channelBox_->setCurrentIndex(i);
			break;
		}
	}

	QList<int> unitList = ChannelsUnits_.value(selectedChannel_);
	unitBox_->addItem("All Units", 0);
	foreach(int unit, unitList)
	{
		unitBox_->addItem(QString("Unit %1").arg(unit), unit);
	}

	//Reset unit to what it was before
	for (int i = 0; i<unitBox_->count(); i++)
	{
		if (unitBox_->itemData(i).toInt() == currUnit)
		{
			unitBox_->setCurrentIndex(i);
			break;
		}
	}
	noCallBack_ = false;
}
void ReplayViewer::taskChanged(QString newTask)
{
	//viewSelectionFrame_->clear();
	currentTaskConfig_ = playbackController_->getDesignRoot()->getExperiment().objectCast<Experiment>()->getTaskByName(newTask)->getTaskConfig();
	connect(currentTaskConfig_.data(), SIGNAL(spikeAdded(int, int, double)), this, SLOT(spikeAdded(int, int, double)));
	connect(playbackController_.data(), SIGNAL(rewarded(int)), this, SLOT(rewarded(int)));

	//Plots		
	connect(playbackController_.data(), SIGNAL(alignPlotSig(int)), currentTaskConfig_.data(), SLOT(alignPlot(int)));	
	connect(playbackController_.data(), SIGNAL(eventAddedSig(int)), currentTaskConfig_.data(), SLOT(eventAdded(int)));
	connect(playbackController_.data(), SIGNAL(scriptContAddedSig(int)), currentTaskConfig_.data(), SLOT(scriptContAdded(int)));
	//end Plots
	viewSelectionFrame_->connectToTaskConfig(playbackController_->getDesignRoot()->getExperiment().objectCast<Experiment>()->getTaskByName(newTask)->getTaskConfig());
	viewSelectionFrame_->setTaskName(newTask);
	viewSelectionFrame_->rebuild();
	ChannelsUnits_.clear();
	updateNeuralUI();

	selectedChannelChanged();
}

// for Plots
/*! \brief Called when the PlaybackController successfully preloads a Session file.  Adds the preloaded data to the
*	RunSelectorWidget and AnalysisSelectorWidget.
*/
void ReplayViewer::sessionLoaded(LoadedSessionData sessionData)
{
	alignElements_ = sessionData.alignElements_;
	spikeReader_ = sessionData.spikeReader_;

	if (activatePlots_)
		viewSelectionFrame_->preloadEvents(playbackController_->getDesignRoot(), alignElements_, spikeReader_);
}
void ReplayViewer::openInNewTab(QWidget* pWidget)
{
	if (!activatePlots_)
		return;

	QString tabName = ((DataViewWidget*)pWidget)->getName();
	if (tabName != "Task")
	{
		viewSelectionFrame_->hideCurrentWidget(pWidget);
		viewTabs_->addTab(pWidget, tabName);
		int index = viewTabs_->indexOf(pWidget);
		viewTabs_->setCurrentIndex(index);
	}
}

void ReplayViewer::closeTab(int index)
{	
if (!activatePlots_)
		return;


	int currIndex = index;

	if (currIndex < 1)
		return;

	QWidget* pWidget = viewTabs_->widget(currIndex);
	if (pWidget)
	{
		bool sameTabAsTask = ((PlotViewWidget*)pWidget)->tabInfo();
		if (!sameTabAsTask)
		{		
			QWidget* pDVWidget = ((DataViewWidget*)(viewTabs_->widget(currIndex)))->getWidget();
			viewTabs_->removeTab(currIndex);
			viewSelectionFrame_->removeFromList(pDVWidget);
		}
	}
}
/*! \brief Callback helper when a checkbox is clicked.
*/
void ReplayViewer::checkClicked(bool)
{	
	if (enablePlots_->checkState() == Qt::Unchecked)
	{
		viewSelectionFrame_->resetPlotsList(1);
		activatePlots_ = false;
	}
	else
		activatePlots_ = true;

	playbackController_->activatePlots(activatePlots_);
	viewSelectionFrame_->updateUI(activatePlots_);
}