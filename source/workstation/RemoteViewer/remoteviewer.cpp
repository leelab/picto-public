#include "remoteviewer.h"

#include "../common/globals.h"
#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/engine/XYSignalChannel.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "../../common/storage/BehavioralDataUnitPackage.h"
#include "../../common/storage/StateDataUnit.h"
#include "../../common/statemachine/statemachine.h"
#include "../../common/stimuli/cursorgraphic.h"
#include "../propertyframe.h"



#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QComboBox>
#include <QLabel>
#include <QXmlStreamReader>
#include <QTimer>
#include <QTime>
#include <QSharedPointer>
#include <QMenu>
#include <QCloseEvent>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFileDialog>
#include <QSlider>
#include <QSpinBox>
#include <QLineEdit>
#include <QtConcurrentRun>
#include "../../common/memleakdetect.h"
using namespace Picto;

#define COMPONENT_UPDATE_PERIOD 500 
#define SPECIAL_STATUS_PERIOD 5000

RemoteViewer::RemoteViewer(QWidget *parent) :
	Viewer(parent),
	serverChannel_(0),
	activeExperiment_(0),
	statusBar_(NULL),
	isAuthorized_(false),
	propertyFrame_(NULL)
{
	setupServerChannel();
	setupEngine();
	setupUi();

	QString dbName = "PictoWorkstation";
	dbName = dbName.toLower();
	QSqlDatabase configDb = QSqlDatabase::addDatabase("QSQLITE",dbName);
	configDb.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + dbName + ".config");
	configDb.open();

	QSqlQuery query(configDb);
	if(!configDb.tables().contains("workstationinfo"))
	{
		query.exec("CREATE TABLE workstationinfo (key TEXT, value TEXT)");
		observerId_ = QUuid::createUuid();
		query.prepare("INSERT INTO workstationinfo (key,value) VALUES ('id',:id)");
		query.bindValue(":id",QString(observerId_));
		query.exec();
	}
	query.exec("SELECT value FROM workstationinfo WHERE key='id'");
	bool rc = query.next();
	Q_ASSERT(rc);
	observerId_ = QUuid(query.value(0).toString());

	currState_ = InitState;
	stateTrigger_ = Disconnected;
	engineTrigger_ = NoEngineTrigger;
	stateUpdateTimer_ = new QTimer(this);
	stateUpdateTimer_->setInterval(0);
	connect(stateUpdateTimer_,SIGNAL(timeout()),this,SLOT(updateState()));
	engineUpdateTimer_ = new QTimer(this);
	engineUpdateTimer_->setInterval(0);
	connect(engineUpdateTimer_,SIGNAL(timeout()),this,SLOT(updateEngine()));
	updatingState_ = false;
	refreshSplash_ = true;
}

RemoteViewer::~RemoteViewer()
{
}
/*! \brief The remote viewer is implemented as a state machine.  This is the main state machine run loop.
 *	Each time this function is called, it checks for state change triggers.  If they have occured, the
 *	state is changed, the new state's enter state function is called, and the new states runState function
 *	is called continuously until the next state change.
 *	Note: If we were to actually start an experiment running from inside this function, the function could not be
 *	entered again until the experiment ended.  For this reason we have an external engine state machine
 *	that runs separately from this one.  If the engine is waiting for a state change to occur, this 
 *	function exits without doing anything.  In this way, any actions to start or stop the engine from
 *	within this function are guaranteed to occur before the next time the function operates.

 */
void RemoteViewer::updateState()
{
	if(engineTrigger_ != NoEngineTrigger)
	{	//If the engine has been triggered to start or stop, we must always wait for it
		//to do that before updating our own state
		return;
	}
	updatingState_ = true;
	updateComponentLists();
	QString directorID = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
	ComponentStatus remoteStatus;
	if(stateTrigger_ != NoViewerTrigger)
		remoteStatus = directorStatus(directorID);
	ViewerState nextState = currState_;
	switch(currState_)
	{
	case InitState:
		nextState = WaitForConnect;
		break;
	case WaitForConnect:
		switch(stateTrigger_)
		{
		case Connected:
			nextState = WaitForJoin;
			break;
		}
		break;
	case WaitForJoin:
		switch(stateTrigger_)
		{
		case Disconnected:
			nextState = WaitForConnect;
			break;
		case StartSessionRequest:
			nextState = CreatingSession;
			break;
		case JoinSessionRequest:
			nextState = JoiningSession;
			break;
		}
		break;
	case JoiningSession:
		switch(stateTrigger_)
		{
		case Disconnected:
			nextState = WaitForConnect;
			break;
		case SessionStopped:
			nextState = StoppedSession;
			break;
		case SessionPaused:
			nextState = PausedSession;
			break;
		case SessionRunning:
			nextState = RunningSession;
			break;
		case DisjoinSessionRequest:
			nextState = WaitForJoin;
			break;
		}
		break;
	case CreatingSession:
		switch(stateTrigger_)
		{
		case Disconnected:
			nextState = WaitForConnect;
			break;
		case SessionStarted:
			nextState = JoiningSession;
			break;
		case SessionDidntStart:
			nextState = WaitForJoin;
		}
		break;
	case StoppedSession:
		switch(stateTrigger_)
		{
		case Disconnected:
			nextState = WaitForConnect;
			break;
		case DisjoinSessionRequest:
			nextState = WaitForJoin;
			break;
		case EndSessionRequest:
			nextState = EndingSession;
			break;
		case SessionRunning:
			nextState = RunningSession;
			break;
		}
		break;
	case PausedSession:
		switch(stateTrigger_)
		{
		case Disconnected:
			nextState = WaitForConnect;
			break;
		case DisjoinSessionRequest:
			nextState = WaitForJoin;
			break;
		case SessionRunning:
			nextState = RunningSession;
			break;
		case SessionStopped:
			nextState = StoppedSession;
			break;
		}
		break;
	case RunningSession:
		switch(stateTrigger_)
		{
		case Disconnected:
			nextState = WaitForConnect;
			break;
		case DisjoinSessionRequest:
			nextState = WaitForJoin;
			break;
		case SessionPaused:
			nextState = PausedSession;
			break;
		case SessionStopped:
			nextState = StoppedSession;
			break;
		}
		break;
	case EndingSession:
		switch(stateTrigger_)
		{
		case Disconnected:
			nextState = WaitForConnect;
			break;
		case SessionEnded:
			nextState = WaitForJoin;
			break;
		}
		break;
	}
	stateTrigger_ = NoViewerTrigger;
	if(nextState != currState_)
	{
		currState_ = nextState;
		enterState();
	}
	runState();
	if(stateTrigger_ != NoViewerTrigger)
		endState();
	updateStatus();
	updatingState_ = false;
}

/*! \brief This is the main function for the engine's simple state machine.
 *	As described in updateState, we need a separate state machine for engine
 *	activity so that starting an experiment doesn't effectively stop the operation
 *	of the main state machine.  The function must never be called from within
 *	updateState() to prevent that same problem, which is why it returns right away
 *	if an updateState() call is in progress.  The function waits for a startEngine
 *	trigger, then starts the engine at the current state of the pictoDirector's 
 *	experiment.  When an experiment stops, the function checks for a StopEngine trigger
 *	which implies that the main state machine has triggered for this engine to stop.
 *	If the trigger is found then the engine will no longer run until the next StartEngine
 *	trigger.  If the StopTrigger isn't found, the engine will start up again the next
 *	time the updateEngine function is called by its timer.  With this system in place,
 *	even if something goes wrong and the engine exits before a task is done, it will just
 *	restart it.
 */
