

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
#include <QTabWidget>
#include <QDir>

#include "remoteviewer.h"

#include "../common/globals.h"
#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/engine/XYSignalChannel.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "../../common/storage/StateDataUnit.h"
#include "../../common/statemachine/statemachine.h"
#include "../../common/stimuli/cursorgraphic.h"
#include "../../common/statemachine/scene.h"
#include "../../common/storage/NeuralDataUnit.h"
#include "../../common/storage/LFPDataUnitPackage.h"
#include "../../common/storage/directordata.h"
#include "../../common/compositor/OutputSignalWidget.h"
#include "../../common/designer/propertyframe.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief The Director/Proxy lists will be updated each time this many ms go by.*/
#define COMPONENT_UPDATE_PERIOD 1000 
/*! \brief The Neural data will be updated each time this many ms go by.*/
#define NEURAL_DATA_UPDATE_PERIOD 50 
/*! \brief The TaskRunViewer will be updated each time this many ms go by.*/
#define SESSION_DATA_UPDATE_PERIOD 1000
/*! \brief Special status message will appear in the bottom left status bar for this many ms.*/
#define SPECIAL_STATUS_PERIOD 1000
/*! \brief An adaptive timeout for waiting for Server responses is used to account for changes in network
 *	activity.  This is the minimum timeout (ms) that is allowed for this system.
 */
#define MIN_ADAPTIVE_TIMEOUT 50
/*! \brief An adaptive timeout for waiting for Server responses is used to account for changes in network
 *	activity.  This is the maximum timeout(ms) that is allowed for this system.
 */
#define MAX_ADAPTIVE_TIMEOUT 500
/*! \brief In order for changes to the default reward quantity that get sent to the Director and
 *	then back to this Workstation to not cause annoying echo effects when we are simply holding 
 *	down the increment/decrement buttons, we set a delay from the latest reward value change.  
 *	Only after this delay can new values coming in from the Director take effect.
 */
#define REWARD_CHANGE_DELAY 5000

/*! \brief Constructs a RemoteViewer object.
 *	\details Sets up the RemoteViewer UI. Initializes variables.  Gets the workstation ID from the Workstation's .config file.
 */
RemoteViewer::RemoteViewer(QWidget *parent) :
	Viewer(parent),
	serverChannel_(0),
	activeExperiment_(0),
	activeDesignRoot_(0),
	statusBar_(NULL),
	isAuthorized_(false),
	propertyFrame_(NULL),
	myDesignRoot_(new DesignRoot())
{
	setupServerChannel();
	setupEngine();
	setupUi();

	QString dbName = "PictoWorkstation";
	dbName = dbName.toLower();
	QSqlDatabase configDb = QSqlDatabase::addDatabase("QSQLITE",dbName);
	QString configPath = QCoreApplication::applicationDirPath()+"/../config";
	QDir configDir(configPath);
	if(!configDir.exists())
	{
		configDir.mkpath(configPath);
		configDir = QDir(configPath);
	}
	configDb.setDatabaseName(configDir.canonicalPath() + "/" + dbName + ".config");
	configDb.open();

	QSqlQuery query(configDb);
	if(!configDb.tables().contains("workstationinfo"))
	{
		query.exec("CREATE TABLE workstationinfo (key TEXT, value TEXT)");
		observerId_ = QUuid::createUuid();
		query.prepare("INSERT INTO workstationinfo (key,value) VALUES ('id',:id)");
		query.bindValue(":id",observerId_.toString());
		query.exec();
	}
	query.exec("SELECT value FROM workstationinfo WHERE key='id'");
	bool rc = query.next();
	IGNORED_PARAMETER(rc);
	Q_ASSERT(rc);
	observerId_ = QUuid(query.value(0).toString());
	configDb.close();

	currState_ = InitState;
	stateTrigger_ = Disconnected;
	stateUpdateTimer_ = new QTimer(this);
	stateUpdateTimer_->setInterval(16);
	connect(stateUpdateTimer_,SIGNAL(timeout()),this,SLOT(updateState()));
	updatingState_ = false;
	zoomChanged_ = false;
	zoomValue_ = 1.0;
}

RemoteViewer::~RemoteViewer()
{
}
/*! \brief The remote viewer is implemented as a state machine.  This is the main state machine run loop.
 *	\details Each time this function is called, it checks for state change triggers.  If they have occured, the
 *	state is changed, the enterState() function is called for the new state.  Every time updateState() is called, runState() is called 
 *	for the state.  Then if a new state change trigger is detected after that, endState() is called for that state. 
 */
