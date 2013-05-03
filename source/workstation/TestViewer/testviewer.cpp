
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

#include "testviewer.h"

#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/designer/propertyframe.h"
#include "../../common/engine/XYSignalChannel.h"
#include "../../common/engine/MouseInputPort.h"
#include "../../common/parameter/OperatorClickParameter.h"
#include "../../common/compositor/OutputSignalWidget.h"
#include "../../common/memleakdetect.h"


TestViewer::TestViewer(QWidget *parent) :
	Viewer(parent),
	status_(Stopped)

{
	setupEngine();
	setupUi();
}

//! Called just before displaying the viewer
void TestViewer::init()
{
	
	////Sometimes while working on an experiment, if we change something and don't reopen the experiment
	////the experimental run doesn't work right.  To prevent this, we are simply reseting the the experiment
	////from xml whenever we open this viewer
	//designRoot_->refreshFromXml();

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
		connect(playAction_,SIGNAL(triggered()),testController_.data(), SLOT(play()));
		connect(pauseAction_,SIGNAL(triggered()),testController_.data(), SLOT(pause()));
		connect(engine_.data(),SIGNAL(pauseRequested()),testController_.data(),SLOT(pause()));
		connect(stopAction_,SIGNAL(triggered()),testController_.data(), SLOT(stop()));
		connect(testController_.data(),SIGNAL(running()),this,SLOT(running()));
		connect(testController_.data(),SIGNAL(paused()),this,SLOT(paused()));
		connect(testController_.data(),SIGNAL(stopped()),this,SLOT(stopped()));
	}
	generateComboBox();


	//FOR TESTING, WE ARE JUST AUTOMATICALLY ACTIVATING THE FIRST ANALYSIS IN THE FILE.
	QSharedPointer<Analysis> firstAnalysis = designRoot_->getAnalysis(0).staticCast<Analysis>();
	if(firstAnalysis)
	{
		QList<QUuid> analysisIds;
		analysisIds.append(firstAnalysis->getAssociateId());
		QSharedPointer<DesignConfig> designConfig = experiment_->getDesignConfig();
		designConfig->setActiveAnalysisIds(analysisIds);
	}
}

//!Called just before hiding the viewer
void TestViewer::deinit()
{
	deiniting_ = true;
	if(testController_ && testController_->isRunning())
	{
		//If the experiment is still running, tell it to stop.
		//After it stops, the testController will trigger its stopped() signal which will
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
}

//! \brief Called when the application is about to quit.  Takes care of closing this windows resources
bool TestViewer::aboutToQuit()
{
	deinit();
	return true;
}

//! Initializes the engine with all of the appropriate pieces for testing
void TestViewer::setupEngine()
{
	//set up the engine
	engine_ = QSharedPointer<Picto::Engine::PictoEngine>(new Picto::Engine::PictoEngine);
	engine_->setExclusiveMode(false);
	engine_->setOperatorAsUser(true);

	//Set up the rendering target
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	pixmapVisualTarget_ = QSharedPointer<Picto::PixmapVisualTarget>(new Picto::PixmapVisualTarget(true,800,600));
	renderingTarget_ = QSharedPointer<Picto::RenderingTarget>(new Picto::RenderingTarget(pixmapVisualTarget_, pcmAuralTarget));
	engine_->addRenderingTarget(renderingTarget_);

	//Set up the visual target host
	//This exists because QSharedPointer<QWidget> results in multiple delete call, which 
	//gives us memory exceptions.
	visualTargetHost_ = new Picto::VisualTargetHost();
	visualTargetHost_->setVisualTarget(pixmapVisualTarget_);
	connect(visualTargetHost_,SIGNAL(clickDetected(QPoint)),this,SLOT(operatorClickDetected(QPoint)));

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

	//display the splash screen
	renderingTarget_->showSplash();
}

//! Sets up the user interface portions of the GUI
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

	////Zoom slider
	//zoomSlider_ = new QSlider;
	//zoomSlider_->setRange(1,59);
	//zoomSlider_->setSingleStep(1);
	//zoomSlider_->setValue(30);
	//zoomSlider_->setOrientation(Qt::Horizontal);
	//connect(zoomSlider_, SIGNAL(sliderMoved(int)), this, SLOT(zoomChanged(int)));


	//TaskList combo box
	taskListBox_ = new QComboBox(this);
	taskListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	taskListBox_->setToolTip("Select Task");

	testToolbar_ = new QToolBar(this);
	testToolbar_->addAction(playAction_);
	testToolbar_->addAction(pauseAction_);
	testToolbar_->addWidget(taskListBox_);
	testToolbar_->addAction(stopAction_);
	testToolbar_->addSeparator();
	testToolbar_->addAction(loadPropsAction_);
	//testToolbar_->addWidget(zoomSlider_);
	
	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(testToolbar_);
	toolbarLayout->addStretch();

	QVBoxLayout *stimulusLayout = new QVBoxLayout;
	stimulusLayout->addWidget(visualTargetHost_);
	foreach(QSharedPointer<Picto::VirtualOutputSignalController> cont,outSigControllers_)
	{
		outputSignalsWidgets_.push_back(new OutputSignalWidget(cont));
		stimulusLayout->addWidget(outputSignalsWidgets_.back());
	}

	QHBoxLayout *operationLayout = new QHBoxLayout;
	propertyFrame_ = new PropertyFrame();
	connect(taskListBox_,SIGNAL(currentIndexChanged(int)),this,SLOT(taskListIndexChanged(int)));
	operationLayout->addWidget(propertyFrame_,Qt::AlignTop);
	operationLayout->addLayout(stimulusLayout);
	operationLayout->setStretch(0,0);
	operationLayout->addStretch();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	mainLayout->addLayout(operationLayout);
	mainLayout->addStretch(1);
	setLayout(mainLayout);

}