void RemoteViewer::updateEngine()
{
	//We don't want to start running the engine from somewhere within the updateState() function
	//or else we'll block that function from continuing to operate.  Calling updateState() from 
	//within this function somewhere is okay because updateState calls don't block for long periods
	//of time the way the engine does.
	if(updatingState_)
		return;
	if(engineTrigger_ == StopEngine)
	{
		engineTrigger_ = NoEngineTrigger;
		return;
	}
	if(engineTrigger_ == StartEngine)
	{
		engine_->resetLastTimeStateDataRequested();
		if(!engine_->updateCurrentStateFromServer())
		{
			setStatus("Server failed to respond to Current State request",true);
			return;
		}
		QString fullPath = engine_->getServerPathUpdate();
		int lastSepIndex = fullPath.lastIndexOf("::");
		QString machinePath = fullPath.left(lastSepIndex);
		QString initState = fullPath.mid(lastSepIndex+2);
		if(machinePath.isEmpty())
			return;
		
		QStringList initStateMachinePath = machinePath.split("::");
		QString taskName = initStateMachinePath.first();

		//Update the task list box
		int taskIdx = taskListBox_->findText(taskName);
		Q_ASSERT_X(taskIdx >= 0,"RemoteViewer::joinSession", "Task name not found in out list of experiments.");
		taskListBox_->setCurrentIndex(taskIdx);
		
		//Set ouselves up so we are synched with the remote director
		Picto::StateMachine::resetSlaveElements();

		////////////TESTING
		qDebug()<<"JumpToState "<<initStateMachinePath<<initState;

		if(!activeExperiment_->jumpToState(initStateMachinePath, initState))
		{
			////////TESTING
			Q_ASSERT(false);

			setStatus("Failed to jump into the state machine correctly",true);
			return;
		}

		if(!activeExperiment_->jumpToState(initStateMachinePath, initState))
		{
			////////TESTING
			Q_ASSERT(false);

			setStatus("Failed to jump into the state machine correctly",true);
			return;
		}
		taskName = initStateMachinePath.first();
		engineTrigger_ = NoEngineTrigger;
		//WARNING:  Nothing after this line will be processed until the task is finished running
		activeExperiment_->runTask(taskName.simplified().remove(' '));
		if(engineTrigger_ == StopEngine)
		{
			engineTrigger_ = NoEngineTrigger;
			refreshSplash_ = true;
		}
		else
		{	//If there's no stopEngine trigger, either the experiment failed, or the task changed.
			//Either way we need to try to start again.
			engineTrigger_ = StartEngine;
		}
	}
}

/*! \brief This function is called once per state machine loop.
 * This function is used to run periodic state specific checks and check 
 * for new state transitions.  A state signifies a new state transition by 
 * setting stateTrigger_ to a new value.  This trigger will then be used to 
 * select the next state in updateState()
 */
void RemoteViewer::runState()
{
	QString directorID = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
	ComponentStatus remoteStatus = directorStatus(directorID);
	switch(currState_)
	{
	case WaitForConnect:
		activeExpName_->setText(experiment_->getName()); //In case operator loads new file
		if(assureChannelConnections())
			stateTrigger_ = Connected;
		break;
	case WaitForJoin:
		activeExpName_->setText(experiment_->getName()); //In case operator loads new file
		if(connectAction_->isChecked())
		{
				
			if(remoteStatus == Idle)
				stateTrigger_ = StartSessionRequest;
			if(remoteStatus > Ending)
				stateTrigger_ = JoinSessionRequest;
		}
		break;
	case JoiningSession:
		activeExpName_->setText(experiment_->getName()); //In case operator loads new file
		if(joinSession())
		{
			if(remoteStatus == Running)
				stateTrigger_ = SessionRunning;
			else if(remoteStatus == Paused)
				stateTrigger_ = SessionPaused;
			else
				stateTrigger_ = SessionStopped;
		}
		if(remoteStatus < Stopped)
		{
			disjoinSession();
			stateTrigger_ = DisjoinSessionRequest;
		}
		break;
	case CreatingSession:
		activeExpName_->setText(experiment_->getName()); //In case operator loads new file
		if(startSession())
			stateTrigger_ = SessionStarted;
		else
			stateTrigger_ = SessionDidntStart;
		break;
	case StoppedSession:
		if(remoteStatus == Running)
			stateTrigger_ = SessionRunning;
		if(!connectAction_->isChecked())
		{
			if(!isAuthorized_)
			{
				disjoinSession();
				stateTrigger_ = DisjoinSessionRequest;
			}
			else
			{
				int	r = QMessageBox::warning(this,Picto::Names->workstationAppName,
					tr("You are disconnecting from a session that is not currently running an experiment.\n"
					"Would you like to end the session as well?"),
					   QMessageBox::Yes|QMessageBox::No);

				if(r == QMessageBox::Yes)
				{
					stateTrigger_ = EndSessionRequest;
				}
				else
				{
					disjoinSession();
					stateTrigger_ = DisjoinSessionRequest;
				}
			}
		}
		if(remoteStatus < Stopped)
		{
			disjoinSession();
			stateTrigger_ = DisjoinSessionRequest;
		}
		if(refreshSplash_)
		{
			renderingTarget_->showSplash();
			refreshSplash_ = false;
		}
		break;
	case PausedSession:
		if(remoteStatus == Running)
			stateTrigger_ = SessionRunning;
		if(remoteStatus == Stopped)
			stateTrigger_ = SessionStopped;
		if(!connectAction_->isChecked() || (remoteStatus < Stopped))
		{
			disjoinSession();
			stateTrigger_ = DisjoinSessionRequest;
		}
		if(stateTrigger_ != NoViewerTrigger)
			stopExperiment();

		break;
	case RunningSession:
		if(remoteStatus == Paused)
			stateTrigger_ = SessionPaused;
		if(remoteStatus == Stopped)
			stateTrigger_ = SessionStopped;
		if(!connectAction_->isChecked() || (remoteStatus < Stopped))
		{
			disjoinSession();
			stateTrigger_ = DisjoinSessionRequest;
		}
		if(stateTrigger_ != NoViewerTrigger)
			stopExperiment();
		break;
	case EndingSession:
		if(endSession() || remoteStatus < Stopped)
			disjoinSession();
			stateTrigger_ = SessionEnded;
		break;
	}
	if(!assureChannelConnections())
	{
		stateTrigger_ = Disconnected;
	}
}

/*! \brief The function is called when a state is ending and a new state will start on the next updateState call.
 *	A state may have different ending actions depending on the trigger that it is ending
 *	with.  This function is called after that trigger has been determined.  
 *	It is important to note that actions on the engine by this function (ie. stop)
 *	will always be completed before the next call to updateState().
 */
