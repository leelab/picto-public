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
	Viewer(parent),
	status_(Stopped)
{
	//Create the Playback Controller that handles all Experiment playback
	playbackController_ = QSharedPointer<PlaybackController>(new PlaybackController(visualTargetHost_));

	//Setup the user interface
	setupUi();
}

//! Called just before displaying the viewer
void ReplayViewer::init()
{
	
}

//!Called just before hiding the viewer
void ReplayViewer::deinit()
{
	playbackController_->stop();
}

//! \brief Called when the application is about to quit.  Takes care of closing this windows resources
bool ReplayViewer::aboutToQuit()
{
	//Stop the engine running. Otherwise, the 
	//experiment would keep on going even though the window was closed and this
	//process would stick around in the task manager for eternity.
	//
	playbackController_->stop();
	return true;
}

//! Sets up the user interface portions of the GUI
void ReplayViewer::setupUi()
{
	//Load Session Actions
	loadSessionAction_ = new QAction(tr("&Load Session"),this);
	connect(loadSessionAction_, SIGNAL(triggered()),this, SLOT(loadSession()));
	loadSessionAction_->setEnabled(true);

	///play/pause/stop actions and toolbar
	playAction_ = new QAction(tr("&Start task"),this);
	playAction_->setIcon(QIcon(":/icons/play.png"));
	playAction_->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
	playAction_->setToolTip("Run (Ctrl+R)");
	connect(playAction_,SIGNAL(triggered()),this, SLOT(play()));

	speed_ = new QDoubleSpinBox();
	speed_->setValue(1.0);
	speed_->setMinimum(0.001);
	speed_->setMaximum(100);
	speed_->setDecimals(3);
	speed_->setSingleStep(.1);
	connect(speed_,SIGNAL(editingFinished()),this,SLOT(setRunSpeed()));


	pauseAction_ = new QAction(tr("&Pause task"),this);
	pauseAction_->setIcon(QIcon(":/icons/pause.png"));
	pauseAction_->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_P));
	pauseAction_->setToolTip("Pause (Ctrl+P)");
	connect(pauseAction_,SIGNAL(triggered()),this, SLOT(pause()));
	pauseAction_->setEnabled(false);

	stopAction_ = new QAction(tr("S&top task"),this);
	stopAction_->setIcon(QIcon(":/icons/stop.png"));
	stopAction_->setToolTip("Stop");
	connect(stopAction_,SIGNAL(triggered()),this, SLOT(stop()));
	stopAction_->setEnabled(false);

	clock_ = new QLabel("0.00");
	connect(playbackController_.data(),SIGNAL(timeChanged(double)),this,SLOT(updateTime(double)));

	loading_ = new QLabel("Loading");
	loading_->setVisible(false);
	connect(playbackController_.data(),SIGNAL(loading(bool)),loading_,SLOT(setVisible(bool)));

	////Zoom slider
	//zoomSlider_ = new QSlider;
	//zoomSlider_->setRange(1,59);
	//zoomSlider_->setSingleStep(1);
	//zoomSlider_->setValue(30);
	//zoomSlider_->setOrientation(Qt::Horizontal);
	//connect(zoomSlider_, SIGNAL(sliderMoved(int)), this, SLOT(zoomChanged(int)));

	testToolbar_ = new QToolBar(this);
	testToolbar_->addAction(loadSessionAction_);
	testToolbar_->addAction(playAction_);
	testToolbar_->addWidget(speed_);
	testToolbar_->addAction(pauseAction_);
	testToolbar_->addAction(stopAction_);
	testToolbar_->addSeparator();
	testToolbar_->addWidget(clock_);
	testToolbar_->addWidget(loading_);
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
		outputSignalsWidgets_.push_back(new OutputSignalWidget(cont));
		stimulusLayout->addWidget(outputSignalsWidgets_.back());
	}

	QHBoxLayout *operationLayout = new QHBoxLayout;
	operationLayout->addLayout(stimulusLayout);
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

	pauseAction_->setEnabled(true);
	stopAction_->setEnabled(true);
	playAction_->setEnabled(false);
	foreach(QWidget * outSigWidg, outputSignalsWidgets_)
	{
		static_cast<OutputSignalWidget*>(outSigWidg)->enable(true);
	}

	playbackController_->play();
}

void ReplayViewer::pause()
{
	qDebug()<<"Pause slot";

	pauseAction_->setEnabled(false);
	stopAction_->setEnabled(true);
	playAction_->setEnabled(true);

	playbackController_->pause();
}

void ReplayViewer::stop()
{
	qDebug()<<"Stop slot";

	pauseAction_->setEnabled(false);
	stopAction_->setEnabled(false);
	playAction_->setEnabled(true);
	foreach(QWidget * outSigWidg, outputSignalsWidgets_)
	{
		static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
	}

	playbackController_->stop();
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
	playbackController_->loadSession(filename);
}

void ReplayViewer::updateTime(double time)
{
	clock_->setText(QString::number(time));
}

void ReplayViewer::setRunSpeed()
{
	playbackController_->setRunSpeed(speed_->value());

}