void RemoteViewer::updateState()
{
	updatingState_ = true;
	updateComponentLists();
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
		case SessionPaused:
			nextState = PausedSession;
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
		updater_->updateState();
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
		updater_->updateState();
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

/*! \brief This function is called once per state machine loop.
 * \details This function is used to run periodic state specific checks/logic and check 
 * for new state transitions.  A state triggers a new state transition by 
 * setting stateTrigger_ to a new value.  This transition will then be used to 
 * select the next state in updateState()
 */
void RemoteViewer::runState()
{
	QString directorID = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
	ComponentStatus remoteStatus = directorStatus(directorID);
	switch(currState_)
	{
	case WaitForConnect:
		activeExpName_->setText(myDesignRoot_->getDesignName()); //In case operator loads new file
		if(assureChannelConnections())
			stateTrigger_ = Connected;
		break;
	case WaitForJoin:
		activeExpName_->setText(myDesignRoot_->getDesignName()); //In case operator loads new file
		if(connectAction_->isChecked())
		{
				
			if(remoteStatus == Idle)
				stateTrigger_ = StartSessionRequest;
			if(remoteStatus > Ending)
				stateTrigger_ = JoinSessionRequest;
		}
		break;
	case JoiningSession:
		activeExpName_->setText(myDesignRoot_->getDesignName()); //In case operator loads new file
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
		activeExpName_->setText(myDesignRoot_->getDesignName()); //In case operator loads new file
		if(startSession())
			stateTrigger_ = SessionStarted;
		else
			stateTrigger_ = SessionDidntStart;
		break;
	case StoppedSession:
		if(zoomChanged_)
		{
			zoomChanged_ = false;
			pixmapVisualTarget_->setZoom(zoomValue_);
			renderingTarget_->showSplash();
		}
		if(remoteStatus == Paused)
			stateTrigger_ = SessionPaused;
		if(remoteStatus == Running)
			stateTrigger_ = SessionRunning;
		if(!connectAction_->isChecked())
		{
			if(shouldEndSession())
				stateTrigger_ = EndSessionRequest;
			else
			{
				disjoinSession();
				stateTrigger_ = DisjoinSessionRequest;
			}
		}
		if((remoteStatus < Stopped) && (remoteStatus > NotFound))
		{
			disjoinSession();
			stateTrigger_ = DisjoinSessionRequest;
		}
		updateSessionDataPackage();
		if(proxyListBox_->currentIndex() > 0)
		{
			updateNeuralData();
			neuralDataViewer_->replot();
		}
		break;
	case PausedSession:
		if(zoomChanged_)
		{
			zoomChanged_ = false;
			Scene::setZoom(zoomValue_);
		}
		if(remoteStatus == Running)
			stateTrigger_ = SessionRunning;
		//If the engine isn't running for some reason, or the director stopped, go to the stopped
		//state.  In the case where the director is still running but our engine stopped, the stop
		//state will just restart the engine and send us back here.
		if(remoteStatus == Stopped)
			stateTrigger_ = SessionStopped;
		if(!connectAction_->isChecked() || ((remoteStatus < Stopped) && (remoteStatus > NotFound)))
		{
			disjoinSession();
			stateTrigger_ = DisjoinSessionRequest;
		}
		if(stateTrigger_ != NoViewerTrigger)
			stopExperiment();
		updateSessionDataPackage();
		if(proxyListBox_->currentIndex() > 0)
		{
			updateNeuralData();
			neuralDataViewer_->replot();
		}
		break;
	case RunningSession:
		if(zoomChanged_)
		{
			zoomChanged_ = false;
			Scene::setZoom(zoomValue_);
		}
		if(remoteStatus == Paused)
			stateTrigger_ = SessionPaused;
		//If the engine isn't running for some reason, or the director stopped, go to the stopped
		//state.  In the case where the director is still running but our engine stopped, the stop
		//state will just restart the engine and send us back here.
		if(remoteStatus == Stopped)
			stateTrigger_ = SessionStopped;
		if(!connectAction_->isChecked() || ((remoteStatus < Stopped) && (remoteStatus > NotFound)))
		{
			disjoinSession();
			stateTrigger_ = DisjoinSessionRequest;
		}
		if(stateTrigger_ != NoViewerTrigger)
			stopExperiment();
		updateSessionDataPackage();
		if(proxyListBox_->currentIndex() > 0)
		{
			updateNeuralData();
			neuralDataViewer_->replot();
		}
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

/*! \brief The function is called when a state is ending and a new state will start on the next updateState() call.
 *	\details A state may have different ending actions depending on the trigger that it is ending
 *	with.  This function is called after that trigger has been determined.  
 *	\note Actions on the engine by this function (ie. stop)
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
 *	\details The function is used to update the UI such that it logically reflects
 *	the actions available in the new state.
 */
void RemoteViewer::enterState()
{
	qDebug("Entering State: " + QString::number(currState_).toLatin1());
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
		currentRunViewer_->clear();
		activeExpName_->setText(myDesignRoot_->getDesignName());
		neuralDataViewer_->deinitialize();
		renderingTarget_->showSplash();
		mainTabbedFrame_->setCurrentIndex(0);
		mainTabbedFrame_->setTabEnabled(0,false);
		mainTabbedFrame_->setTabEnabled(1,false);
		mainTabbedFrame_->setTabIcon(1,QIcon());
		mainTabbedFrame_->setTabEnabled(2,false);
		mainTabbedFrame_->setTabIcon(2,QIcon());
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
		currentRunViewer_->clear();
		activeExpName_->setText(myDesignRoot_->getDesignName());
		neuralDataViewer_->deinitialize();
		renderingTarget_->showSplash();
		mainTabbedFrame_->setCurrentIndex(0);
		mainTabbedFrame_->setTabEnabled(0,false);
		mainTabbedFrame_->setTabEnabled(1,false);
		mainTabbedFrame_->setTabEnabled(2,false);
		mainTabbedFrame_->setTabIcon(2,QIcon());
		mainTabbedFrame_->setTabIcon(1,QIcon());
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
		currentRunViewer_->clear();
		activeExpName_->setText(myDesignRoot_->getDesignName());
		neuralDataViewer_->deinitialize();
		neuralDataViewer_->initialize();
		renderingTarget_->showSplash();
		mainTabbedFrame_->setCurrentIndex(0);
		mainTabbedFrame_->setTabEnabled(0,false);
		mainTabbedFrame_->setTabEnabled(1,false);
		mainTabbedFrame_->setTabIcon(1,QIcon());
		mainTabbedFrame_->setTabEnabled(2,false);
		mainTabbedFrame_->setTabIcon(2,QIcon());
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
		currentRunViewer_->clear();
		activeExpName_->setText(myDesignRoot_->getDesignName());
		neuralDataViewer_->initialize();
		renderingTarget_->showSplash();
		mainTabbedFrame_->setCurrentIndex(0);
		mainTabbedFrame_->setTabEnabled(0,false);
		mainTabbedFrame_->setTabEnabled(1,false);
		mainTabbedFrame_->setTabIcon(1,QIcon());
		mainTabbedFrame_->setTabEnabled(2,false);
		mainTabbedFrame_->setTabIcon(2,QIcon());
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
		currentRunViewer_->markLatestAsRunning(false);
		pixmapVisualTarget_->setZoom(zoomValue_);
		renderingTarget_->showSplash();
		mainTabbedFrame_->setTabEnabled(0,true);
		mainTabbedFrame_->setTabIcon(2,currentRunViewer_->getLatestRunIcon());
		activeExpName_->setText(activeDesignRoot_->getDesignName());
		propertyFrame_->setEnabled(isAuthorized_);
		foreach(QWidget * outSigWidg, outputSignalsWidgets_)
		{
			static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
		}
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
		mainTabbedFrame_->setTabEnabled(0,true);
		mainTabbedFrame_->setTabIcon(2,currentRunViewer_->getLatestRunIcon());
		currentRunViewer_->markLatestAsRunning(true);
		Scene::setZoom(zoomValue_);
		activeExpName_->setText(activeDesignRoot_->getDesignName());
		propertyFrame_->setEnabled(isAuthorized_);
		generateTaskList();
		foreach(QWidget * outSigWidg, outputSignalsWidgets_)
		{
			static_cast<OutputSignalWidget*>(outSigWidg)->enable(true);
		}
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
		mainTabbedFrame_->setTabEnabled(0,true);
		mainTabbedFrame_->setTabIcon(2,currentRunViewer_->getLatestRunIcon());
		currentRunViewer_->markLatestAsRunning(true);
		Scene::setZoom(zoomValue_);
		activeExpName_->setText(activeDesignRoot_->getDesignName());
		propertyFrame_->setEnabled(isAuthorized_);
		generateTaskList();
		foreach(QWidget * outSigWidg, outputSignalsWidgets_)
		{
			static_cast<OutputSignalWidget*>(outSigWidg)->enable(true);
		}
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
		currentRunViewer_->clear();
		activeExpName_->setText(activeDesignRoot_->getDesignName());
		neuralDataViewer_->deinitialize();
		renderingTarget_->showSplash();
		mainTabbedFrame_->setCurrentIndex(0);
		mainTabbedFrame_->setTabEnabled(0,false);
		mainTabbedFrame_->setTabEnabled(1,false);
		mainTabbedFrame_->setTabIcon(1,QIcon());
		mainTabbedFrame_->setTabEnabled(2,false);
		mainTabbedFrame_->setTabIcon(2,QIcon());
		taskListBox_->clear();
		foreach(QWidget * outSigWidg, outputSignalsWidgets_)
		{
			static_cast<OutputSignalWidget*>(outSigWidg)->enable(false);
		}
		qobject_cast<PropertyFrame*>(propertyFrame_)->clearProperties();
		break;
	}
}

/*! \brief Called just before displaying the RemoteViewer.
 *	\details Loads the current Experiment and starts various timers.
 */
void RemoteViewer::init()
{
	//Sometimes while working on an experiment, if we change something and don't reopen the experiment
	//the experimental run doesn't work right.  To prevent this, we are simply reseting the the experiment
	//from xml whenever we open this viewer
	bool res = myDesignRoot_->resetDesignRoot(designRoot_->getDesignRootText());
	if(!res)
	{
		DesignMessage errorMsg = myDesignRoot_->getLastError();
		QMessageBox box;
		box.setTextFormat(Qt::RichText);
		box.setText(errorMsg.name);
		box.setDetailedText(errorMsg.details);
		box.setIconPixmap(QPixmap(":/icons/x.png"));		
		box.exec();
	}
	if(myDesignRoot_->hasWarning())
	{
		DesignMessage warnMsg = myDesignRoot_->getLastWarning();
		QMessageBox::warning(0,warnMsg.name,warnMsg.details);
	}
	myDesignRoot_->enableRunMode(true);
	if(!myDesignRoot_->compiles())
	{
		QMessageBox msg;
		msg.setText("Local design does not compile. "
			"You can join a running session, but you will not be able to start a new one.");
		msg.setIconPixmap(QPixmap(":/icons/triangle.png"));
		msg.exec();
	}
	
	experiment_ = myDesignRoot_->getExperiment().staticCast<Experiment>();
	if(!experiment_)
	{
		QMessageBox msg;
		msg.setText("Failed to load current experiment.");
		msg.setIconPixmap(QPixmap(":/icons/triangle.png"));
		msg.exec();
	}

	if(currState_ == InitState)
	{	//The remote viewer was just created.  Initialize it by running updateState once.
		updateState();
	}
	//This timer assures that we don't update component lists more often than necessary
	componentListsTimer_.restart();
	//This timer assures that we don't update neural data more often than necessary
	neuralDataTimer_.restart();
	//This timer assures that we don't update session data more often than necessary
	sessionDataTimer_.restart();
	//This timer triggers each new run of updateState()
	stateUpdateTimer_->start();
	//This timer makes sure that we don't read in old reward duration data just after setting a new value.
	rewardDurChangeTimer_.restart();
}

/*! \brief Called just before hiding the viewer.
 *	\details Stops the stateUpdateTimer_ so that the RemoteViewer statemachine won't be active when the
 *	viewer doesn't have focus.
 */
void RemoteViewer::deinit()
{
	//Stop the timers so that our state update functions won't get called anymore.
	stateUpdateTimer_->stop();
	emit deinitComplete();
}

/*! \brief Called when the application is about to quit.  Takes care of closing this window's resources.
 *	\details If a Session is open and stopped, this triggers an endSession() request.  This also calls
 *	deinit() because otherwise the Experiment could continue running even though the GUI was closed and
 *	the process would stick around until we restarted the computer or closed it from the task manager.
 */
bool RemoteViewer::aboutToQuit()
{
	//Its not so pretty to have to work outside the state machine like this, but if we're about to
	//quit, we don't know what's going to be happening to the state machine before the process is released
	//so we have to take care of possibly ending the current session here. :(
	if((currState_ == StoppedSession) && shouldEndSession())
		endSession();

	//Calling deinit is important because it stops the experiment.  Otherwise, the 
	//experiment would keep on going even though the window was closed and this
	//process would stick around in the task manager for eternity.
	deinit();
	return true;
}

/*! \brief Initializes the engine for running the RemoteViewer environment (PixmapVisualTarget, AudioRewardController, etc).
 */
void RemoteViewer::setupEngine()
{
	//set up the engine
	engine_ = QSharedPointer<Picto::Engine::PictoEngine>(new Picto::Engine::PictoEngine);
	engine_->setExclusiveMode(false);
	engine_->setOperatorAsUser(true);
	engine_->setSlaveMode(true);

	//Set up remote state updater
	updater_ = QSharedPointer<RemoteStateUpdater>(new RemoteStateUpdater(engineSlaveChannel_));

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
	QSharedPointer<Picto::SignalChannel> signalChannel(new Picto::XYSignalChannel("Position"));
	engine_->addSignalChannel(signalChannel);

	QSharedPointer<Picto::GenericInput> genericInputs(new Picto::GenericInput("GenericInputs"));
	engine_->addSignalChannel(genericInputs);

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
}

/*! \brief Sets up the user interface portions of the GUI.
 */
void RemoteViewer::setupUi()
{
	//----- Task Action -----
	playAction_ = new QAction(tr("R&un"),this);
	playAction_->setIcon(QIcon(":/icons/play.png"));
	playAction_->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
	playAction_->setToolTip("Run (Ctrl+R)");
	connect(playAction_,SIGNAL(triggered()),this, SLOT(playAction()));

	pauseAction_ = new QAction(tr("&Pause"),this);
	pauseAction_->setIcon(QIcon(":/icons/pause.png"));
	pauseAction_->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_P));
	pauseAction_->setToolTip("Pause (Ctrl+P)");
	connect(pauseAction_,SIGNAL(triggered()),this, SLOT(pause()));

	stopAction_ = new QAction(tr("&Stop"),this);
	stopAction_->setIcon(QIcon(":/icons/stop.png"));
	stopAction_->setToolTip("Stop");
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
	rewardQuantity_->setToolTip("Reward Quantity (Ms)");
	connect(rewardQuantity_, SIGNAL(editingFinished()), this, SLOT(changeRewardDuration()));

	loadPropsAction_ = new QAction(tr("&Load Task Properties from Session"),this);
	loadPropsAction_->setIcon(QIcon(":/icons/loadvalues.png"));
	loadPropsAction_->setToolTip("Load Task Properties from Session");
	connect(loadPropsAction_, SIGNAL(triggered()),this, SLOT(LoadPropValsFromFile()));
	loadPropsAction_->setEnabled(false);

	//TaskList combo box
	taskListBox_ = new QComboBox;
	taskListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	taskListBox_->setToolTip("Select Task");

	//The toolbar
	toolBar_ = new QToolBar;

	toolBar_->addAction(playAction_);
	toolBar_->addAction(pauseAction_);
	toolBar_->addWidget(taskListBox_);
	toolBar_->addAction(stopAction_);
	toolBar_->addSeparator();
	toolBar_->addAction(loadPropsAction_);
	toolBar_->addSeparator();
	toolBar_->addWidget(rewardQuantity_);
	toolBar_->addAction(rewardAction_);
	toolBar_->addSeparator();

	//----- Connection Actions -----
	connectAction_ = new QAction(tr("&Connect"), this);
	connectAction_->setCheckable(true);
	connectAction_->setIcon(QIcon(":/icons/disconnected.png"));
	connectAction_->setToolTip("Join a Session");

	//connect(connectAction_, SIGNAL(triggered(bool)), this, SLOT(changeConnectionState(bool)));

	//Password Line Edit
	passwordEdit_ = new QLineEdit();
	passwordEdit_->setEnabled(false);
	passwordEdit_->setToolTip("Enter Session Key");

	directorListBox_ = new QComboBox;
	directorListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	directorListBox_->setToolTip("Select a Director");

	proxyListBox_ = new QComboBox;
	proxyListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	proxyListBox_->addItem("No Proxy",QUuid().toString());
	proxyListBox_->setToolTip("Select a Neural Proxy");
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

	toolBar_->addWidget(new QLabel(Picto::Names->directorAppName + ": ", this));
	toolBar_->addWidget(directorListBox_);
	toolBar_->addWidget(new QLabel(Picto::Names->proxyServerAppName + ": ", this));
	toolBar_->addWidget(proxyListBox_);
	toolBar_->addWidget(new QLabel("Session Key:"));
	toolBar_->addWidget(passwordEdit_);
	toolBar_->addAction(connectAction_);

	//------ Main layout -----------
	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(toolBar_);
	toolbarLayout->addStretch();

	//----------Plots---------------
	neuralDataViewer_ = new NeuralDataViewer(engine_);
	connect(updater_.data(),SIGNAL(framePresented(double)),neuralDataViewer_,SLOT(setBehavioralTime(double)));

	//--------Run Info--------------
	currentRunViewer_ = new TaskRunViewer();
	connect(	currentRunViewer_,
				SIGNAL(taskRunDataChanged(qulonglong)),
				this,
				SLOT(modifyRunDataUnit(qulonglong))
			);

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

	QVBoxLayout *stimulusLayout = new QVBoxLayout;
	stimulusLayout->addWidget(visualTargetHost_);
	foreach(QSharedPointer<Picto::VirtualOutputSignalController> cont,outSigControllers_)
	{
		outputSignalsWidgets_.push_back(new OutputSignalWidget(cont));
		stimulusLayout->addWidget(outputSignalsWidgets_.back());
	}
	QWidget *stimulusWidget = new QWidget();
	stimulusWidget->setLayout(stimulusLayout);

	mainTabbedFrame_ = new QTabWidget(this);
	mainTabbedFrame_->addTab(stimulusWidget,"Behavioral");
	mainTabbedFrame_->addTab(neuralDataViewer_,"Neural");
	mainTabbedFrame_->addTab(currentRunViewer_,"Run Info");
	mainTabbedFrame_->setTabEnabled(0,false);
	mainTabbedFrame_->setTabEnabled(1,false);
	
	QHBoxLayout *operationLayout = new QHBoxLayout;
	operationLayout->addLayout(leftPane);
	operationLayout->addWidget(mainTabbedFrame_);
	operationLayout->setStretch(0,0);
	operationLayout->addStretch();


	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	mainLayout->addLayout(operationLayout);
	//mainLayout->addStretch(1);
	mainLayout->addWidget(statusBar_);
	
	setLayout(mainLayout);

}