void RemoteViewer::endState()
{
	switch(currState_)
	{
	case WaitForConnect:
		break;
	case WaitForJoin:
		break;
	case JoiningSession:
		if(	(stateTrigger_ == DisjoinSessionRequest)
			|| (stateTrigger_ == Disconnected)	)
			disjoinSession();
		break;
	case CreatingSession:
		break;
	case StoppedSession:
		if(	(stateTrigger_ == DisjoinSessionRequest)
			|| (stateTrigger_ == Disconnected)	)
			disjoinSession();
		break;
	case PausedSession:
		stopExperiment();
		if(	(stateTrigger_ == DisjoinSessionRequest)
			|| (stateTrigger_ == Disconnected)	)
			disjoinSession();
		break;
	case RunningSession:
		stopExperiment();
		if(	(stateTrigger_ == DisjoinSessionRequest)
			|| (stateTrigger_ == Disconnected)	)
			disjoinSession();
		break;
	case EndingSession:
		if(	(stateTrigger_ == SessionEnded)
			|| (stateTrigger_ == Disconnected)	)
			disjoinSession();
		break;
	}
}

/*! \brief This function is called once each time a new state is entered.
 *	The function is used to update the UI such that it logically reflects
 *	the actions available in the new state.
 */
void RemoteViewer::enterState()
{
	switch(currState_)
	{
	case WaitForConnect:
		setStatus(tr("Looking for Picto Server"));
		playAction_->setEnabled(false);
		stopAction_->setEnabled(false);
		pauseAction_->setEnabled(false);
		rewardAction_->setEnabled(false);
		rewardQuantity_->setEnabled(false);
		connectAction_->setEnabled(false);
		connectAction_->setIcon(QIcon(":/icons/disconnected.png"));
		connectAction_->setChecked(false);
		directorListBox_->setEnabled(false);
		proxyListBox_->setEnabled(false);
		passwordEdit_->setEnabled(false);
		taskListBox_->setEnabled(false);
		zoomSlider_->setEnabled(false);
		taskListBox_->setEnabled(false);
		propertyFrame_->setEnabled(false);
		loadPropsAction_->setEnabled(false);
		activeExpName_->setText(experiment_->getName());
		renderingTarget_->showSplash();
		taskListBox_->clear();
		qobject_cast<PropertyFrame*>(propertyFrame_)->clearProperties();
		break;
	case WaitForJoin:
		setStatus(tr("Connected to Picto Server"));
		playAction_->setEnabled(false);
		stopAction_->setEnabled(false);
		pauseAction_->setEnabled(false);
		rewardAction_->setEnabled(false);
		rewardQuantity_->setEnabled(false);
		connectAction_->setEnabled(true);
		connectAction_->setIcon(QIcon(":/icons/disconnected.png"));
		connectAction_->setChecked(false);
		directorListBox_->setEnabled(true);
		proxyListBox_->setEnabled(true);
		passwordEdit_->setEnabled(true);
		taskListBox_->setEnabled(false);
		zoomSlider_->setEnabled(false);
		updateComponentLists(true);
		taskListBox_->setEnabled(false);
		propertyFrame_->setEnabled(false);
		loadPropsAction_->setEnabled(false);
		activeExpName_->setText(experiment_->getName());
		renderingTarget_->showSplash();
		taskListBox_->clear();
		qobject_cast<PropertyFrame*>(propertyFrame_)->clearProperties();
		break;
	case JoiningSession:
		setStatus(tr("Joining active session"));
		playAction_->setEnabled(false);
		stopAction_->setEnabled(false);
		pauseAction_->setEnabled(false);
		rewardAction_->setEnabled(false);
		rewardQuantity_->setEnabled(false);
		connectAction_->setEnabled(true);
		connectAction_->setIcon(QIcon(":/icons/disconnected.png"));
		connectAction_->setChecked(true);
		directorListBox_->setEnabled(false);
		proxyListBox_->setEnabled(false);
		passwordEdit_->setEnabled(false);
		taskListBox_->setEnabled(false);
		zoomSlider_->setEnabled(false);
		taskListBox_->setEnabled(false);
		propertyFrame_->setEnabled(false);
		loadPropsAction_->setEnabled(false);
		activeExpName_->setText(experiment_->getName());
		renderingTarget_->showSplash();
		taskListBox_->clear();
		qobject_cast<PropertyFrame*>(propertyFrame_)->clearProperties();
		break;
	case CreatingSession:
		setStatus(tr("Starting new session"));
		playAction_->setEnabled(false);
		stopAction_->setEnabled(false);
		pauseAction_->setEnabled(false);
		rewardAction_->setEnabled(false);
		rewardQuantity_->setEnabled(false);
		connectAction_->setEnabled(true);
		connectAction_->setIcon(QIcon(":/icons/disconnected.png"));
		connectAction_->setChecked(true);
		directorListBox_->setEnabled(false);
		proxyListBox_->setEnabled(false);
		passwordEdit_->setEnabled(false);
		taskListBox_->setEnabled(false);
		zoomSlider_->setEnabled(false);
		taskListBox_->setEnabled(false);
		propertyFrame_->setEnabled(false);
		loadPropsAction_->setEnabled(false);
		activeExpName_->setText(experiment_->getName());
		renderingTarget_->showSplash();
		taskListBox_->clear();
		qobject_cast<PropertyFrame*>(propertyFrame_)->clearProperties();
		break;
	case StoppedSession:
		setStatus(tr("In Session: Stopped"));
		playAction_->setEnabled(isAuthorized_);
		stopAction_->setEnabled(false);
		pauseAction_->setEnabled(false);
		rewardAction_->setEnabled(isAuthorized_);
		rewardQuantity_->setEnabled(isAuthorized_);
		connectAction_->setEnabled(true);
		connectAction_->setIcon(QIcon(":/icons/connected.png"));
		connectAction_->setChecked(true);
		directorListBox_->setEnabled(false);
		proxyListBox_->setEnabled(false);
		passwordEdit_->setEnabled(false);
		taskListBox_->setEnabled(isAuthorized_);
		zoomSlider_->setEnabled(true);
		loadPropsAction_->setEnabled(true);
		activeExpName_->setText(activeExperiment_->getName());
		propertyFrame_->setEnabled(isAuthorized_);
		generateTaskList();
		break;
	case PausedSession:
		setStatus(tr("In Session: Paused"));
		playAction_->setEnabled(isAuthorized_);
		stopAction_->setEnabled(isAuthorized_);
		pauseAction_->setEnabled(false);
		rewardAction_->setEnabled(isAuthorized_);
		rewardQuantity_->setEnabled(isAuthorized_);
		connectAction_->setEnabled(true);
		connectAction_->setIcon(QIcon(":/icons/connected.png"));
		connectAction_->setChecked(true);
		directorListBox_->setEnabled(false);
		proxyListBox_->setEnabled(false);
		passwordEdit_->setEnabled(false);
		taskListBox_->setEnabled(false);
		zoomSlider_->setEnabled(true);
		taskListBox_->setEnabled(false);
		loadPropsAction_->setEnabled(true);
		activeExpName_->setText(activeExperiment_->getName());
		propertyFrame_->setEnabled(isAuthorized_);
		generateTaskList();
		syncExperiment();
		break;
	case RunningSession:
		setStatus(tr("In Session: Running"));
		playAction_->setEnabled(false);
		stopAction_->setEnabled(isAuthorized_);
		pauseAction_->setEnabled(isAuthorized_);
		rewardAction_->setEnabled(isAuthorized_);
		rewardQuantity_->setEnabled(isAuthorized_);
		connectAction_->setEnabled(true);
		connectAction_->setIcon(QIcon(":/icons/connected.png"));
		connectAction_->setChecked(true);
		directorListBox_->setEnabled(false);
		proxyListBox_->setEnabled(false);
		passwordEdit_->setEnabled(false);
		taskListBox_->setEnabled(false);
		zoomSlider_->setEnabled(true);
		taskListBox_->setEnabled(false);
		loadPropsAction_->setEnabled(true);
		activeExpName_->setText(activeExperiment_->getName());
		propertyFrame_->setEnabled(isAuthorized_);
		generateTaskList();
		syncExperiment();
		break;
	case EndingSession:
		setStatus(tr("Ending Session"));
		playAction_->setEnabled(false);
		stopAction_->setEnabled(false);
		pauseAction_->setEnabled(false);
		rewardAction_->setEnabled(false);
		rewardQuantity_->setEnabled(false);
		connectAction_->setEnabled(true);
		connectAction_->setIcon(QIcon(":/icons/connected.png"));
		connectAction_->setChecked(false);
		directorListBox_->setEnabled(false);
		proxyListBox_->setEnabled(false);
		passwordEdit_->setEnabled(false);
		taskListBox_->setEnabled(false);
		zoomSlider_->setEnabled(false);
		taskListBox_->setEnabled(false);
		propertyFrame_->setEnabled(false);
		activeExpName_->setText(activeExperiment_->getName());
		renderingTarget_->showSplash();
		taskListBox_->clear();
		qobject_cast<PropertyFrame*>(propertyFrame_)->clearProperties();
		break;
	}
}

