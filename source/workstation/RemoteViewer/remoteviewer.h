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
#include "neuraldataviewer.h"
#include "TaskRunViewer.h"

#include <QUuid>
#include <QFuture>
using namespace Picto;

class QAction;
class QToolBar;
class QComboBox;
class QLabel;
class QSlider;
class QSpinBox;
class QLineEdit;
class QTabWidget;

/*!	\brief	This views and controls a remotely running experiment
 *
 *	This is the viewer that will be used while an experiment is running.  It has the
 *	following features:
 *		Allows starting, stopping, and pausing of remotely running experiments
 *		Displays behavioral input (eye tracker) as a moving cursor
 *		Displays current state of remote experiment
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
	void updateEngine();
















	//void changeConnectionState(bool checked);
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

	enum ViewerTrigger
	{
		NoViewerTrigger,
		Connected,
		Disconnected,
		JoinSessionRequest,
		StartSessionRequest,
		DisjoinSessionRequest,
		SessionStarted,
		SessionDidntStart,
		SessionStopped,
		SessionPaused,
		SessionRunning,
		EndSessionRequest,
		SessionEnded
	} stateTrigger_;

	enum EngineTrigger
	{
		NoEngineTrigger,
		ContinueEngine,
		StartEngine,
	} engineTrigger_;

	
	QTimer *stateUpdateTimer_;
	QTimer *engineUpdateTimer_;
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
	bool syncExperiment();
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

	//Since we don't want the experiemnt to be changed while it is being run
	//we keep a local copy here.
	QSharedPointer<Picto::Experiment> activeExperiment_;

	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<DesignRoot> myDesignRoot_;
	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> outSigControllers_;

	Picto::VisualTargetHost *visualTargetHost_;
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

	int rewardChannel_;  //Channel used for issuing manual rewards

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

private slots:
	void taskListIndexChanged(int index);
	bool assureChannelConnections();

};

#endif