/*! \brief Creates the CommandChannel objects that will be used to gather Session data and send commands to change the Session state.
 */
void RemoteViewer::setupServerChannel()
{
	serverChannel_ = new Picto::CommandChannel(observerId_,"WORKSTATION",this);
	engineSlaveChannel_ = new Picto::CommandChannel(observerId_,"WORKSTATION",this);
	neuralSlaveChannel_ = new Picto::CommandChannel(observerId_,"WORKSTATION",this);
}

/*! \brief Called when the play button is pressed.  Calls play() or resume() depending on the current run state.
 */
void RemoteViewer::playAction()
{
	if(currState_ == StoppedSession)
		play();
	else
		resume();
}

/*! \brief Tells the Server to start running the currently selected Task.
 */
void RemoteViewer::play()
{
	QString modifiedTaskName = taskListBox_->currentText();
	modifiedTaskName = modifiedTaskName.simplified().remove(' ');
	sendTaskCommand("start:"+modifiedTaskName);
}

/*! \brief Tells the Server to resume running the current Task from its paused state.
 */
void RemoteViewer::resume()
{
	sendTaskCommand("resume");
}

/*! \brief Tells the Server to pause the current Task.
 */
void RemoteViewer::pause()
{
	sendTaskCommand("pause");
}

/*! Tells the server to stop running the current Task.
 */
