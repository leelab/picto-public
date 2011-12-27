#ifndef _REMOTE_VIEWER_H_
#define _REMOTE_VIEWER_H_

#include "../viewer.h"
#include "../../common/engine/pictoengine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "../../common/compositor/VisualTargetHost.h"
#include "../../common/network/CommandChannel.h"
#include "../../common/network/ServerDiscoverer.h"

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
	void LoadPropValsFromFile();
	void parameterMessageReady(QSharedPointer<Property> changedProp);
	void operatorClickDetected(QPoint pos);
	void zoomChanged(int zoom);











	void updateState();
	void updateEngine();
















	//void changeConnectionState(bool checked);
	void updateComponentLists(bool immediate = false);


private:


	void enterState();
	void runState();
	QTime componentListsTimer_;








	enum ViewerState
	{
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
	} latestTrigger_;

	enum EngineTrigger
	{
		NoEngineTrigger,
		StartEngine,
		StopEngine
	} engineTrigger_;

	
	QTimer *stateUpdateTimer_;
	QTimer *engineUpdateTimer_;
	QString initState_;
	bool refreshSplash_;














	typedef struct
	{
		QString name;
		QString status;
		QString address;
		QString id;
		QString sessionID;
	}ComponentInstance;

	enum ComponentStatus {NotFound, Idle, Ending, Stopped, Paused, Running};

	void setupEngine();
	void setupServerChannel();
	void setupUi();
	void generateTaskList();
	void updateUI();

	void setStatus(QString status);
	bool sendTaskCommand(QString target, QString msgContent = "");
	bool startSession();
	bool endSession();
	bool joinSession();
	bool syncExperiment();
	void stopExperiment();
	bool disjoinSession();

	ComponentStatus directorStatus(QString id);
	ComponentStatus proxyStatus(QString id);

	QList<ComponentInstance> getDirectorList();
	QList<ComponentInstance> getProxyList();

	//Since we don't want the experiemnt to be changed while it is being run
	//we keep a local copy here.
	QSharedPointer<Picto::Experiment> activeExperiment_;

	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;

	Picto::VisualTargetHost *visualTargetHost_;
	QSharedPointer<Picto::Experiment> experiment_;
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
	QLabel *statusBar_;

	QTimer *updateTimer_;
	QTimer *reconnectTimer_;
	QTimer *timeoutTimer_;

	Picto::CommandChannel *serverChannel_;
	Picto::CommandChannel *engineSlaveChannel_;
	Picto::ServerDiscoverer *serverDiscoverer_;

	ComponentStatus localStatus_;
	
	bool isAuthorized_;

	QUuid sessionId_;
	QUuid observerId_;

	int rewardChannel_;  //Channel used for issuing manual rewards

	QList<ComponentInstance> currDirectorList_;
	QList<ComponentInstance> currProxyList_;
	bool channelSignalsConnected_;
	QWeakPointer<Asset> lastTask_;
	QWeakPointer<Asset> lastActiveExperiment_;
	bool runningEngine_;
	bool updatingState_;

private slots:
	void taskListIndexChanged(int index);
	bool assureChannelConnections();

};

#endif