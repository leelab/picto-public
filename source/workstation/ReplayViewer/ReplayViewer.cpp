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

#include "ReplayViewer.h"

#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../propertyframe.h"
#include "../../common/engine/XYSignalChannel.h"
#include "../../common/engine/MouseInputPort.h"
#include "../../common/parameter/OperatorClickParameter.h"
#include "../../common/compositor/OutputSignalWidget.h"


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
}

//!Called just before hiding the viewer
void ReplayViewer::deinit()
{
	playbackController_->stop();
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
	loadSessionAction_ = new QAction(tr("&Load Session"),this);
	connect(loadSessionAction_, SIGNAL(triggered()),this, SLOT(loadSession()));
	loadSessionAction_->setEnabled(true);

	runs_ = new QComboBox();
	runs_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	runs_->setToolTip("Select a Run from the Session");
	connect(runs_,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentRun(int)));
	connect(playbackController_.data(),SIGNAL(runsUpdated(QStringList)),this,SLOT(updateRunsList(QStringList)));

	///play/pause/stop actions and toolbar
	playAction_ = new QAction(tr("&Start task"),this);
	playAction_->setIcon(QIcon(":/icons/play.png"));
	playAction_->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
	playAction_->setToolTip("Run (Ctrl+R)");
	connect(playAction_,SIGNAL(triggered()),this, SLOT(play()));

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
	connect(playbackController_.data(),SIGNAL(finishedPlayback()),stopAction_,SLOT(trigger()));
	stopAction_->setEnabled(false);

	progress_ = new ProgressWidget();
	progress_->setMaximum(1);
	progress_->setHighlightColor(0,QColor("#88f"));
	progress_->setHighlightColor(1,QColor("#f88"));
	connect(playbackController_.data(),SIGNAL(timeChanged(double)),this,SLOT(updateTime(double)));
	connect(playbackController_.data(),SIGNAL(loadedTo(double,double)),this,SLOT(updateLoadTimes(double,double)));
	connect(progress_,SIGNAL(valueRequested(double)),this,SLOT(jumpRequested(double)));
	connect(progress_,SIGNAL(userAction(bool)),this,SLOT(userChoosingJump(bool)));

	status_ = new QLabel("Ready");
	connect(playbackController_.data(),SIGNAL(loading(bool)),this,SLOT(loading(bool)));

	////Zoom slider
	//zoomSlider_ = new QSlider;
	//zoomSlider_->setRange(1,59);
	//zoomSlider_->setSingleStep(1);
	//zoomSlider_->setValue(30);
	//zoomSlider_->setOrientation(Qt::Horizontal);
	//connect(zoomSlider_, SIGNAL(sliderMoved(int)), this, SLOT(zoomChanged(int)));

	testToolbar_ = new QToolBar(this);
	testToolbar_->addAction(loadSessionAction_);
	testToolbar_->addWidget(runs_);
	testToolbar_->addAction(playAction_);
	testToolbar_->addAction(pauseAction_);
	testToolbar_->addAction(stopAction_);
	testToolbar_->addSeparator();
	testToolbar_->addWidget(status_);
	testToolbar_->addSeparator();
	//testToolbar_->addWidget(zoomSlider_);
	
	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(testToolbar_);
	toolbarLayout->addStretch();

	QVBoxLayout *stimulusLayout = new QVBoxLayout;
	//Set up the visual target host
	//This exists because QSharedPointer<QWidget> results in multiple delete call, which 
	//gives us memory exceptions.
	visualTargetHost_ = new Picto::VisualTargetHost();
	visualTargetHost_->setVisualTarget(playbackController_->getVisualTarget());
	stimulusLayout->addWidget(visualTargetHost_);

	//Setup Output Signal Widgets
	foreach(QSharedPointer<Picto::VirtualOutputSignalController> cont
		,playbackController_->getOutputSignalControllers())
	{
		cont->moveToThread(QApplication::instance()->thread());
		outputSignalsWidgets_.push_back(new OutputSignalWidget(cont));
		stimulusLayout->addWidget(outputSignalsWidgets_.back());
	}
	stimulusLayout->addWidget(progress_);

	QHBoxLayout *operationLayout = new QHBoxLayout;
	operationLayout->addLayout(stimulusLayout);
	operationLayout->addWidget(speed_);
	operationLayout->addStretch();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	mainLayout->addLayout(operationLayout);
	mainLayout->addStretch(1);
	setLayout(mainLayout);
}

