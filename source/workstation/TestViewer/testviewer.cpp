
#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QComboBox>
#include <QLabel>
#include <QXmlStreamReader>
#include <QMenu>
#include <QFileDialog>
#include <QSlider>
#include <QTabWidget>
#include <QSplitter>

#include "testviewer.h"

#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/compositor/OutputSignalWidget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/designer/propertyframe.h"
#include "../../common/designer/UIHelper.h"
#include "../../common/engine/XYSignalChannel.h"
#include "../../common/engine/MouseInputPort.h"
#include "../../common/parameter/OperatorClickParameter.h"

#include "../DataViewer/DataViewOrganizer.h"
#include "../DataViewer/DataViewWidget.h"
#include "../DataViewer/ViewSelectionFrame.h"

#include "../../common/memleakdetect.h"

#include "../../common/storage/NeuralDataUnit.h"

TestViewer::TestViewer(QWidget *parent) :
	Viewer(parent),
	status_(Stopped),
	isRunning_(false),
	NeuralTick_("audio/tick.wav"),
	selectedChannel_(0), selectedUnit_(0), noCallBack_(false)
{
	setupEngine();
	setupUi();
}

/*! \brief Called just before displaying the viewer.
 *	\details This function prepares the DesignRoot and the various UI components for a test run.
 *	\note Sometimes while working on an experiment, if we change something and don't reopen the experiment
 *	we have found that the experimental run doesn't work right.  To prevent this, we are simply reseting 
 *	the Experiment from xml whenever this Viewer is opened and init is called.  At some point, we should
 *	probably go through and debug this issue.
 */
void TestViewer::init()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Sometimes while working on an experiment, if we change something and don't reopen the experiment
	//the experimental run doesn't work right.  To prevent this, we are simply reseting the the experiment
	//from xml whenever we open this viewer
	designRoot_->refreshFromXml();
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	viewSelectionFrame_->clear();

	deiniting_ = false;
	designRoot_->enableRunMode(true);
	QSharedPointer<DesignRoot> myDesignRoot = designRoot_;
	if(!myDesignRoot->compiles())
	{
		QMessageBox msg;
		msg.setText("Experiment does not compile.");
		msg.setIconPixmap(QPixmap(":/icons/triangle.png"));
		msg.exec();
		return;
	}
	experiment_ = myDesignRoot->getExperiment().staticCast<Experiment>();
	if(!experiment_)
	{
		QMessageBox msg;
		msg.setText("Failed to load current experiment.");
		msg.setIconPixmap(QPixmap(":/icons/triangle.png"));
		msg.exec();
		return;
	}
	if(experiment_)
	{
		experiment_->setEngine(engine_);
		static_cast<PropertyFrame*>(propertyFrame_)->setTopLevelDataStore(experiment_.staticCast<DataStore>());
		loadPropsAction_->setEnabled(true);
		testController_ = QSharedPointer<TestPlaybackController>(new TestPlaybackController(experiment_));
		connect(playAction_,SIGNAL(triggered()),this, SLOT(playTriggered()));
		connect(playAction_,SIGNAL(triggered()),testController_.data(), SLOT(play()));
		connect(pauseAction_,SIGNAL(triggered()),testController_.data(), SLOT(pause()));
		connect(engine_.data(),SIGNAL(pauseRequested()),testController_.data(),SLOT(pause()));
		connect(stopAction_,SIGNAL(triggered()),testController_.data(), SLOT(stop()));
		connect(testController_.data(),SIGNAL(running()),this,SLOT(running()));
		connect(testController_.data(),SIGNAL(paused()),this,SLOT(paused()));
		connect(testController_.data(),SIGNAL(stopped()),this,SLOT(stopped()));
	}
	engine_->setFrameTimerFactory(experiment_->getDesignConfig()->getFrameTimerFactory());
	viewSelectionFrame_->clearAnalysisSelection(true);
	generateComboBox();
	analysisSelector_->setLocalDesignRoot(designRoot_->getExperiment()->getName(),designRoot_);
	analysisSelector_->setCurrentFile(designRoot_->getExperiment()->getName());
	connect(designRoot_->getExperiment()->getDesignConfig().data(),SIGNAL(runStarted(QUuid)),this,SLOT(runStarted(QUuid)));
}

