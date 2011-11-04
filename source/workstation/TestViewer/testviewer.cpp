#include "testviewer.h"

#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/engine/MouseSignalChannel.h"
#include "../propertyframe.h"
#include "../../common/parameter/OperatorClickParameter.h"


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
	//If we're stopped, load the current experiment.  If we are paused,
	//then we shouldn't load the experiment.
	if(status_ == Stopped)
	{
		experiment_ = pictoData_->getExperiment();
		if(!experiment_)
		{
			QMessageBox msg;
			msg.setText("Failed to load current experiment.");
			msg.exec();
		}
		else
		{
			experiment_->setEngine(engine_);
			static_cast<PropertyFrame*>(propertyFrame_)->setTopLevelDataStore(experiment_.staticCast<DataStore>());
			loadPropsAction_->setEnabled(true);
		}
	}

	generateComboBox();
}

//!Called just before hiding the viewer
void TestViewer::deinit()
{
	//stop the engine running
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
	QSharedPointer<Picto::MouseSignalChannel> mouseChannel(new Picto::MouseSignalChannel(10,visualTargetHost_));
	engine_->addSignalChannel("PositionChannel",mouseChannel);

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
	connect(playAction_,SIGNAL(triggered()),this, SLOT(play()));

	pauseAction_ = new QAction(tr("&Pause task"),this);
	pauseAction_->setIcon(QIcon(":/icons/pause.png"));
	connect(pauseAction_,SIGNAL(triggered()),this, SLOT(pause()));
	pauseAction_->setEnabled(false);

	stopAction_ = new QAction(tr("S&top task"),this);
	stopAction_->setIcon(QIcon(":/icons/stop.png"));
	connect(stopAction_,SIGNAL(triggered()),this, SLOT(stop()));
	stopAction_->setEnabled(false);

	loadPropsAction_ = new QAction(tr("&Load values from Session"),this);
	connect(loadPropsAction_, SIGNAL(triggered()),this, SLOT(LoadPropValsFromFile()));
	loadPropsAction_->setEnabled(false);

	//TaskList combo box
	taskListBox_ = new QComboBox(this);
	taskListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	generateComboBox();

	testToolbar_ = new QToolBar(this);
	testToolbar_->addAction(playAction_);
	testToolbar_->addAction(pauseAction_);
	testToolbar_->addAction(stopAction_);
	
	testToolbar_->addWidget(new QLabel("Task:", this));
	testToolbar_->addSeparator();
	testToolbar_->addWidget(taskListBox_);
	testToolbar_->addAction(loadPropsAction_);

	
	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(testToolbar_);
	toolbarLayout->addStretch();

	QHBoxLayout *operationLayout = new QHBoxLayout;
	propertyFrame_ = new PropertyFrame();
	connect(taskListBox_,SIGNAL(currentIndexChanged(int)),this,SLOT(taskListIndexChanged(int)));
	operationLayout->addWidget(propertyFrame_,Qt::AlignTop);
	operationLayout->addWidget(visualTargetHost_);
	operationLayout->setStretch(0,0);
	operationLayout->addStretch();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	mainLayout->addLayout(operationLayout);
	mainLayout->addStretch(1);
	setLayout(mainLayout);

}

void TestViewer::play()
{
	qDebug()<<"Play slot";

	pauseAction_->setEnabled(true);
	stopAction_->setEnabled(true);
	playAction_->setEnabled(false);

	if(status_ == Stopped)
	{
		status_ = Running;
		if(experiment_)
		{
			experiment_->runTask(taskListBox_->currentText());
		}
		stop();
	}
	else if(status_ == Paused)
	{
		status_ = Running;
		engine_->resume();
	}
}

void TestViewer::pause()
{
	qDebug()<<"Pause slot";
	
	status_ = Paused;
	engine_->pause();

	pauseAction_->setEnabled(false);
	stopAction_->setEnabled(true);
	playAction_->setEnabled(true);

}

void TestViewer::stop()
{
	qDebug()<<"Stop slot";
	engine_->stop();
	status_ = Stopped;

	pauseAction_->setEnabled(false);
	stopAction_->setEnabled(false);
	playAction_->setEnabled(true);

	pixmapVisualTarget_->clear();

	//display the splash screen
	renderingTarget_->showSplash();
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

/*! \brief Resets the current experiment
 *
 *	Rather than writing some extra code to reset all of the scripting environments, 
 *	and return all of the objects to their initial states, we're simply going to 
 *	generate a new experiement from the experiemnt text.  Note that this function
 *	has no error checking (all of that would have been handled when we entered
 *	the trial viewer), so if there are errors in the XML, we simply end up with a
 *	null experiment.
 */
void TestViewer::resetExperiment()
{
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(experiment_->toXml()));
	experiment_ = QSharedPointer<Picto::Experiment>();

	//read until we either see an experiment tag, or the end of the file
	while(xmlReader->name() != "Experiment" && !xmlReader->atEnd()) 
		xmlReader->readNext();

	if(xmlReader->atEnd())
		return;

	experiment_ = QSharedPointer<Picto::Experiment>(Picto::Experiment::Create());

	if(!experiment_->fromXml(xmlReader))
	{
		experiment_ = QSharedPointer<Picto::Experiment>();
	}
	else
	{
		experiment_->setEngine(engine_);
	}
	return;

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
}

void TestViewer::operatorClickDetected(QPoint pos)
{
	OperatorClickParameter::addClick(pos);
}