void ReplayViewer::play()
{
	qDebug()<<"Play slot";
	playbackController_->play();
}

void ReplayViewer::pause()
{
	qDebug()<<"Pause slot";
	playbackController_->pause();
}

void ReplayViewer::stop()
{
	qDebug()<<"Stop slot";
	playbackController_->stop();
}

void ReplayViewer::playbackStatusChanged(int status)
{
	switch(status)
	{
	case PlaybackControllerData::Idle:
			progress_->setSliderProgress(0);
			progress_->setHighlightRange(0,0,0);
			progress_->setHighlightRange(1,0,0);
			pauseAction_->setEnabled(false);
			stopAction_->setEnabled(false);
			playAction_->setEnabled(false);
			playbackController_->getRenderingTarget()->showSplash();
		break;
		case PlaybackControllerData::Stopped:
			progress_->setSliderProgress(0);
			progress_->setHighlightRange(0,0,0);
			progress_->setHighlightRange(1,0,0);
			status_->setText("Stopped");
			pauseAction_->setEnabled(true);
			stopAction_->setEnabled(false);
			playAction_->setEnabled(true);
			foreach(QWidget * outSigWidg, outputSignalsWidgets_)
			{
				static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
			}
			playbackController_->getVisualTarget()->clear();
			playbackController_->getRenderingTarget()->showSplash();
		break;
		case PlaybackControllerData::Running:
			status_->setText("Playing");
			pauseAction_->setEnabled(true);
			stopAction_->setEnabled(true);
			playAction_->setEnabled(false);
			foreach(QWidget * outSigWidg, outputSignalsWidgets_)
			{
				static_cast<OutputSignalWidget*>(outSigWidg)->enable(true);
			}
		break;
		case PlaybackControllerData::Paused:
			pauseAction_->setEnabled(false);
			stopAction_->setEnabled(true);
			playAction_->setEnabled(true);
		break;
	}
}

void ReplayViewer::loadSession()
{
	QStringList filenames = QFileDialog::getOpenFileNames(	this,
															tr("Select sessions"),
															".",
															"Picto Sessions(*.sqlite)");
	if(!filenames.size())
		return;
	filenames.sort();
	QString filename = filenames[0];
	//Load Sqlite Database
	filename = filename.replace("\\","/");
	Q_ASSERT(filename.lastIndexOf("/") >=0);
	Q_ASSERT(filename.lastIndexOf(".") >=0);
	QString result = playbackController_->loadSession(filename);
	if(!result.isEmpty())
		QMessageBox::warning(0,"Session could not be loaded",result);

}

void ReplayViewer::updateTime(double time)
{
	double runLength = playbackController_->getRunLength();
	if(runLength < 0)
		runLength = 1;
	progress_->setMaximum(runLength);
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

void ReplayViewer::updateRunsList(QStringList runs)
{
	runs_->setEnabled(false);
	runs_->clear();
	for(int i=0;i<runs.size();i++)
		runs_->addItem(runs[i],i);
	runs_->setEnabled(true);
}

void ReplayViewer::setCurrentRun(int index)
{
	playbackController_->selectRun(index);
	progress_->setSliderProgress(0);
	progress_->setHighlightRange(0,0,0);
	progress_->setHighlightRange(1,0,0);
	jumpDownRequested_ = false;
}

void ReplayViewer::loading(bool load)
{
	if(load)
	{
		lastStatus_ = status_->text();
		status_->setText("Loading");
	}
	else
	{
		status_->setText(lastStatus_);
	}
}

void ReplayViewer::jumpRequested(double time)
{
	if(time < progress_->getHighlightMax(0))
		jumpDownRequested_ = true;
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
		if(playAction_->isEnabled())
			playAction_->trigger();
		pausedFromJump_ = false;
	}
}