/*! \brief Called just before hiding the viewer.
 *	\details This function stops the current Task Run and clears the PropertyFrame.  It is 
 *	important that it clears the PropertyFrame because otherwise an unexpected signal->slot->signal->...
 *	path leading from Property value changes through the PropertyFrame back to
 *	a different part of the Property value would be active.  This would lead to unexpected behavior
 *	in the StateMachineEditor.  We also handle stopping the Experiment in a kind of round about way.
 *	Due to the fact that we don't yet run Experiments from a Qt Event loop and we need to just
 *	process events internally now and then, if an Experiment is running, this function will have
 *	been called from some QCoreApplication::processEvents() call internal to the Experiment.  In that
 *	case, calling stop() here will not actually stop the Experiment until somewhere way up the call stack.
 *	If we were to directly emit deinitCompleted() here, we would be telling the next viewer that it can
 *	take the focus before this Viewer's Experiment actually stopped running.  Instead, we connect the
 *	TestPlaybackController's stopped() signal to our stopped() slot which emits deinitCompleted().  By
 *	doing things this way, we are sure that when the next Viewer takes focus, this Viewer's test Run
 *	will actually be fully stopped and done.
 */
void TestViewer::deinit()
{
	deiniting_ = true;
	if(testController_ && testController_->isRunning())
	{
		//If the experiment is still running, tell it to stop.
		//After it stops, the testController will trigger its stopped() signal which
		//will trigger our stopped() slot.  This will handle emitting the deinitCompleted()
		//signal.  It is important that we do it this way in case the event loop is
		//currently running from within the experimental run.  In that case, we need to actually leave
		//this deinit() function after telling the experiment to stop in order to give it a
		//chance to actually follow the stop command.  Only once testController emits stopped()
		//can we be sure that we're really finished with the current experiment.  Only then
		//is it safe to give control to another viewer.
		testController_->stop();
	}
	else
		stopped();
	Q_ASSERT(propertyFrame_);
	//Clear the property frame by setting its data store to null.  If we don't do this then an unexpected
	//path leading from property value changes through the property frame back to
	//a different part of the propery value would be active... this gets rid of that path
	static_cast<PropertyFrame*>(propertyFrame_)->setTopLevelDataStore(QSharedPointer<DataStore>());
}

/*! \brief Called when the application is about to quit.  In this case, it just calls deinit().
 */
bool TestViewer::aboutToQuit()
{
	deinit();
	return true;
}

/*! \brief Initializes the engine with all of the appropriate pieces for testing
 */
void TestViewer::setupEngine()
{
	//set up the engine
	engine_ = QSharedPointer<Picto::Engine::PictoEngine>(new Picto::Engine::PictoEngine);
	engine_->setExclusiveMode(false);
	engine_->setOperatorAsUser(true);
	engine_->setSlaveMode(false);

	//Set up the rendering target
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	pixmapVisualTarget_ = QSharedPointer<Picto::PixmapVisualTarget>(new Picto::PixmapVisualTarget(true,800,600));
	renderingTarget_ = QSharedPointer<Picto::RenderingTarget>(new Picto::RenderingTarget(pixmapVisualTarget_, pcmAuralTarget));
	engine_->addRenderingTarget(renderingTarget_);

	//Set up the visual target host
	//This exists because QSharedPointer<QWidget> results in multiple delete call, which 
	//gives us memory exceptions.
	visualTargetHost_ = new Picto::RecordingVisualTargetHost();
	visualTargetHost_->setVisualTarget(pixmapVisualTarget_);
	connect(visualTargetHost_, SIGNAL(clickDetected(QPoint)), this, SLOT(operatorClickDetected(QPoint)));
	connect(visualTargetHost_, SIGNAL(updateRecordingTime(double)), this, SLOT(setRecordTime(double)));


	//set up mouse signal channel
	QSharedPointer<Picto::MouseInputPort> mousePort(new Picto::MouseInputPort(visualTargetHost_));
	QSharedPointer<Picto::XYSignalChannel> mouseChannel(new Picto::XYSignalChannel("Position",0,1,8,mousePort));
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
	connect(rewardController.data(), SIGNAL(rewarded(int)), this, SLOT(rewarded(int)));

	//display the splash screen
	renderingTarget_->showSplash();
}