void RemoteViewer::stop()
{
	sendTaskCommand("stop");
}

/*! \brief Requests that a new reward be given to the Experiment subject with the default value.
 */
void RemoteViewer::reward()
{
	sendTaskCommand(QString("reward:%1").arg(rewardChannel_));
}

/*! \brief Requests that the default reward duration be changed to the current value in the UI spinbox.
 */
void RemoteViewer::changeRewardDuration()
{
	sendTaskCommand(QString("reward:%1").arg(rewardChannel_),QString::number(rewardQuantity_->value()));
	rewardDurChangeTimer_.restart();
}

/*! \brief Requests that the Session's TaskRun data for the TaskRun with the input runId be updated with the latest values
 *	from this Workstation (set in the TaskRunViewer).
 */
void RemoteViewer::modifyRunDataUnit(qulonglong runId)
{
	Q_ASSERT(currSessionDataPack_);
	if(!currSessionDataPack_)
		return;
	QSharedPointer<TaskRunDataUnit> runUnit = currSessionDataPack_->getTaskRunByRunId(runId);
	Q_ASSERT(runUnit);
	if(runUnit.isNull())
		return;
	sendTaskCommand("updatedata",runUnit->toXml());
	updateSessionDataPackage(true);
}

/*! \brief Loads runtime Property initValues from a previous Session to replace those in the current Experiment.
 *	\warning We haven't tested this in a while and it may not be working.  Regardless, it might be good to do this
 *	differently and allow operators to save current Property configurations and restore them later.
 */
