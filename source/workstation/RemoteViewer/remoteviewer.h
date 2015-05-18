#ifndef _REMOTE_VIEWER_H_
#define _REMOTE_VIEWER_H_

#include "../viewer.h"
#include "../../common/engine/pictoengine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "../../common/compositor/VisualTargetHost.h"
#include "../../common/iodevices/VirtualOutputSignalController.h"
#include "../../common/network/CommandChannel.h"
#include "../../common/network/ServerDiscoverer.h"
#include "../../common/storage/SessionDataPackage.h"
#include "../../common/engine/SlaveExperimentDriver.h"
#include "RemoteStateUpdater.h"
#include "neuraldataviewer.h"
#include "TaskRunViewer.h"

#include <QUuid>
#include <QFuture>


QT_BEGIN_NAMESPACE
class QAction;
class QToolBar;
class QComboBox;
class QLabel;
class QSlider;
class QSpinBox;
class QLineEdit;
class QTabWidget;
QT_END_NAMESPACE


class ViewSelectionFrame;
class OutputWidgetHolder;
namespace Picto {
	class AnalysisSelectorWidget;
}

using namespace Picto;
/*!	\brief	This allows Operators to view and control a remotely running Experimental Session.
 *	\details This is the control panle that the operator uses while an experiment is running.  It has the following
 *	features:
 *		- Allows for Creating/Ending a Session
 *		- Allows for Starting/Stopping a Task Run
 *		- Allows for Pausing/Resuming a Task Run
 *		- Allows for Rewarding the Experimental subject
 *		- Allows for updating the name, notes and saved value of the current Task Run.
 *		- Allows for chaging runtime Propety initValues in the remote Experiment. 
 *		- Displays the current Experimental activity, including the current visual state and behavioral input (eye tracker)
 *				as a moving cursor.
 *		- Displays the current neural activity including Neural spikes and local field potentials
 *		- Displays the names of all Director / Proxy instances currently attached to the Picto Server.
 *	Due to the Server-client architecture of the Picto system, multiple Workstations can be used to control/view the same
 *	Experiment (only Workstations with the correct password entered in the password box can control a Session).  All
 *	Workstations attached to the same Session are all synchronized.  When one changes something about the Experiment,
 *	all others are updated accordingly.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class RemoteViewer : public Viewer
{
	Q_OBJECT
public:
	RemoteViewer(QWidget *parent=0);
	virtual ~RemoteViewer();

	QString type() { return "Remote"; };
	bool aboutToQuit();

public slots:
	void init();  //Called just before displaying the viewer
	void deinit();	//Called just after the user switches out of the viewer

private slots:
	void playAction();
	void play();
	void resume();
	void pause();
	void stop();
	void reward();
	void changeRewardDuration();
	void modifyRunDataUnit(qulonglong runId);
	void LoadPropValsFromFile();
	void parameterMessageReady(QSharedPointer<Property> changedProp);
	void operatorClickDetected(QPoint pos);
	void zoomChanged(int zoom);

	void updateState();

	void updateComponentLists(bool immediate = false);
	void updateNeuralData();


private:
	void enterState();
	void runState();
	void endState();
	QTime componentListsTimer_;
	QTime neuralDataTimer_;
	QTime sessionDataTimer_;
	QTime rewardDurChangeTimer_;

	/* \brief The States for the ViewerState StateMachine.
	 */
	enum ViewerState
	{
		InitState,
		WaitForConnect,
		WaitForJoin,
		JoiningSession,
		CreatingSession,
		StoppedSession,
		PausedSession,
		RunningSession,
		EndingSession
	} currState_;

	/* \brief The various causes for a change in the ViewerState.
	 */
	enum ViewerTrigger
	{
		NoViewerTrigger,
		Connected,
		Disconnected,
		JoinSessionRequest,
		StartSessionRequest,
		DisjoinSessionRequest,
		RejoinSessionRequest,
		SessionStarted,
		SessionDidntStart,
		SessionStopped,
		SessionPaused,
		SessionRunning,
		EndSessionRequest,
		SessionEnded
	} stateTrigger_;
	
	QTimer *stateUpdateTimer_;
	QString initState_;

	typedef struct
	{
		QString name;
		QString status;
		QString address;
		QString id;
		QString sessionID;
		QHash<int,int> rewardMap_;
	}ComponentInstance;

	enum ComponentStatus {NotFound, Idle, Ending, Stopped, Paused, Running};

	void setupEngine();
	void setupServerChannel();
	void setupUi();
	void generateTaskList();
	void updateUI();

	void setStatus(QString status,bool highPriority=false);
	void updateStatus();
	bool sendTaskCommand(QString target, QString msgContent = "");
	bool startSession();
	bool endSession();
	bool joinSession();
	void stopExperiment();
	bool disjoinSession();
	bool shouldEndSession();
	QSharedPointer<Picto::ProtocolResponse> sendCommandGetReply(QSharedPointer<Picto::ProtocolCommand> cmd,int nonDefaultTimeoutMs = -1);

	ComponentStatus directorStatus(QString id);
	ComponentStatus proxyStatus(QString id);

	QList<ComponentInstance> getDirectorList();
	QList<ComponentInstance> getProxyList();
	void updateSessionDataPackage(bool immediate = false);
	void updatePlotOptions();

	//Since we don't want the experiemnt to be changed while it is being run we keep a local copy here.
	QSharedPointer<Picto::Experiment> activeExperiment_;
	QSharedPointer<DesignRoot> activeDesignRoot_;

	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<RemoteStateUpdater> updater_;
	QSharedPointer<SlaveExperimentDriver> slaveExpDriver_;
	QSharedPointer<DesignRoot> myDesignRoot_;

	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> outSigControllers_;

	//! Holds and renders the Task view
	Picto::VisualTargetHost *visualTargetHost_;
	//! The widget that can rearrange the ViewWidgets in the Central View
	ViewSelectionFrame *viewSelectionFrame_;
	//! Widget used to select what analyses are to be run alongside the task
	Picto::AnalysisSelectorWidget* analysisSelector_;
	QSharedPointer<Picto::Experiment> experiment_;
	QHash<QUuid,QSharedPointer<Picto::ProtocolResponse>> pendingResponses_;
	QWidget *propertyFrame_;

	QAction *playAction_;
	QAction *pauseAction_;
	QAction *stopAction_;
	QAction *rewardAction_;
	QSpinBox *rewardQuantity_;
	QAction *connectAction_;
	QLineEdit *passwordEdit_;
	QAction *loadPropsAction_;

	QToolBar *toolBar_;
	QComboBox *taskListBox_;
	QComboBox *directorListBox_;
	QComboBox *proxyListBox_;
	QLabel *activeExpName_;
	QSlider *zoomSlider_;
	QLabel *zoomPercentage_;
	QTabWidget *mainTabbedFrame_;
	QLabel *statusBar_;
	NeuralDataViewer* neuralDataViewer_;
	TaskRunViewer* currentRunViewer_;
	OutputWidgetHolder *outputWidgetHolder_;
	QVector<QWidget *> outputSignalsWidgets_;

	QTimer *updateTimer_;
	QTimer *reconnectTimer_;
	QTimer *timeoutTimer_;

	Picto::CommandChannel *serverChannel_;
	Picto::CommandChannel *engineSlaveChannel_;
	Picto::CommandChannel *neuralSlaveChannel_;
	
	bool isAuthorized_;

	QUuid sessionId_;
	QUuid observerId_;

	//!	Channel used for issuing manual rewards
	int rewardChannel_;  

	QList<ComponentInstance> currDirectorList_;
	QList<ComponentInstance> currProxyList_;
	QSharedPointer<Picto::SessionDataPackage> currSessionDataPack_;
	bool channelSignalsConnected_;
	QWeakPointer<Asset> lastTask_;
	QWeakPointer<Asset> lastActiveExperiment_;
	bool updatingState_;
	QString specialStatus_;
	QString defaultStatus_;
	QTime statusTimer_;
	bool zoomChanged_;
	float zoomValue_;
	QHash<QString,int> adaptiveResponseDelays_;


	//Stolen from PlaybackController.  Should push into a common, base class in order to reduce duplication
	QStringList precacheAnalysisNames(QSharedPointer<DesignRoot> import);
	QString activateAnalyses(QStringList analysisData);
	QStringList getAnalyses(bool bSkipLocal = false);

	QHash<QString, QString> cachedAnalysisNames_;
	QHash<QString, QUuid> cachedAnalysis_;
	uint lastExpHash_;

	int numImportedAnalyses_;

	enum RejoinStatus
	{
		None = 0,
		AwaitingDisconnect,
		AwaitingRejoin

	} awaitingRejoin_;

	QStringList analysesCurrentlyActive_;

private slots:
	void notifyAnalysisSelection(const QString&, bool, bool);
	void taskListIndexChanged(int index);
	bool assureChannelConnections();
	void currTaskChanged(QString task);
	void runStarted(QUuid runId);

};

#endif