/*! \brief Sets up the user interface portions of the GUI
 */
void TestViewer::setupUi()
{
	///play/pause/stop actions and toolbar
	playAction_ = new QAction(tr("&Start task"),this);
	playAction_->setIcon(QIcon(":/icons/play.png"));
	playAction_->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
	playAction_->setToolTip("Run (Ctrl+R)");

	pauseAction_ = new QAction(tr("&Pause task"),this);
	pauseAction_->setIcon(QIcon(":/icons/pause.png"));
	pauseAction_->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_P));
	pauseAction_->setToolTip("Pause (Ctrl+P)");
	pauseAction_->setEnabled(false);

	stopAction_ = new QAction(tr("S&top task"),this);
	stopAction_->setIcon(QIcon(":/icons/stop.png"));
	stopAction_->setToolTip("Stop");
	stopAction_->setEnabled(false);

	loadPropsAction_ = new QAction(tr("&Load Task Properties from Session"),this);
	loadPropsAction_->setIcon(QIcon(":/icons/loadvalues.png"));
	loadPropsAction_->setToolTip("Load Task Properties from Session");
	connect(loadPropsAction_, SIGNAL(triggered()),this, SLOT(LoadPropValsFromFile()));
	loadPropsAction_->setEnabled(false);

	userType_ = new QComboBox();
	userType_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	userType_->setToolTip("Select a User View");
	userType_->addItem("Operator",0);
	userType_->addItem("Test Subject",1);
	connect(userType_,SIGNAL(currentIndexChanged(int)),this,SLOT(setUserType(int)));
	Q_ASSERT(!engine_.isNull());
	engine_->setOperatorAsUser(true);
	engine_->setSlaveMode(false);


	//TaskList combo box
	taskListBox_ = new QComboBox(this);
	taskListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	taskListBox_->setToolTip("Select Task");

	//recording tool
	toggleRecord_ = new QAction(tr("&Toggle Record"), this);
	toggleRecord_->setIcon(QIcon(":/icons/red_led_off.svg"));
	connect(toggleRecord_, SIGNAL(triggered()), this, SLOT(toggleRecording()));
	toggleRecord_->setEnabled(false);

	//Indicates if the toggleRecord button is set for record mode or not
	recordModeOn_ = false;

	recordTime_ = new QLCDNumber(6);
	recordTime_->setSegmentStyle(QLCDNumber::Flat);
	setRecordTime(0);

	restartRecord_ = new QAction(tr("&Restart Record"), this);
	restartRecord_->setIcon(QIcon(":/icons/restart.svg"));
	connect(restartRecord_, SIGNAL(triggered()), this, SLOT(restartRecording()));
	restartRecord_->setEnabled(false);

	saveRecording_ = new QAction(tr("&Save Recording"), this);
	saveRecording_->setIcon(QIcon(":/icons/savevideo.svg"));
	connect(saveRecording_, SIGNAL(triggered()), this, SLOT(saveRecording()));
	saveRecording_->setEnabled(false);

	channelBox_ = new QComboBox();
	channelBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	channelBox_->setToolTip("Select a Channel");
	channelBox_->addItem("All Channels", 0);
	channelBox_->setCurrentIndex(0);
	connect(channelBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedChannelChanged(int)));

	unitBox_ = new QComboBox(this);
	unitBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	unitBox_->setToolTip("Select a Unit");
	unitBox_->addItem("All Units", 0);
	unitBox_->setCurrentIndex(0);
	connect(unitBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedUnitChanged(int)));
	//end recording

	testToolbar_ = new QToolBar(this);
	testToolbar_->addAction(playAction_);
	testToolbar_->addAction(pauseAction_);
	testToolbar_->addWidget(taskListBox_);
	testToolbar_->addAction(stopAction_);
	testToolbar_->addSeparator();
	testToolbar_->addAction(loadPropsAction_);
	testToolbar_->addSeparator();
	testToolbar_->addWidget(userType_); 
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

	DataViewOrganizer *dataViewOrganizer = new DataViewOrganizer();

	QVBoxLayout *stimulusLayout = new QVBoxLayout;
	stimulusLayout->setContentsMargins(0, 0, 0, 0);
	stimulusLayout->setMargin(0);
	stimulusLayout->addWidget(dataViewOrganizer);

	foreach(QSharedPointer<Picto::VirtualOutputSignalController> cont,outSigControllers_)
	{
		outputSignalsWidgets_.push_back(new OutputSignalWidget(cont));
		stimulusLayout->addWidget(outputSignalsWidgets_.back());
	}
	
	propertyFrame_ = new PropertyFrame();
	connect(taskListBox_,SIGNAL(currentIndexChanged(int)),this,SLOT(taskListIndexChanged(int)));

	QTabWidget* rightTabs = new QTabWidget();
	analysisSelector_ = new Picto::AnalysisSelectorWidget();
	outputWidgetHolder_ = new OutputWidgetHolder();
	rightTabs->addTab(analysisSelector_,"Select Analyses");
	rightTabs->addTab(outputWidgetHolder_,"Analysis Output");

	viewSelectionFrame_ = new ViewSelectionFrame();
	viewSelectionFrame_->setVisualTargetHost(visualTargetHost_);
	viewSelectionFrame_->connectToViewerLayout(dataViewOrganizer);
	viewSelectionFrame_->setStyleSheet("ViewSelectionFrame { border: 1px solid gray }");

	connect(analysisSelector_, SIGNAL(notifyAnalysisSelection(const QString&, bool, bool)),
		viewSelectionFrame_, SLOT(notifyAnalysisSelection(const QString&, bool, bool)));

	connect(analysisSelector_, SIGNAL(clearAnalysisSelection(bool)),
		viewSelectionFrame_, SLOT(clearAnalysisSelection(bool)));

	QVBoxLayout *leftToolFrame = new QVBoxLayout;
	leftToolFrame->addWidget(viewSelectionFrame_, 0);
	leftToolFrame->addWidget(propertyFrame_, 1);
	leftToolFrame->setContentsMargins(0, 11, 0, 0);
	leftToolFrame->setSizeConstraint(QLayout::SetMinAndMaxSize);
	leftToolFrame->setAlignment(Qt::AlignTop);

	QSplitter *operationLayout = new QSplitter(Qt::Horizontal);
	QWidget *container = new QWidget(operationLayout);
	container->setLayout(leftToolFrame);
	container->sizePolicy().setHorizontalStretch(QSizePolicy::Maximum);
	operationLayout->addWidget(container);
	operationLayout->setStretchFactor(0, 0);
	operationLayout->setCollapsible(0, true);

	container = new QWidget(operationLayout);
	container->setLayout(stimulusLayout);
	operationLayout->addWidget(container);
	operationLayout->setStretchFactor(1, 10);

	operationLayout->addWidget(rightTabs);
	operationLayout->setStretchFactor(2, 3);

	Picto::UIHelper::addSplitterLine(operationLayout->handle(1), 200);
	Picto::UIHelper::addSplitterLine(operationLayout->handle(2), 200);

	operationLayout->setHandleWidth(11);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout,0);
	mainLayout->addWidget(operationLayout,1);
	setLayout(mainLayout);
}