void RemoteViewer::LoadPropValsFromFile()
{
	QString filename = QFileDialog::getOpenFileName(this,
			tr("Load Properties From Session"),".","Sqlite files (*.sqlite)");
	if(propertyFrame_)
		static_cast<PropertyFrame*>(propertyFrame_)->updatePropertiesFromFile(filename);
}

/*! \brief Called whenever an Experiment Parameter (runtime Property) from the left side bar is changed.  
 *	Sends a request to the server to change that value in the Experiment.
 *	\details In practice, what happens is the value is sent to the Director which loads it into the initValue
 *	of the corresponding Property.  When the Property's parent next re-enters scope, this InitValue is copied
 *	to the runtime value such that the value only goes into effect at a safe defined time.
 */
void RemoteViewer::parameterMessageReady(QSharedPointer<Property> changedProp)
{
	Q_ASSERT(changedProp);
	QString path = changedProp->getPath();
	QString name = changedProp->getName();
	if(changedProp->getAssetId()<0)
		return;
	sendTaskCommand(QString("parameter:%1").arg(QString::number(changedProp->getAssetId())),changedProp->initValToUserString());
}

/*! \brief Tells the Server that the operator just clicked on the main window, along with where the click occured.
 *	\details This is useful for Experiments that require spatial feedback like the Eye Calibration task.
 */
void RemoteViewer::operatorClickDetected(QPoint pos)
{
	if(currState_ == RunningSession)
		sendTaskCommand(QString("click:%1,%2").arg(pos.x()).arg(pos.y()));
}

/*! \brief Called when the Zoom slider value changes.  Adjusts the zoom level in the Viewer accordingly.
 */
void RemoteViewer::zoomChanged(int zoom)
{
	if(!pixmapVisualTarget_)
		return;
	//Fix zoom so that each tick is worth 3.333%
	float fixedZoom = float(zoom*.05);
	zoomPercentage_->setText(QString::number(fixedZoom*100)+QString("%"));
	zoomValue_ = fixedZoom;
	//Set the zoomChanged_ flag so that it will be picked up by the state machine the next time it cares.
	zoomChanged_ = true;
}

/*! \brief Sets the status message to the passed in string.
 *	@param status The new status string to be displayed.
 *	@param highPriority If true, the input status will override the regular default status
 *		and be displayed for a short time after which the default status will be shown again.
 *		If false, the status will be set as the new default status.
 */
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

/*! \brief Generates the task list combobox if it is empty.
 */
void RemoteViewer::generateTaskList()
{
	if(taskListBox_->count() == 0)
	{
		taskListBox_->addItems(activeExperiment_->getTaskNames());
	}
}

/*! \brief updates the combo boxes containing Director and Proxy instances.
 *
 *	\details This function asks the Server for a current list of Director/Proxy instances, and 
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

	//Update reward duration box according to the current director
	if(!rewardQuantity_->hasFocus() && (rewardDurChangeTimer_.elapsed() > REWARD_CHANGE_DELAY))
	{
		foreach(RemoteViewer::ComponentInstance director,directors)
		{
			if(director.id == directorListBox_->itemData(directorListBox_->currentIndex()).toString())
			{
				if(director.rewardMap_.contains(rewardChannel_))
					rewardQuantity_->setValue(director.rewardMap_[rewardChannel_]);
				break;
			}
		}
		
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

/*! \brief Called from runState() to retrieve Neural data from the PictoServer and add it to the NeuralDataViewer.
 */
void RemoteViewer::updateNeuralData()
{
	//Don't run this too often
	if(neuralDataTimer_.elapsed() < NEURAL_DATA_UPDATE_PERIOD)
		return;
	neuralDataTimer_.restart();	

	mainTabbedFrame_->setTabIcon(1,QIcon(":/icons/circle.png"));
	while(neuralSlaveChannel_->incomingResponsesWaiting())
		neuralSlaveChannel_->getResponse();
	if(!neuralSlaveChannel_->assureConnection(100))
	{
		return;
	}

	//Collect the data from the server
	QString commandStr = QString("GETDATA LatestNeural:%1 PICTO/1.0").arg(neuralDataViewer_->getLatestId());
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(commandStr));
	QSharedPointer<Picto::ProtocolResponse> response;

	neuralSlaveChannel_->sendRegisteredCommand(command);
	QString commandID = command->getFieldValue("Command-ID");
	//qDebug(QString("Sent command: %1 at Time:%2").arg(commandID).arg(command->getFieldValue("Time-Sent")).toLatin1());

	do
	{
		QCoreApplication::processEvents();
		if(!neuralSlaveChannel_->waitForResponse(1000))
		{
			return;
		}
		response = neuralSlaveChannel_->getResponse();
	}while(!response || response->getFieldValue("Command-ID") != commandID);

	//Response not 200:OK
	if(response->getResponseCode() != Picto::ProtocolResponseType::OK)
	{
		return;
	}
	
	QByteArray xmlFragment = response->getContent();
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));

	while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "Data");

	if(xmlReader->atEnd())
	{
		return;
	}
	
	bool hadData = false;
	xmlReader->readNext();
	while(!xmlReader->isEndElement() && xmlReader->name() != "Data" && !xmlReader->atEnd())
	{
		if(xmlReader->name() == "LFPDataUnitPackage")
		{
			LFPDataUnitPackage pack;
			pack.fromXml(xmlReader);
			neuralDataViewer_->addLFPData(pack);
			hadData = true;
		}
		else if(xmlReader->name() == "NDU")
		{
			NeuralDataUnit unit;
			unit.fromXml(xmlReader);
			neuralDataViewer_->addSpikeData(unit);
			hadData = true;
		}
		xmlReader->readNext();
	}
	//Only once we actually receive neural data do we enable the neural tab of the mainTabbedFrame
	mainTabbedFrame_->setTabEnabled(1,true);
	if(hadData)
	{
		mainTabbedFrame_->setTabIcon(1,QIcon(":/icons/runningWorkstation.png"));
	}
}

