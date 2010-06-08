/*!	\brief	This views and controls a remotely running experiment
 *
 *	This is the viewer that will be used while an experiment is running.  It has the
 *	following features:
 *		Allows starting, stopping, and pausing of remotely running experiments
 *		Displays behavioral input (eye tracker) as a moving cursor
 *		Displays current state of remote experiment
 */
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

class RemoteViewer : public Viewer
{
	Q_OBJECT
public:
	RemoteViewer(QWidget *parent=0);
	QString type() { return "Remote"; };

public slots:
	void init();  //Called just before displaying the viewer
	void deinit();	//Called just after the user switches out of the viewer

private slots:
	void play();
	void pause();
	void stop();

	void changeConnectionState();
	
	void updateStatus();
	void updateDirectorList();
	void updateActions();

private:
	typedef struct
	{
		QString name;
		QString status;
		QString address;
	}DirectorInstance;



	void setupEngine();
	void setupServerChannel();
	void setupUi();
	void generateTaskList();

	void setStatus(QString status);
	bool sendTaskCommand(QString target);
	bool startSession();
	bool endSession();

	bool directorIsRunning(QString addr);
	QList<DirectorInstance> getDirectorList();

	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;

	Picto::VisualTargetHost *visualTargetHost_;

	QAction *playAction_;
	QAction *pauseAction_;
	QAction *stopAction_;
	QAction *connectAction_;
	//QAction *disconnectAction_;

	QToolBar *toolBar_;
	QComboBox *taskListBox_;
	QComboBox *directorListBox_;
	QLabel *statusBar_;

	QTimer *updateTimer_;

	Picto::CommandChannel *serverChannel_;
	Picto::ServerDiscoverer *serverDiscoverer_;

	enum Status {Stopped, Running, Paused};
	Status status_;
	bool startedSession_;

	QUuid sessionId_;
};

#endif