/*! \brief Loads runtime Property initValues from a previous Session to replace those in the current Experiment.
 *	\warning We haven't tested this in a while and it may not be working.  Regardless, it might be good to do this
 *	differently and allow operators to save current Property configurations and restore them later.
 */
void TestViewer::LoadPropValsFromFile()
{
	QString filename = QFileDialog::getOpenFileName(this,
			tr("Load Properties From Session"),".","Sqlite files (*.sqlite)");
	if(propertyFrame_)
		static_cast<PropertyFrame*>(propertyFrame_)->updatePropertiesFromFile(filename);
}

/*! \brief Generates the task list combobox.
 */
void TestViewer::generateComboBox()
{
	Q_ASSERT(taskListBox_);
	taskListBox_->clear();

	if(!experiment_)
		return;

	taskListBox_->addItems(experiment_->getTaskNames());
}

/*! \brief Called when the play button is pressed.  Activates selected Analyses, disables the AnalysisSelectorWidget
 *	and sets up Live Data Readers for the active Analyses to use for the test.  
 *
 *	\note Live Data Readers are used by Analyses to access data over ranges of times.  They
 *	are different from the Data Readers in played back Sessions in that they have no access to future data
 *	and in some cases their data is simulated (ie. LFP, since there is no neural system attached to the
 *	TestViewer).
 */