/*! \brief Returns the current status (ComponentStatus) of the remote Director with the input Component Id.
 */
RemoteViewer::ComponentStatus RemoteViewer::directorStatus(QString id)
{
	QList<ComponentInstance> directors = currDirectorList_;

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

/*! \brief Returns the current status (ComponentStatus) of the remote Proxy with the input Component Id.
 */
RemoteViewer::ComponentStatus RemoteViewer::proxyStatus(QString id)
{
	QList<ComponentInstance> proxies = currProxyList_;

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

/*! \brief Returns a list of Directors and their properties
 *
 *	\details This sends out a DIRECTORLIST command, and then parses the response into a 
 *	QList of ComponentInstances (really just a struct with a couple of strings).
 *	If something goes wrong, or no director instances are found, an empty
 *	list is returned.
 */  
QList<RemoteViewer::ComponentInstance> RemoteViewer::getDirectorList()
{
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("DIRECTORLIST / PICTO/1.0"));
	QSharedPointer<Picto::ProtocolResponse> response;

	response = sendCommandGetReply(command);
	if(response.isNull())
		return currDirectorList_;
	currDirectorList_.clear();

	QByteArray xmlFragment = response->getContent();
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));

	while(!xmlReader->atEnd())
	{
		xmlReader->readNext();

		if(xmlReader->isStartElement() && xmlReader->name() == "Director")
		{
			ComponentInstance director;

			while(!xmlReader->atEnd())
			{
				xmlReader->readNext();

				if(xmlReader->name() == "Name" && xmlReader->isStartElement())
				{
					director.name = xmlReader->readElementText();
				}
				else if(xmlReader->name() == "Address" && xmlReader->isStartElement())
				{
					director.address = xmlReader->readElementText();
				}
				else if(xmlReader->name() == "Id" && xmlReader->isStartElement())
				{
					director.id = xmlReader->readElementText();
				}
				else if(xmlReader->name() == "Status" && xmlReader->isStartElement())
				{
					director.status = xmlReader->readElementText();
				}
				else if(xmlReader->name() == "Session-ID" && xmlReader->isStartElement())
				{
					director.sessionID = xmlReader->readElementText();
				}
				else if(xmlReader->name() == "Details" && xmlReader->isStartElement())
				{
					QString dataStr = xmlReader->readElementText();
					DirectorData directorData;
					directorData.fromXml(dataStr);
					director.rewardMap_ = directorData.getRewardMap();
				}
				else if(xmlReader->name() == "Director" && xmlReader->isEndElement())
				{
					currDirectorList_.append(director);
					break;
				}
			}
		}
	}
	return currDirectorList_;
}

/*! \brief Returns a list of Proxy components and their addresses.
 *
 *	\details This sends out a PROXYLIST command, and then parses the response into a 
 *	QList of ComponentInstances (really just a struct with a few strings).
 *	If something goes wrong, or no Proxy instances are found, an empty
 *	list is returned.
 */  
QList<RemoteViewer::ComponentInstance> RemoteViewer::getProxyList()
{
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("PROXYLIST / PICTO/1.0"));
	QSharedPointer<Picto::ProtocolResponse> response;

	response = sendCommandGetReply(command);
	if(response.isNull())
		return currProxyList_;
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

/*! \brief Retrieves the latest Task Run data from the Picto Server (run names, notes, saved values, etc) and
 *	sets them to the TaskRunViewer.
 *	@param immediate If true, the retrieval / set will occur immediately.  If false, the function will only
 *	actually perform the retrieval/set once for every SESSION_DATA_UPDATE_PERIOD milliseconds.
 */
void RemoteViewer::updateSessionDataPackage(bool immediate)
{
	if(!immediate)
	{
		//Don't run this too often
		if(sessionDataTimer_.elapsed() < SESSION_DATA_UPDATE_PERIOD)
			return;
	}
	sessionDataTimer_.restart();	

	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("GETDATA SessionData PICTO/1.0"));
	QSharedPointer<Picto::ProtocolResponse> response;

	response = sendCommandGetReply(command);
	if(response.isNull())
		return;

	QByteArray xmlFragment = response->getContent();
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));

	while(!xmlReader->atEnd())
	{
		xmlReader->readNext();

		if(xmlReader->isStartElement() && xmlReader->name() == "SessionDataPackage")
		{
			QSharedPointer<Picto::SessionDataPackage> newPack(new Picto::SessionDataPackage());
			newPack->fromXml(xmlReader);
			if(newPack)
				currSessionDataPack_ = newPack;

			if(currSessionDataPack_ && currSessionDataPack_->getNumRuns())
			{
				currentRunViewer_->enableEditing(isAuthorized_);
				currentRunViewer_->setTaskRunData(currSessionDataPack_->getRunsMap());
				mainTabbedFrame_->setTabEnabled(2,true);
				mainTabbedFrame_->setTabIcon(2,currentRunViewer_->getLatestRunIcon());
			}
		}
	}
}