//! \brief Called just before displaying the viewer
void RemoteViewer::init()
{
	experiment_ = pictoData_->getExperiment();
	if(!experiment_)
	{
		QMessageBox msg;
		msg.setText("Failed to load current experiment.");
		msg.exec();
	}
	if(currState_ == InitState)
	{	//The remote viewer was just created.  Initialize it by running updateState once.
		updateState();
	}
	//This timer assures that we don't update component lists more often than necessary
	componentListsTimer_.restart();
	//This timer triggers each new run of updateState()
	stateUpdateTimer_->start();
	//This timer triggers each new run of updateEngine()
	engineUpdateTimer_->start();
}

//! \brief Called just before hiding the viewer
void RemoteViewer::deinit()
{
	//stop the engine running
	stopExperiment();

	//Stop the timers so that our state update functions won't get called anymore.
	stateUpdateTimer_->stop();
	engineUpdateTimer_->stop();
}

//! \brief Called when the application is about to quit.  Takes care of closing this windows resources
bool RemoteViewer::aboutToQuit()
{
	//Calling deinit is important because it stops the experiment.  Otherwise, the 
	//experiment would keep on going even though the window was closed and this
	//process would stick around in the task manager for eternity.
	deinit();
	return true;
}

//! \brief Initializes the engine with all of the appropriate pieces for testing
void RemoteViewer::setupEngine()
{
	//set up the engine
	engine_ = QSharedPointer<Picto::Engine::PictoEngine>(new Picto::Engine::PictoEngine);
	engine_->setExclusiveMode(false);
	engine_->setSlaveMode(true,engineSlaveChannel_);
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

	//set up signal channel
	QSharedPointer<Picto::SignalChannel> signalChannel(new Picto::XYSignalChannel());
	engine_->addSignalChannel("PositionChannel",signalChannel);

	//Set up event code generator
	QSharedPointer<Picto::EventCodeGenerator> nullGenerator;
	nullGenerator = QSharedPointer<Picto::EventCodeGenerator>(new Picto::NullEventCodeGenerator());
	engine_->setEventCodeGenerator(nullGenerator);

	//set up reward controller
	QSharedPointer<Picto::RewardController> rewardController;
	rewardController = QSharedPointer<Picto::RewardController>(new Picto::AudioRewardController());
	engine_->setRewardController(rewardController);

	refreshSplash_ = true;
}

//! \brief Sets up the user interface portions of the GUI
void RemoteViewer::setupUi()
{
	//----- Task Action -----
	playAction_ = new QAction(tr("P&lay"),this);
	playAction_->setIcon(QIcon(":/icons/play.png"));
	connect(playAction_,SIGNAL(triggered()),this, SLOT(playAction()));

	pauseAction_ = new QAction(tr("&Pause"),this);
	pauseAction_->setIcon(QIcon(":/icons/pause.png"));
	connect(pauseAction_,SIGNAL(triggered()),this, SLOT(pause()));

	stopAction_ = new QAction(tr("&Stop"),this);
	stopAction_->setIcon(QIcon(":/icons/stop.png"));
	connect(stopAction_,SIGNAL(triggered()),this, SLOT(stop()));

	rewardAction_ = new QAction(tr("&Reward"), this);
	rewardAction_->setIcon(QIcon(":/icons/reward.png"));
	rewardAction_->setShortcut(QKeySequence(Qt::Key_F1));
	rewardAction_->setToolTip("Reward (F1)");
	connect(rewardAction_, SIGNAL(triggered()),this, SLOT(reward()));
	rewardChannel_ = 1;

	rewardQuantity_ = new QSpinBox();
	rewardQuantity_->setValue(60);
	rewardQuantity_->setMinimum(1);
	rewardQuantity_->setMaximum(999999);
	rewardQuantity_->setSingleStep(10);

	loadPropsAction_ = new QAction(tr("&Load Task Properties from Session"),this);
	loadPropsAction_->setIcon(QIcon(":/icons/loadvalues.png"));
	connect(loadPropsAction_, SIGNAL(triggered()),this, SLOT(LoadPropValsFromFile()));
	loadPropsAction_->setEnabled(false);

	//TaskList combo box
	taskListBox_ = new QComboBox;
	taskListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	//The toolbar
	toolBar_ = new QToolBar;

	toolBar_->addAction(playAction_);
	toolBar_->addAction(pauseAction_);
	toolBar_->addAction(stopAction_);
	toolBar_->addAction(rewardAction_);
	toolBar_->addWidget(rewardQuantity_);
	toolBar_->addSeparator();
	toolBar_->addWidget(new QLabel("Task: ", this));
	toolBar_->addWidget(taskListBox_);
	toolBar_->addAction(loadPropsAction_);
	toolBar_->addSeparator();

	//----- Connection Actions -----
	connectAction_ = new QAction(tr("&Connect"), this);
	connectAction_->setCheckable(true);
	connectAction_->setIcon(QIcon(":/icons/disconnected.png"));
	//connect(connectAction_, SIGNAL(triggered(bool)), this, SLOT(changeConnectionState(bool)));

	//Password Line Edit
	passwordEdit_ = new QLineEdit();
	passwordEdit_->setEnabled(false);

	directorListBox_ = new QComboBox;
	directorListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	proxyListBox_ = new QComboBox;
	proxyListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	proxyListBox_->addItem("No Proxy",QUuid().toString());
	//updateComponentLists();

	//Active Experiment name
	activeExpName_ = new QLabel();

	//Zoom slider
	zoomSlider_ = new QSlider;
	zoomSlider_->setRange(2,20);
	zoomSlider_->setSingleStep(1);
	zoomSlider_->setPageStep(2);
	zoomSlider_->setValue(20);
	zoomSlider_->setOrientation(Qt::Horizontal);
	connect(zoomSlider_, SIGNAL(valueChanged(int)), this, SLOT(zoomChanged(int)));

	zoomPercentage_ = new QLabel("100%");

	toolBar_->addAction(connectAction_);
	toolBar_->addWidget(new QLabel(Picto::Names->directorAppName + ": ", this));
	toolBar_->addWidget(directorListBox_);
	toolBar_->addWidget(new QLabel(Picto::Names->proxyServerAppName + ": ", this));
	toolBar_->addWidget(proxyListBox_);
	toolBar_->addWidget(new QLabel("Session Key:"));
	toolBar_->addWidget(passwordEdit_);

	//------ Main layout -----------
	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(toolBar_);
	toolbarLayout->addStretch();

	statusBar_ = new QLabel();

	propertyFrame_ = new PropertyFrame();
	propertyFrame_->setEnabled(false);
	connect(taskListBox_,SIGNAL(currentIndexChanged(int)),this,SLOT(taskListIndexChanged(int)));
	connect(propertyFrame_,SIGNAL(parameterMessageReady(QSharedPointer<Property>)),this,SLOT(parameterMessageReady(QSharedPointer<Property>)));
	QHBoxLayout* activeExpLayout = new QHBoxLayout();
	activeExpLayout->addWidget(new QLabel("Active Experiment: "));
	activeExpLayout->addWidget(activeExpName_);
	
	QHBoxLayout* zoomLayout = new QHBoxLayout();
	zoomLayout->addWidget(new QLabel("Zoom: ", this));
	zoomLayout->addWidget(zoomSlider_);
	zoomLayout->addWidget(zoomPercentage_);
		
	QVBoxLayout *leftPane = new QVBoxLayout;
	leftPane->addLayout(activeExpLayout);
	leftPane->addLayout(zoomLayout);
	leftPane->addWidget(propertyFrame_,Qt::AlignTop);

	QHBoxLayout *operationLayout = new QHBoxLayout;
	operationLayout->addLayout(leftPane);
	operationLayout->addWidget(visualTargetHost_);
	operationLayout->setStretch(0,0);
	operationLayout->addStretch();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	mainLayout->addLayout(operationLayout);
	mainLayout->addStretch(1);
	mainLayout->addWidget(statusBar_);
	setLayout(mainLayout);

}