void TestViewer::playTriggered()
{
	if(!testController_->isRunning())
	{
		//Find out which analyses should be enabled and enable them
		QList<QUuid> analysisIds = analysisSelector_->getSelectedAnalysisIds();
		QSharedPointer<DesignConfig> designConfig = experiment_->getDesignConfig();
		//If there are active analysis ids, add the various data readers (frameReader, etc) to the 
		//designConfig.  Then add them to the Picto Engine so that it can update them with data.
		//Generally, the SlaveExperimentDriver is responsible for creating the readers and the StateUpdater
		//is responsible for making sure they have data.  Eventually, the Test Viewer should contain a 
		//running experiment and an interface to a SlaveExperimentViewer that handles the playback and analysis.
		//Until that happens, we create the DataReaders here and update them in the PictoEngine.
		if(analysisIds.size())
		{
			if(!liveFrameReader_)
			{
				//Setup Test Data Readers for Analysis
				liveFrameReader_ = QSharedPointer<LiveFrameReader>(new LiveFrameReader());
				liveLfpReader_ = QSharedPointer<LiveLfpReader>(new LiveLfpReader(QList<int>() << 1 << 3 << 4 << 5 << 8,0.001));
				liveRewardReader_ = QSharedPointer<LiveRewardReader>(new LiveRewardReader());
				liveRunNotesReader_ = QSharedPointer<LiveRunNotesReader>(new LiveRunNotesReader());
				liveSpikeReader_ = QSharedPointer<LiveSpikeReader>(new LiveSpikeReader(5,2,32));
				signalReaders_.append(QSharedPointer<LiveSignalReader>(new LiveSignalReader("Position",QStringList()<<"x"<<"y",0.002)));
				signalReaders_.append(QSharedPointer<LiveSignalReader>(new LiveSignalReader("Diameter", QStringList() << "x" << "y", 0.004)));
				signalReaders_.append(QSharedPointer<LiveSignalReader>(new LiveSignalReader("GenericInputs", QStringList() << "input0" << "input1" << "input2" << "input3" << "input4" << "input5" << "input6" << "input7", 0.004)));
			}
			designConfig->setFrameReader(liveFrameReader_.staticCast<FrameReader>());
			designConfig->setLfpReader(liveLfpReader_.staticCast<LfpReader>());
			designConfig->setRewardReader(liveRewardReader_.staticCast<RewardReader>());
			designConfig->setRunNotesReader(liveRunNotesReader_.staticCast<RunNotesReader>());
			designConfig->setSpikeReader(liveSpikeReader_.staticCast<SpikeReader>());
			foreach(QSharedPointer<LiveSignalReader> signalReader,signalReaders_)
			{
				designConfig->setSignalReader(signalReader->getName(),signalReader.staticCast<SignalReader>());
			}
			engine_->setFrameReader(liveFrameReader_);
			engine_->setLfpReader(liveLfpReader_);
			engine_->setRewardReader(liveRewardReader_);
			engine_->setSpikeReader(liveSpikeReader_);
			connect(liveSpikeReader_.data(), SIGNAL(spikeEvent(double, int, int, QVariantList)), this, SLOT(spikeEvent(double, int, int, QVariantList)));
			foreach(QSharedPointer<LiveSignalReader> signalReader,signalReaders_)
			{
				engine_->setSignalReader(signalReader->getName(),signalReader);
			}
		}
		else
		{
			//If there's no analysis.  Set frame readers to null so that we don't waste memory on them.
			//in case they were set last time.
			designConfig->setFrameReader(QSharedPointer<FrameReader>());
			designConfig->setLfpReader(QSharedPointer<LfpReader>());
			designConfig->setRewardReader(QSharedPointer<RewardReader>());
			designConfig->setSpikeReader(QSharedPointer<SpikeReader>());
			designConfig->clearSignalReaders();
			engine_->setFrameReader(QSharedPointer<LiveFrameReader>());
			engine_->setLfpReader(QSharedPointer<LiveLfpReader>());
			engine_->setRewardReader(QSharedPointer<LiveRewardReader>());
			engine_->setSpikeReader(QSharedPointer<LiveSpikeReader>());
			engine_->clearSignalReaders();
		}


		designConfig->setActiveAnalysisIds(analysisIds);
	}

	analysisSelector_->setEnabled(false);
	//QMetaObject::invokeMethod(testController_.data(),"play");
}