/*! \brief Starts a new session
 *
 *	\details This starts a new session on the server.  The act of starting a session basically 
 *	ties together the various components involved (Director, and Proxy) and 
 *	starts a new Session database file on the Server.  Note that the Workstation instance isn't really 
 *	tied to the session.  This is intentional, and makes it easy to remotely view and
 *	control running sessions from multiple Workstations.  It also allows Experiments to continue even
 *	if no Workstations are watching it.
 *
 *	\note Starting the session is really just a remote command and has no effect 
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

	if(!myDesignRoot_->compiles())
	{
		QMessageBox msg;
		msg.setText("Local experiment has errors.  Cannot start session.");
		msg.setIconPixmap(QPixmap(":/icons/x.png"));
		msg.exec();
		return false;
	}

	QByteArray dataXml = myDesignRoot_->getDesignRootText().toUtf8();

	QSharedPointer<DesignConfig> designConfig = experiment_->getDesignConfig();
	if(!designConfig)
		return false;
	dataXml.append(designConfig->toXml().toUtf8());
	startSessCommand->setContent(dataXml);
	startSessCommand->setFieldValue("Content-Length",QString("%1").arg(dataXml.length()));
	startSessCommand->setFieldValue("Observer-ID",observerId_.toString());
	startSessCommand->setFieldValue("Password",passwordEdit_->text());
	startSessCommand->setFieldValue("Experiment",myDesignRoot_->getDesignName());

	QSharedPointer<Picto::ProtocolResponse> loadExpResponse;

	loadExpResponse = sendCommandGetReply(startSessCommand,10000);
	if(loadExpResponse.isNull())
		return false;

	if(loadExpResponse->getResponseCode() == Picto::ProtocolResponseType::NotFound)
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
 *	\details This is the opposite of startSession().  Again note that this function has no real
 *	effect on our local workstation instance.  See the comments on startSession() for
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

	QSharedPointer<Picto::ProtocolCommand> endSessCommand(new Picto::ProtocolCommand("ENDSESSION "+sessionId_.toString()+" PICTO/1.0"));
	endSessCommand->setFieldValue("Observer-ID",observerId_.toString());

	QSharedPointer<Picto::ProtocolResponse> endSessResponse;

	endSessResponse = sendCommandGetReply(endSessCommand,5000);
	if(endSessResponse.isNull())
		return false;

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

/*! \brief Joins to a currently running session. In order to observe or control it.
 *
 *	\details Joining to a Session is the act of connecting this Workstation to an already 
 *	running Session.  This is what allows us to see the Session on the Workstation
 *	screen.
 *
 *	\note We always keep the latest active Experiment in memory so that this function
 *	will go quickly if we rejoin to a Session that we were previously connected too.
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

	joinSessResponse = sendCommandGetReply(joinSessCommand,3000);
	if(joinSessResponse.isNull())
		return false;

	if(joinSessResponse->getResponseCode() != Picto::ProtocolResponseType::OK)
	{
		setStatus(tr("Unexpected response to JOINSESSION. Unable to join session."),true);
		return false;
	}

	content = joinSessResponse->getDecodedContent();
	QUuid remoteSessionId = QUuid(joinSessResponse->getFieldValue("SessionID"));
	if(remoteSessionId == QUuid())	
	{
		setStatus(tr("SessionID not found in response from server."),true);
		return false;
	}

	//If we didn't already, we'll need to also put the experiment in activeExperiment
	if(sessionId_.isNull() || (remoteSessionId != sessionId_))
	{
		sessionId_ = remoteSessionId;
		//Extract the design/experiment xml
		activeDesignRoot_ = QSharedPointer<DesignRoot>(new DesignRoot());
		if(!activeDesignRoot_->resetDesignRoot(QString(content)))
		{
			setStatus(tr("Unable to deserialize Experiment returned by JOINSESSION"),true);
			return false;
		}
		activeDesignRoot_->enableRunMode(true);
		activeExperiment_ = activeDesignRoot_->getExperiment().staticCast<Experiment>();
	}
	activeExperiment_->setEngine(engine_);
	updater_->initForNewSession();
	slaveExpDriver_ = QSharedPointer<SlaveExperimentDriver>(new SlaveExperimentDriver(activeExperiment_,updater_));
	connect(slaveExpDriver_.data(),SIGNAL(taskChanged(QString)),this,SLOT(currTaskChanged(QString)));

	serverChannel_->setSessionId(sessionId_);
	engineSlaveChannel_->setSessionId(sessionId_);
	neuralSlaveChannel_->setSessionId(sessionId_);

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




/*! \brief This does not do anything and should probably be deleted.
 *
 */
bool RemoteViewer::syncExperiment()
{
	return true;
}

/*! \brief Causes the engine to stop the currently running experiment before the next time updateState() runs.
 *
 */
void RemoteViewer::stopExperiment()
{
	Q_ASSERT(engine_);
	engine_->stop();
}

/*! \brief Disjoins a currently running session
 *
 *	\details This is the opposite of the join function.  This detaches us from a running session 
 *	(but it doesn't actually end the session).
 */
bool RemoteViewer::disjoinSession()
{
	serverChannel_->setSessionId(QUuid());
	engineSlaveChannel_->setSessionId(QUuid());
	neuralSlaveChannel_->setSessionId(QUuid());
	return true;
}

/*!	\brief Gets feedback from the operator as whether they would like to end the current session.
 *	\note If the user is not authorized on the current Session, this will always return false.
 */
bool RemoteViewer::shouldEndSession()
{
	if(!isAuthorized_)
		return false;
	int	r = QMessageBox::warning(this,Picto::Names->workstationAppName,
		tr("You are disconnecting from a session that is not currently running an experiment.\n"
		"Would you like to end the session as well?"),
		   QMessageBox::Yes|QMessageBox::No);

	if(r == QMessageBox::Yes)
	{
		return true;
	}
	return false;
}

/*! \brief Takes care of sending an input cmd ProtocolCommand to the Server and returning the received response.
 *	\details If the response times out, this function returns an empty pointer.  If nonDefaultTimeoutMs is 
 *	not set, an adaptiveTimeout is used that is adjusted based on historical timeouts and reply times.
 */
