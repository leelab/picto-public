#ifndef _REMOTE_VIEWER_H_
#define _REMOTE_VIEWER_H_

#include "viewer.h"
#include "../../common/engine/pictoengine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "../../common/compositor/VisualTargetHost.h"
#include "../../common/network/CommandChannel.h"
#include "../../common/network/ServerDiscoverer.h"

#include <QUuid>

class QAction;
class QToolBar;
class QComboBox;
class QLabel;

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

	QString type() { return "Remote"; };
	bool aboutToQuit();

public slots:
	void init();  //Called just before displaying the viewer
	void deinit();	//Called just after the user switches out of the viewer

private slots:
	void play();
	void pause();
	void stop();
	void reward();

	void changeConnectionState(bool checked);
	
	void updateStatus();
	void updateLists();
	void updateActions();

	void checkForTimeouts();

private:
	typedef struct
	{
		QString name;
		QString status;
		QString address;
	}DirectorInstance;

	struct ProxyServerInfo
	{
		QString name;
		int id;
	};

	enum DirectorStatus {Error, Idle, Stopped, Paused, Running};

	void setupEngine();
	void setupServerChannel();
	void setupUi();
	void generateTaskList();

	void setStatus(QString status);
	bool sendTaskCommand(QString target);
	bool startSession();
	bool endSession();
	bool joinSession();
	bool disjoinSession();

	DirectorStatus directorStatus(QString addr);
	QList<DirectorInstance> getDirectorList();

	QList<ProxyServerInfo> getProxyList();

	//Since we don't want the experiemnt to be changed while it is being run
	//we keep a local copy here.
	Picto::Experiment *activeExperiment_;

	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;

	Picto::VisualTargetHost *visualTargetHost_;

	QAction *playAction_;
	QAction *pauseAction_;
	QAction *stopAction_;
	QAction *rewardAction_;
	QAction *connectAction_;

	QToolBar *toolBar_;
	QComboBox *taskListBox_;
	QComboBox *directorListBox_;
	QComboBox *proxyListBox_;
	QLabel *statusBar_;

	QTimer *updateTimer_;
	QTimer *timeoutTimer_;

	Picto::CommandChannel *serverChannel_;
	Picto::CommandChannel *engineSlaveChannel_;
	Picto::CommandChannel *behavioralDataChannel_;
	Picto::ServerDiscoverer *serverDiscoverer_;

	DirectorStatus localStatus_;
	
	bool startedSession_;
	bool enableTaskCommands_;

	QUuid sessionId_;
	QUuid observerId_;

	int rewardChannel_;  //Channel used for issuing manual rewards

};

#endif