/*! \brief This is called when the TestPlaybackController reports that a Task is running.  It changes
 *	various aspects of the UI accordingly.
 */
void TestViewer::running()
{
	isRunning_ = true;

	taskListBox_->setEnabled(false);
	pauseAction_->setEnabled(true);
	stopAction_->setEnabled(true);
	playAction_->setEnabled(false);
	toggleRecord_->setEnabled(true);
	foreach(QWidget * outSigWidg, outputSignalsWidgets_)
	{
		static_cast<OutputSignalWidget*>(outSigWidg)->enable(true);
	}
}

/*! \brief This is called when the TestPlaybackController reports that a Task is paused.  It changes
 *	various aspects of the UI accordingly.
 */
void TestViewer::paused()
{
	taskListBox_->setEnabled(false);
	pauseAction_->setEnabled(false);
	stopAction_->setEnabled(true);
	playAction_->setEnabled(true);
	toggleRecord_->setEnabled(false);
}

/*! \brief This is called when the TestPlaybackController reports that a Task is stopped.  It changes
 *	various aspects of the UI accordingly, disables active Analyses, puts the splash screen up in the
 *	central window, and emits deinitComplete() if the stopping was a result of the deinit() function
 *	(See deinit() for more details).
 */
void TestViewer::stopped()
{
	taskListBox_->setEnabled(true);
	pauseAction_->setEnabled(false);
	stopAction_->setEnabled(false);
	playAction_->setEnabled(true);
	toggleRecord_->setEnabled(false);
	foreach(QWidget * outSigWidg, outputSignalsWidgets_)
	{
		static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
	}

	pixmapVisualTarget_->clear();

	//Disable any active analyses
	if(experiment_)
	{
		QSharedPointer<DesignConfig> designConfig = experiment_->getDesignConfig();
		designConfig->setActiveAnalysisIds(QList<QUuid>());
	}

	//Enable the Analysis Selector
	analysisSelector_->setEnabled(true);
	//display the splash screen
	renderingTarget_->showSplash();
	if(deiniting_)
	{
		emit deinitComplete();
		return;
	}
}

/*! \brief This is called when the selected Task in the task combobox changes.  It updates the run time enabled Properties
 *	in the PropetyFrame for that Task and sets that Task as the active Task in the TestPlaybackController.
 */
void TestViewer::taskListIndexChanged(int)
{
	if(!experiment_)
		return;
	QSharedPointer<Task> task = experiment_->getTaskByName(taskListBox_->currentText());
	if(!task)
		return;

	//viewSelectionFrame_->clear();
	viewSelectionFrame_->connectToTaskConfig(task->getTaskConfig());
	viewSelectionFrame_->rebuild();

	qobject_cast<PropertyFrame*>(propertyFrame_)->setTopLevelDataStore(task.staticCast<DataStore>());
	loadPropsAction_->setEnabled(true);
	Q_ASSERT(testController_);
	testController_->setTask(taskListBox_->currentText());
}

/*! \brief This is called when the operator clicks somewhere in the display window.  It informs the OperatorClickParameter
 *	that a click was detected (OperatorClickParameter::addClick()).  This is important in order to be able to test
 *	experiments that use the OperatorClickParameter, like the Calibration Task.
 */
void TestViewer::operatorClickDetected(QPoint pos)
{
	OperatorClickParameter::addClick(pos);
}

/*! \brief Called when the User Type dropdown value changes.  Changes the user type in the PlaybackController accordingly.
 *	\note The active user, Operator or Subject, determines which graphics are shown in the display depending on each
 *	graphics OperatorView/SubjectView Properties.
 */
void TestViewer::setUserType(int index)
{
	Q_ASSERT(!engine_.isNull());
	switch(index)
	{
	case 0:
		engine_->setOperatorAsUser(true);
		break;
	case 1:
		engine_->setOperatorAsUser(false);
		break;
	}
}

/*! \brief Called when the run with the input runId starts.  Calls OutputWidgetHolder::newRunStarted().
 */