QSharedPointer<Picto::ProtocolResponse> RemoteViewer::sendCommandGetReply(QSharedPointer<Picto::ProtocolCommand> cmd, int nonDefaultTimeoutMs)
{
	QTime timer;
	timer.start();
	serverChannel_->sendRegisteredCommand(cmd);
	//qDebug("Sent " + cmd->getMethod().toLatin1() + " Command.");
	bool useAdaptiveTimeout = (nonDefaultTimeoutMs == -1);
	QString method = cmd->getMethod();
	int* responseDelay = &nonDefaultTimeoutMs;
	if(useAdaptiveTimeout)
	{
		if(!adaptiveResponseDelays_.contains(method))
			adaptiveResponseDelays_[method] = MIN_ADAPTIVE_TIMEOUT;
		responseDelay = &adaptiveResponseDelays_[method];
	}
	QString commandID = cmd->getFieldValue("Command-ID");
	QSharedPointer<Picto::ProtocolResponse> cmdResponse;
	pendingResponses_[commandID] = cmdResponse;
	//Get the response to this command
	do
	{
		//Update the delay according to the time since sending the command
		int delay = (*responseDelay)-timer.elapsed();
		if(delay < 0)
			delay = 0;
		//Wait up to the maximum delay for a response
		serverChannel_->waitForResponse(delay);

		//Insert all responses received into pendingResponses_ hash.
		while(serverChannel_->incomingResponsesWaiting())
		{
			QSharedPointer<Picto::ProtocolResponse> response = serverChannel_->getResponse();	
			if(pendingResponses_.contains(response->getFieldValue("Command-ID")))
			{
				pendingResponses_[response->getFieldValue("Command-ID")] = response;
			}
		}

		//Check if our reponse is in the pending responses hash.
		if(pendingResponses_.contains(commandID))
		{	//We got our response.  Break out of the loop
			break;
		}
	//Keep looping until the responseDelay time has passed or we break out (after getting our response), whichever comes first.
	}while(timer.elapsed() < (*responseDelay));

	//Get our response from the pendingResponses_ hash and remove it.
	cmdResponse = pendingResponses_[commandID];
	pendingResponses_.remove(commandID);

	//If there was no response.  Print a status message indicating this.
	if(cmdResponse.isNull())
	{
		QString statusMessage = method + " command timed out.";
		if(useAdaptiveTimeout)
		{
			int lastAdaptDelay = *responseDelay;
			(*responseDelay) *= 2;
			if((*responseDelay) > MAX_ADAPTIVE_TIMEOUT)
				(*responseDelay)  = MAX_ADAPTIVE_TIMEOUT;
			if(lastAdaptDelay != (*responseDelay))
				statusMessage += " Compensating... Timeout: " + QString::number((*responseDelay)) + " ms.";
			//qDebug(cmd->getMethod().toLatin1() + " command timed out.");
		}
		if(currState_ != WaitForConnect)
		{	//If we're not even connected, there will obviously be timeouts here.  No need to keep on whining about them.
			setStatus(statusMessage,true);
		}
		//Return an empty reply, since we didn't get the one we wanted.
		return QSharedPointer<Picto::ProtocolResponse>();
	}

	//If we're here, we got a response.  Update the adaptive timeout for this command type according to the time it took to 
	//receive the reply.
	//qDebug(cmd->getMethod().toLatin1() + " command reply received.");
	if(useAdaptiveTimeout)
	{
		(*responseDelay) = 2*timer.elapsed();
		if((*responseDelay)<MIN_ADAPTIVE_TIMEOUT)
			(*responseDelay) = MIN_ADAPTIVE_TIMEOUT;
		if((*responseDelay) > MAX_ADAPTIVE_TIMEOUT)
			(*responseDelay)  = MAX_ADAPTIVE_TIMEOUT;
	}

	//Return the response.
	return cmdResponse;
}

/*! Sends a Task command with the given target.  Returns true if execution was successful.
 *	\details TASK commands are responsible for most of the commands that change the Experimental
 *	state.  They handle things like play, stop, pause, resume, reward, etc.
 */
bool RemoteViewer::sendTaskCommand(QString target, QString msgContent)
{
	QSharedPointer<Picto::ProtocolCommand> cmd(new Picto::ProtocolCommand("TASK "+target+" PICTO/1.0"));
	cmd->setFieldValue("Observer-ID",observerId_.toString());
	if(!msgContent.isEmpty())
	{
		QByteArray byteContent = msgContent.toLatin1();
		cmd->setContent(byteContent);
		cmd->setFieldValue("Content-Length",QString::number(byteContent.length()));
	}
	
	QSharedPointer<Picto::ProtocolResponse> cmdResponse;

	cmdResponse = sendCommandGetReply(cmd);
	if(cmdResponse.isNull())
		return false;

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

/*! \brief Called whenever a new task is selected in the Combobox.  Updates the values in the left hand PropertyFrame 
 *	to reflect the new Task.
 */
void RemoteViewer::taskListIndexChanged(int)
{
	QSharedPointer<Task> task = activeExperiment_->getTaskByName(taskListBox_->currentText());
	if(!task)
		return;
	qobject_cast<PropertyFrame*>(propertyFrame_)->setTopLevelDataStore(task.staticCast<DataStore>());
}

/*! \brief Assures that the Server channels are connected.  Returns false, sets the status accordingly and attempts to reconnect
 *	if the connection is lost.
 * \details This function is called for each runState() call.
 */
bool RemoteViewer::assureChannelConnections()
{
	bool hadDisconnect = false;
	if(!serverChannel_->isConnected())
	{
		if(currState_ != WaitForConnect)
			setStatus(tr("Viewer Channel lost connection.  Attempting reconnect"),true);
		serverChannel_->connectToServer();
		if(!serverChannel_->isConnected())
		{
			if(currState_ != WaitForConnect)
				setStatus(tr("Viewer Channel failed to reconnect"),true);
			hadDisconnect = true;
		}
		else
			setStatus(tr("Viewer Channel reconnected"),true);
	}
	if(!engineSlaveChannel_->isConnected())
	{	//Since nothing happens on this channel except during running and paused states, it disconnects
		//all the time whenever the server times it out.  It should always be ready though, so we keep on
		//reconnecting it, but we don't tell the user when it gets disconnected and we don't let that
		//affect the remoteViewer state.
		engineSlaveChannel_->connectToServer();
	}
	if(!neuralSlaveChannel_->isConnected())
	{
		neuralSlaveChannel_->connectToServer();
	}
	return !hadDisconnect;
}

/*! \brief Called when the running Task on the Director changes.  Updates the currently
 *	selected Task in the combobox accordingly (which results in updating the left hand PropertyFrame.
 *	\note This is necessary since another Workstation might be controlling the same Session that this
 *	one is connected to.
 */
void RemoteViewer::currTaskChanged(QString task)
{
	if(task == taskListBox_->currentText())
		return;
	for(int i=0;i<taskListBox_->count();i++)
	{
		if(taskListBox_->itemText(i) == task)
		{	
			taskListBox_->setCurrentIndex(i);
			return;
		}
	}
}