//! \brief Creates the server and engineSlaveChannels for UI actions affecting the server, and experimental state requests respectively.
void RemoteViewer::setupServerChannel()
{
	serverChannel_ = new Picto::CommandChannel(observerId_,"WORKSTATION",this);
	engineSlaveChannel_ = new Picto::CommandChannel(observerId_,"WORKSTATION",this);
}

//! \brief Called when the play button is pressed
void RemoteViewer::playAction()
{
	if(currState_ == StoppedSession)
		play();
	else
		resume();
}

//! \brief Tells the server to start running a task
void RemoteViewer::play()
{
	QString modifiedTaskName = taskListBox_->currentText();
	modifiedTaskName = modifiedTaskName.simplified().remove(' ');
	sendTaskCommand("start:"+modifiedTaskName);
}

//! \brief Tells the server to resume running the current task from its paused state
void RemoteViewer::resume()
{
	sendTaskCommand("resume");
}

//! \brief Tells the server to pause the current task
void RemoteViewer::pause()
{
	sendTaskCommand("pause");
}

//! Tells the server to stop running the current task
void RemoteViewer::stop()
{
	sendTaskCommand("stop");
}

//! \brief Requests that a new reward be given to the user.
void RemoteViewer::reward()
{
	sendTaskCommand(QString("reward:%1").arg(rewardChannel_),QString::number(rewardQuantity_->value()));
}

//! \brief Loads property values from a previous session to replace those in the current experiment.
void RemoteViewer::LoadPropValsFromFile()
{
	QString filename = QFileDialog::getOpenFileName(this,
			tr("Load Properties From Session"),".","Sqlite files (*.sqlite)");
	if(propertyFrame_)
		static_cast<PropertyFrame*>(propertyFrame_)->updatePropertiesFromFile(filename);
}

//! \brief Called whenever an experiment paramter is changed.  Sends a request to the server to change that parameter value in the experiment.
void RemoteViewer::parameterMessageReady(QSharedPointer<Property> changedProp)
{
	Q_ASSERT(changedProp);
	QString path = changedProp->getPath();
	QString name = changedProp->getName();
	if(changedProp->getAssetId()<0)
		return;
	sendTaskCommand(QString("parameter:%1").arg(QString::number(changedProp->getAssetId())),changedProp->toUserString());
}

//! \brief Tells the server that the operator just clicked on the main window, and where the click occured.
void RemoteViewer::operatorClickDetected(QPoint pos)
{
	if(currState_ == RunningSession)
		sendTaskCommand(QString("click:%1,%2").arg(pos.x()).arg(pos.y()));
}

//! \brief Adjusts the experimental zoom.
void RemoteViewer::zoomChanged(int zoom)
{
	if(!pixmapVisualTarget_)
		return;
	//Fix zoom so that each tick is worth 3.333%
	float fixedZoom = float(zoom*.05);
	zoomPercentage_->setText(QString::number(fixedZoom*100)+QString("%"));
	//We don't want the cursor to change size just because we're zooming in and out of experiment.
	//This takes care of that.
	CursorGraphic::setZoom(fixedZoom);
	pixmapVisualTarget_->setZoom(fixedZoom);
	refreshSplash_ = true;
}

//! \brief Sets the status message to the passed in string
void RemoteViewer::setStatus(QString status, bool highPriority)
{
	if(!statusBar_)
		return;
	if(highPriority)
	{
		specialStatus_ = status;
		statusTimer_.restart();
	}
	else
		defaultStatus_ = status;
}

/*! \brief Called once per updateState(), this displays statuses at the bottom of window, giving 
 *	special statuses precedence over defaults for a set length of time
 */
void RemoteViewer::updateStatus()
{
	if(statusTimer_.elapsed() < SPECIAL_STATUS_PERIOD)
		statusBar_->setText(specialStatus_);
	else
		statusBar_->setText(defaultStatus_);
}

//! \brief Generates the UI task list if it is empty
void RemoteViewer::generateTaskList()
{
	if(taskListBox_->count() == 0)
	{
		taskListBox_->addItems(activeExperiment_->getTaskNames());
		taskListBox_->setCurrentIndex(0);
	}
}

/*! \brief updates the combo boxes containing Director instances and proxy servers
 *
 *	This function asks the server for a current list of Director/Proxy instances, and 
 *	then adds new ones to the combo box and removes old ones, as needed.
 */