void TestViewer::runStarted(QUuid runId)
{
	outputWidgetHolder_->newRunStarted(runId);
}
void TestViewer::spikeEvent(double time, int channel, int unit, QVariantList waveform)
{
	//Emit a sound for each spike detected
	if ((selectedChannel_ == 0) || ((channel == selectedChannel_) && ((unit == selectedUnit_) || (selectedUnit_ == 0))))
	{
		//play sound
		NeuralTick_.play();

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
				//Update the Combo box for channel/unit
				updateNeuralUI();
			}
		}

		//Tell the Visual Target Host to encode audio for the spike, when recording a Picto movie
		visualTargetHost_->spikeAdded(time);
	}
}
void TestViewer::rewarded(int quantity)
{
	visualTargetHost_->rewarded(quantity);
}
void TestViewer::updateNeuralUI()
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
void TestViewer::selectedChannelChanged(int channel)
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

	unitBox_->clear();
	selectedUnit_ = 0;

	QList<int> unitList = ChannelsUnits_.value(selectedChannel_);
	unitBox_->addItem("All Units", 0);
	foreach(int unit, unitList)
	{
		unitBox_->addItem(QString("Unit %1").arg(unit), unit);
	}

	//This will take care of resetting the spike plot for the case where channel changes but unit index doesn't change.
	selectedUnitChanged(unitBox_->currentIndex());
}
void TestViewer::selectedUnitChanged(int unit)
{
	if (noCallBack_)
		return;

	if (selectedChannel_ < 0)
		selectedChannel_ = 0;

	if (selectedUnit_ < 0)
		selectedUnit_ = 0;

	QString selectedStr = unitBox_->itemData(unitBox_->currentIndex()).toString();
	if (selectedStr.split(QRegExp("\\s")).last().toInt())
		selectedUnit_ = selectedStr.split(QRegExp("\\s")).last().toInt();
}

//recording command added
/*! \brief Updates the Recording Visual Target Host to record or not depending on the current run and record mode states.
*/
void TestViewer::updateRecordingTarget()
{
	if (recordModeOn_ && (isRunning_))
	{
		if (!visualTargetHost_->isRecording())
			visualTargetHost_->toggleRecording();
	}
	else
	{
		if (visualTargetHost_->isRecording())
			visualTargetHost_->toggleRecording();
	}
}
/*! \brief Called when the user toggles the record button.
*	\details Handles changes to the Icon, and the recording state.
*	\sa updateRecordingTarget()
*/
void TestViewer::toggleRecording()
{
	if (!visualTargetHost_)
		return;
	//Toggle record mode
	recordModeOn_ = !recordModeOn_;

	//Update the recording LED according to the new recording state.
	if (recordModeOn_)
		toggleRecord_->setIcon(QIcon(":/icons/red_led_on.svg"));
	else
		toggleRecord_->setIcon(QIcon(":/icons/red_led_off.svg"));

	updateRecordingTarget();
}

/*! \brief Called when the restart recording button is pressed.  Enacts the request.
*/
void TestViewer::restartRecording()
{
	if (!visualTargetHost_)
		return;
	visualTargetHost_->restartRecording();
}

/*! \brief Called when the save recording button is pressed. Shows a dialog box allowing the user to choose a save file
*	path, saves the file at that path or shows a warning dialog if it doesn't work.
*/
void TestViewer::saveRecording()
{
	QString filename = QFileDialog::getSaveFileName(this,
		tr("Save Video File"), ".", QString("Video (*.%1)").arg(visualTargetHost_->getVideoFileType()));
	if (filename.isNull())
		return;
	if (!visualTargetHost_->saveRecordingAs(filename))
	{
		QMessageBox::warning(0, "Failed to Save Video", "The recorded video could not be saved at " + filename.toLatin1() + ".");
	}
}

/*! \brief Updates the record time in the toolbar label to the latest value.  Also deals with UI changes as a function of
*	whether the record time is positive or not.
*/
void TestViewer::setRecordTime(double time)
{
	if (recordTime_->value() == time)
		return;
	//If record time is positive, enable restart and saving, otherwise turn them off
	if (recordTime_ > 0)
	{
		restartRecord_->setEnabled(true);
		saveRecording_->setEnabled(true);
	}
	else
	{
		restartRecord_->setEnabled(false);
		saveRecording_->setEnabled(false);
	}
	recordTime_->display(QString::number(time, 'f', 3));
}
//end recording