void TestViewer::LoadPropValsFromFile()
{
	QString filename = QFileDialog::getOpenFileName(this,
			tr("Load Properties From Session"),".","Sqlite files (*.sqlite)");
	if(propertyFrame_)
		static_cast<PropertyFrame*>(propertyFrame_)->updatePropertiesFromFile(filename);
}

void TestViewer::generateComboBox()
{
	Q_ASSERT(taskListBox_);
	taskListBox_->clear();

	if(!experiment_)
		return;

	taskListBox_->addItems(experiment_->getTaskNames());
}

void TestViewer::running()
{
	taskListBox_->setEnabled(false);
	pauseAction_->setEnabled(true);
	stopAction_->setEnabled(true);
	playAction_->setEnabled(false);
	foreach(QWidget * outSigWidg, outputSignalsWidgets_)
	{
		static_cast<OutputSignalWidget*>(outSigWidg)->enable(true);
	}
}
void TestViewer::paused()
{
	taskListBox_->setEnabled(false);
	pauseAction_->setEnabled(false);
	stopAction_->setEnabled(true);
	playAction_->setEnabled(true);
}
void TestViewer::stopped()
{
	taskListBox_->setEnabled(true);
	pauseAction_->setEnabled(false);
	stopAction_->setEnabled(false);
	playAction_->setEnabled(true);
	foreach(QWidget * outSigWidg, outputSignalsWidgets_)
	{
		static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
	}

	pixmapVisualTarget_->clear();

	//display the splash screen
	renderingTarget_->showSplash();
	if(deiniting_)
	{
		emit deinitComplete();
		return;
	}
}

void TestViewer::taskListIndexChanged(int)
{
	if(!experiment_)
		return;
	QSharedPointer<Task> task = experiment_->getTaskByName(taskListBox_->currentText());
	if(!task)
		return;
	qobject_cast<PropertyFrame*>(propertyFrame_)->setTopLevelDataStore(task.staticCast<DataStore>());
	loadPropsAction_->setEnabled(true);
	Q_ASSERT(testController_);
	testController_->setTask(taskListBox_->currentText());
}

void TestViewer::operatorClickDetected(QPoint pos)
{
	OperatorClickParameter::addClick(pos);
}

//void TestViewer::zoomChanged(int zoom)
//{
//	if(!pixmapVisualTarget_)
//		return;
//	//Fix zoom so that each tick is worth 3.333%
//	float fixedZoom = float(zoom)/30.0;
//	pixmapVisualTarget_->setZoom(fixedZoom);
//}