void RemoteViewer::updateComponentLists(bool immediate)
{
	if(!immediate)
	{
		if(componentListsTimer_.elapsed() < COMPONENT_UPDATE_PERIOD)
			return;
		componentListsTimer_.restart();
	}
	QIcon runningIcon(QIcon(":/icons/runningWorkstation.png"));
	QIcon endingSession(QIcon(":/icons/endingSession.png"));

	//Update the director combo box
	QList<ComponentInstance> directors;
	directors = getDirectorList();

	//Add new directors
	for(int i=0; i<directors.length(); i++)
	{
		ComponentInstance d = directors[i];
		if(directorListBox_->findData(d.id) == -1)
		{
			if(d.status.toUpper() == "IDLE")
				directorListBox_->addItem(d.name, d.id);
			else if(d.status.toUpper() == "ENDING")
				directorListBox_->addItem(endingSession, d.name, d.id);
			else
				directorListBox_->addItem(runningIcon, d.name, d.id);
		}
	}

	//Remove old directors
	for(int i=0; i<directorListBox_->count(); i++)
	{
		bool remove = true;
		for(int j=0; j<directors.length(); j++)
		{
			if(directors[j].id ==directorListBox_->itemData(i))
			{
				if(directors[j].status.toUpper() == "IDLE")
					directorListBox_->setItemIcon(i,QIcon());
				else if(directors[j].status.toUpper() == "ENDING")
					directorListBox_->setItemIcon(i,endingSession);
				else
					directorListBox_->setItemIcon(i,runningIcon);

				remove = false;
				break;
			}
		}
		if(remove)
			directorListBox_->removeItem(i);
	}

	//Update the proxy combo box
	QList<ComponentInstance> proxies;
	proxies = getProxyList();

	//Add new proxies
	for(int i=0; i<proxies.length(); i++)
	{
		ComponentInstance p = proxies[i];
		if(proxyListBox_->findData(p.id) == -1)
		{
			if(p.status.toUpper() == "IDLE")
				proxyListBox_->addItem(p.name, p.id);
			else if(p.status.toUpper() == "ENDING")
				proxyListBox_->addItem(endingSession, p.name, p.id);
			else
				proxyListBox_->addItem(runningIcon, p.name, p.id);
		}
	}

	//Remove old proxies
	// Skip first entry because that's the "No Proxy" option.
	for(int i=1; i<proxyListBox_->count(); i++)
	{
		bool remove = true;
		for(int j=0; j<proxies.length(); j++)
		{
			if(proxies[j].id ==proxyListBox_->itemData(i))
			{
				if(proxies[j].status.toUpper() == "IDLE")
					proxyListBox_->setItemIcon(i,QIcon());
				else if(proxies[j].status.toUpper() == "ENDING")
					proxyListBox_->setItemIcon(i,endingSession);
				else
					proxyListBox_->setItemIcon(i,runningIcon);

				remove = false;
				break;
			}
		}
		if(remove)
			proxyListBox_->removeItem(i);
	}
}

/*! \brief returns the current status of a remote director
 */
RemoteViewer::ComponentStatus RemoteViewer::directorStatus(QString id)
{
	QList<ComponentInstance> directors = getDirectorList();

	foreach(ComponentInstance director, directors)
	{
		if(director.id == id)
		{
			if(director.status.toUpper() == "IDLE")
				return Idle;
			if(director.status.toUpper() == "ENDING")
				return Ending;
			else if(director.status.toUpper() == "STOPPED")
				return Stopped;
			else if(director.status.toUpper() == "RUNNING")
				return Running;
			else if(director.status.toUpper() == "PAUSED")
				return Paused;
			else
				return NotFound;
		}
	}
	//If we made it this far, something is wrong...
	return NotFound;
}

/*! \brief returns the current status of a remote proxy
 */
RemoteViewer::ComponentStatus RemoteViewer::proxyStatus(QString id)
{
	QList<ComponentInstance> proxies = getProxyList();

	foreach(ComponentInstance proxy, proxies)
	{
		if(proxy.id == id)
		{
			if(proxy.status.toUpper() == "IDLE")
				return Idle;
			if(proxy.status.toUpper() == "ENDING")
				return Ending;
			else if(proxy.status.toUpper() == "STOPPED")
				return Stopped;
			else if(proxy.status.toUpper() == "RUNNING")
				return Running;
			else if(proxy.status.toUpper() == "PAUSED")
				return Paused;
			else
				return NotFound;
		}
	}
	//If we made it this far, something is wrong...
	return NotFound;
}

/*! \brief Returns a list of directors and their properties
 *
 *	This sends out a DIRECTORLIST command, and then parses the response into a 
 *	QList of ComponentInstances (really just a struct with a couple of strings).
 *	If something goes wrong, or no director instances are found, an empty
 *	list is returned.
 */  
QList<RemoteViewer::ComponentInstance> RemoteViewer::getDirectorList()
{
	//Get rid of any incoming responses that were ignored.
	while(serverChannel_->incomingResponsesWaiting() != 0)
		serverChannel_->getResponse();

	if(!serverChannel_->assureConnection())
	{
		return currDirectorList_;
	}

	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("DIRECTORLIST / PICTO/1.0"));
	QSharedPointer<Picto::ProtocolResponse> response;

	serverChannel_->sendRegisteredCommand(command);
	QString commandID = command->getFieldValue("Command-ID");
	//Get the response to this command
	do
	{
		if(!serverChannel_->waitForResponse(100))
		{
			setStatus(tr("DIRECTORLIST command timed out"),true);
			return currDirectorList_;
		}
		response = serverChannel_->getResponse();
	}while(!response || response->getFieldValue("Command-ID") != commandID);
	
	currDirectorList_.clear();

	QByteArray xmlFragment = response->getContent();
	QXmlStreamReader xmlReader(xmlFragment);

	while(!xmlReader.atEnd())
	{
		xmlReader.readNext();

		if(xmlReader.isStartElement() && xmlReader.name() == "Director")
		{
			ComponentInstance director;

			while(!xmlReader.atEnd())
			{
				xmlReader.readNext();

				if(xmlReader.name() == "Name" && xmlReader.isStartElement())
				{
					director.name = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Address" && xmlReader.isStartElement())
				{
					director.address = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Id" && xmlReader.isStartElement())
				{
					director.id = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Status" && xmlReader.isStartElement())
				{
					director.status = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Session-ID" && xmlReader.isStartElement())
				{
					director.sessionID = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Director" && xmlReader.isEndElement())
				{
					currDirectorList_.append(director);
					break;
				}
			}
		}
	}
	return currDirectorList_;
}

/*! \brief Returns a list of proxy servers and their address
 *
 *	This sends out a PROXYLIST command, and then parses the response into a 
 *	QList of ComponentInstances (really just a struct with a few strings).
 *	If something goes wrong, or no proxy instances are found, an empty
 *	list is returned.
 */  
QList<RemoteViewer::ComponentInstance> RemoteViewer::getProxyList()
{
	while(serverChannel_->incomingResponsesWaiting() != 0)
		serverChannel_->getResponse();

	if(!serverChannel_->assureConnection())
	{
		return currProxyList_;
	}

	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("PROXYLIST / PICTO/1.0"));
	QSharedPointer<Picto::ProtocolResponse> response;

	serverChannel_->sendRegisteredCommand(command);
	QString commandID = command->getFieldValue("Command-ID");
	//Get the response to this command
	do
	{
		if(!serverChannel_->waitForResponse(100))
		{
			setStatus(tr("SPROXYLIST command timed out"),true);
			return currProxyList_;
		}
		response = serverChannel_->getResponse();
	}while(!response || response->getFieldValue("Command-ID") != commandID);
	
	currProxyList_.clear();

	QByteArray xmlFragment = response->getContent();
	QXmlStreamReader xmlReader(xmlFragment);

	while(!xmlReader.atEnd())
	{
		xmlReader.readNext();

		if(xmlReader.isStartElement() && xmlReader.name() == "Proxy")
		{
			ComponentInstance proxy;

			while(!xmlReader.atEnd())
			{
				xmlReader.readNext();

				if(xmlReader.name() == "Name" && xmlReader.isStartElement())
				{
					proxy.name = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Address" && xmlReader.isStartElement())
				{
					proxy.address = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Id" && xmlReader.isStartElement())
				{
					proxy.id = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Status" && xmlReader.isStartElement())
				{
					proxy.status = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Proxy" && xmlReader.isEndElement())
				{
					currProxyList_.append(proxy);
					break;
				}
			}
		}
	}
	return currProxyList_;
}

/*! \brief Starts a new session
 *
 *	This starts a new session on the server.  The act of starting a session basically 
 *	ties together the various components involved (Director, and proxy) and 
 *	starts a new session database.  Note that the Workstation instance isn't really 
 *	tied to the session.  This is intentional, and makes it easy to remotely view and
 *	control running sessions.
 *
 *	Note that starting the session is really just a remote command and has no effect 
 *	on the local workstation.  If we want to actually watch the session, we'll then need 
 *	to join the session (joinSession()).
 *
 *	The order of function calls that would occur for a single user running an experiment
 *	will look like this:
 *		startSession()
 *		joinSession()
 *		disjoinSession()
 *		endSession()
 *	If a user simply wants to observe a session that someone else started, it would
 *	look like this:
 *		joinSession()
 *		disjoinSession()
 */
bool RemoteViewer::startSession()
{
	////Check for a currently running session
	//if(!sessionId_.isNull())
	//	return true;

#ifndef DEVELOPMENTBUILD
	//Require a password to start a session
	if(passwordEdit_->text().isEmpty())
	{
		QMessageBox::warning(0,"Enter Session Key","You must enter a session key to start a new session.");
		return false;
	}
#endif

	QString commandStr;
	QString directorID = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
	QString proxyID = proxyListBox_->itemData(proxyListBox_->currentIndex()).toString();
	commandStr = "STARTSESSION "+directorID+"/"+proxyID+" PICTO/1.0";

	QSharedPointer<Picto::ProtocolCommand> startSessCommand(new Picto::ProtocolCommand(commandStr));

	QByteArray dataXml = pictoDataText_->toPlainText().toUtf8();

	QSharedPointer<ExperimentConfig> expConfig = experiment_->getExperimentConfig();
	if(!expConfig)
		return false;
	dataXml.append(expConfig->toXml().toUtf8());
	startSessCommand->setContent(dataXml);
	startSessCommand->setFieldValue("Content-Length",QString("%1").arg(dataXml.length()));
	startSessCommand->setFieldValue("Observer-ID",observerId_.toString());
	startSessCommand->setFieldValue("Password",passwordEdit_->text());

	QSharedPointer<Picto::ProtocolResponse> loadExpResponse;

	serverChannel_->sendRegisteredCommand(startSessCommand);
	QString commandID = startSessCommand->getFieldValue("Command-ID");
	//Get the response to this command
	do
	{
		if(!serverChannel_->waitForResponse(10000))
		{
			setStatus(tr("STARTSESSION command timed out"),true);
			return false;
		}
		loadExpResponse = serverChannel_->getResponse();
	}while(!loadExpResponse || loadExpResponse->getFieldValue("Command-ID") != commandID);


	if(loadExpResponse.isNull())
	{
		setStatus(tr("No response received from server.\nExperiment not loaded."),true);
		return false;
	}
	else if(loadExpResponse->getResponseCode() == Picto::ProtocolResponseType::NotFound)
	{
		setStatus(tr("Director instance not found with ID:") + directorID + tr(".  Experiment not loaded."),true);
		return false;
	}
	else if(loadExpResponse->getResponseCode() == Picto::ProtocolResponseType::Unauthorized)
	{
		setStatus(tr("Director instance has already loaded an experiment.  Experiment not loaded."),true);
		return false;
	}
	else if(loadExpResponse->getResponseCode() == Picto::ProtocolResponseType::OK)
	{
		setStatus(tr("Experiment loaded on remote Director instance."),true);
		return true;
	}
	else
	{
		setStatus(tr("Unexpected response from server.\nExperiment not loaded."),true);
		return false;
	}

}

/*! \brief Ends a currently running session
 *
 *	This is the opposite of startSession.  Again note that this function has no real
 *	effect on our local workstation instance.  See the comments on startSession for
 *	a more detailed explanation.
 */
bool RemoteViewer::endSession()
{
	//Check for a currently running session
	if(sessionId_.isNull())
	{
		setStatus(tr("Not currently in a session"),true);
		return false;
	}


	QSharedPointer<Picto::ProtocolCommand> endSessCommand(new Picto::ProtocolCommand("ENDSESSION "+sessionId_+" PICTO/1.0"));
	endSessCommand->setFieldValue("Observer-ID",observerId_.toString());

	QSharedPointer<Picto::ProtocolResponse> endSessResponse;

	serverChannel_->sendRegisteredCommand(endSessCommand);
	QString commandID = endSessCommand->getFieldValue("Command-ID");
	//Get the response to this command
	do
	{
		if(!serverChannel_->waitForResponse(5000))
		{
			setStatus(tr("ENDSESSION command timed out"),true);
			qDebug()<<(tr("ENDSESSION command timed out"));
			return false;
		}
		endSessResponse = serverChannel_->getResponse();
	}while(!endSessResponse || endSessResponse->getFieldValue("Command-ID") != commandID);

	if(endSessResponse->getResponseCode() == Picto::ProtocolResponseType::NotFound)
	{
		setStatus(tr("Session ID not found, session not ended"),true);
	}
	else if(endSessResponse->getResponseCode() == Picto::ProtocolResponseType::OK)
	{
		setStatus(tr("Session ended"),true);
	}
	else if(endSessResponse->getResponseCode() == Picto::ProtocolResponseType::Unauthorized)
	{
		setStatus(tr("Workstation instance not authorized to end session"),true);
	}
	else
	{
		setStatus(tr("Unexpected response to ENDSESSION command"),true);
	}
	sessionId_ = QUuid();

	return true;

}

/*! \brief Joins a currently running session as an "observer" of the session
 *
 *	Joining a session is the act of connecting this workstation to an already 
 *	running session.  This is what allows us to see the session on the workstation
 *	screen.
 *
 *	We always keep the latest active experiment in memory so that this function
 *	will go quickly if we rejoin a sessin that we were previously connected too.
 */	
bool RemoteViewer::joinSession()
{
	QString directorID = directorListBox_->itemData(directorListBox_->currentIndex()).toString();

	QByteArray content;
	QSharedPointer<QXmlStreamReader> xmlReader;
	QString commandStr;

	//Start by figuring out this sessions sessionID

	//Send a JOINSESSION command
	commandStr = "JOINSESSION "+directorID+" PICTO/1.0";

	QSharedPointer<Picto::ProtocolCommand> joinSessCommand(new Picto::ProtocolCommand(commandStr));
	joinSessCommand->setFieldValue("Password",passwordEdit_->text());
	joinSessCommand->setFieldValue("Observer-ID",observerId_.toString());
	QSharedPointer<Picto::ProtocolResponse> joinSessResponse;

	serverChannel_->sendRegisteredCommand(joinSessCommand);
	QString commandID = joinSessCommand->getFieldValue("Command-ID");
	//Get the response to this command
	do
	{
		if(!serverChannel_->waitForResponse(3000))
		{
			setStatus(tr("JOINSESSION command timed out"),true);
			return false;
		}
		joinSessResponse = serverChannel_->getResponse();
	}while(!joinSessResponse || joinSessResponse->getFieldValue("Command-ID") != commandID);

	if(joinSessResponse.isNull() || joinSessResponse->getResponseCode() != Picto::ProtocolResponseType::OK)
	{
		setStatus(tr("Unexpected response to JOINSESSION. Unable to join session."),true);
		return false;
	}

	content = joinSessResponse->getDecodedContent();
	xmlReader = QSharedPointer<QXmlStreamReader>(new QXmlStreamReader(content));

	//Extract the sessionID
	while(!xmlReader->atEnd() && !(xmlReader->isStartElement() && xmlReader->name() == "SessionID"))
		xmlReader->readNext();

	QUuid remoteSessionId = QUuid();
	if(!xmlReader->atEnd())
	{
		remoteSessionId = QUuid(xmlReader->readElementText());
	}
	else
	{
		setStatus(tr("SessionID not found in response from server."),true);
		return false;
	}

	//If we didn't already, we'll need to also put the experiment in activeExperiment
	if(sessionId_.isNull() || (remoteSessionId != sessionId_))
	{
		sessionId_ = remoteSessionId;
		//Extract the experiment xml
		activeExperiment_ = QSharedPointer<Picto::Experiment>(Picto::Experiment::Create());
		while(!xmlReader->atEnd() && !(xmlReader->isStartElement() && xmlReader->name() == "Experiment"))
			xmlReader->readNext();

		if(!xmlReader->atEnd())
		{
			if(!activeExperiment_->fromXml(xmlReader))
			{
				setStatus(tr("Unable to deserialize Experiment returned by JOINSESSION"),true);
				return false;
			}
		}
		else
		{
			setStatus(tr("No Experiment returned by JOINSESSION"),true);
			return false;
		}

	}
	activeExperiment_->setEngine(engine_);

	serverChannel_->setSessionId(sessionId_);
	engineSlaveChannel_->setSessionId(sessionId_);

	setStatus(tr("Existing session joined. Session ID: ")+ sessionId_.toString(),true);

	//Send an isauthorized TASK command to figure out if we're authorized to send commands
	//on this session.
	if(sendTaskCommand("isauthorized"))
		isAuthorized_ = true;
	else
		isAuthorized_ = false;

	//Finally figure out what the status of the remote director is (stopped, running, or paused)
	//and get our director running in that state.
	ComponentStatus remoteStatus = directorStatus(directorID);
	if(remoteStatus < Stopped)
	{
		setStatus("Attempted to join a session with a director that isn't in an active session",true);
		return false;
	}

	return true;
}




/*! \brief Causes the engine to start running the experiment the next time updateEngine() is called.
 *
 */
bool RemoteViewer::syncExperiment()
{
	engineTrigger_ = StartEngine;
	return true;
}

/*! \brief Causes the engine to stop the currently running experiment before the next time updateState() runs.
 *
 */
void RemoteViewer::stopExperiment()
{
	Q_ASSERT(engine_);
	engine_->stop();
	engineTrigger_ = StopEngine;
}

/*! \brief Disjoins a currently running session
 *
 *	This is the opposite of the join function.  This detaches us from a running session 
 *	(but it doesn't actually end the session0.
 */
bool RemoteViewer::disjoinSession()
{
	serverChannel_->setSessionId(QUuid());
	engineSlaveChannel_->setSessionId(QUuid());

	return true;
}

//! Sends a task command with the given target.  Returns true if execution was successful
bool RemoteViewer::sendTaskCommand(QString target, QString msgContent)
{
	QSharedPointer<Picto::ProtocolCommand> cmd(new Picto::ProtocolCommand("TASK "+target+" PICTO/1.0"));
	cmd->setFieldValue("Observer-ID",observerId_.toString());
	if(!msgContent.isEmpty())
	{
		QByteArray byteContent = msgContent.toAscii();
		cmd->setContent(byteContent);
		cmd->setFieldValue("Content-Length",QString::number(byteContent.length()));
	}
	
	QSharedPointer<Picto::ProtocolResponse> cmdResponse;

	serverChannel_->sendRegisteredCommand(cmd);
	QString commandID = cmd->getFieldValue("Command-ID");
	//Get the response to this command
	do
	{
		if(!serverChannel_->waitForResponse(100))
		{
			setStatus(tr("TASK ")+target+" command timed out",true);
			return false;
		}
		cmdResponse = serverChannel_->getResponse();
	}while(!cmdResponse || cmdResponse->getFieldValue("Command-ID") != commandID);

	if(cmdResponse->getFieldValue("Password")!="")
		passwordEdit_->setText(cmdResponse->getFieldValue("Password"));

	if(cmdResponse->getResponseType() == "OK")
	{
		setStatus("TASK "+target+tr(" command completed"),true);
		return true;
	}
	else if(cmdResponse->getResponseCode() == 400)
	{
		setStatus("TASK "+target+tr(" command failed: ")+QString(cmdResponse->getContent()),true);
		return false;
	}
	else if(cmdResponse->getResponseCode() == 404)
	{
		setStatus("TASK "+target+tr(" command failed: Session ID not recognized"),true);
		return false;
	}
	else if(cmdResponse->getResponseCode() == Picto::ProtocolResponseType::Unauthorized)
	{
		setStatus("TASK "+target+tr(" command failed: Workstation instance not authorized"),true);
		return false;
	}
	else
	{
		Q_ASSERT(false);
		return false;
	}
}

/*! \brief Called whenever a new task is selected.  
 *	Updates the values in the property frame to reflect the new task.
 */
void RemoteViewer::taskListIndexChanged(int)
{
	QSharedPointer<Task> task = activeExperiment_->getTaskByName(taskListBox_->currentText());
	if(!task)
		return;
	qobject_cast<PropertyFrame*>(propertyFrame_)->setTopLevelDataStore(task.staticCast<DataStore>());
}

/*! \brief Assures that the server channels are connected and signifies if the connection is lost
 * This function is called for each runState() call.
 */
bool RemoteViewer::assureChannelConnections()
{
	bool hadDisconnect = false;
	if(!serverChannel_->isConnected())
	{
		serverChannel_->connectToServer();
		if(!serverChannel_->isConnected())
			hadDisconnect = true;
	}
	if(!engineSlaveChannel_->isConnected())
	{
		engineSlaveChannel_->connectToServer();
		if(!engineSlaveChannel_->isConnected())
			hadDisconnect = true;
	}
	return !hadDisconnect;
}