#ifndef PLAYBACKCONTROLLER_H_
#define PLAYBACKCONTROLLER_H_
#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QWidget>
#include <QVector>

#include "../../common/engine/pictoengine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "../../common/compositor/VisualTargetHost.h"
#include "../../common/iodevices/VirtualOutputSignalController.h"
#include "PlaybackStateUpdater.h"
#include "PlaybackThread.h"
using namespace Picto;

struct PlaybackCommand 
{
	enum Type {NoCommand, Load, ChangeRun, ChangeSpeed, Play, Pause, Stop, Jump};
	PlaybackCommand(){commandType = NoCommand;};
	PlaybackCommand(Type cmd,QVariant val=0){commandType = cmd;commandData=val;};
	Type commandType;
	QVariant commandData;
};

class PlaybackControllerData
{
public:
	PlaybackControllerData();
	enum Status {None,Idle,Stopped, Running, Paused};
	void setAsSetup();
	bool isSetup();
	void setCurrTime(double val);
	double getCurrTime();
	void setRunSpeed(double val);
	double getRunSpeed();
	void setRunLength(double val);
	double getRunLength();
	void setStatus(Status val);
	PlaybackControllerData::Status getStatus();
	void setNextStatus(Status val);
	PlaybackControllerData::Status getNextStatus();
	void pushCommand(PlaybackCommand cmd, bool toBack=true);
	PlaybackCommand getNextCommand();

private:
	QMutex mutex_;
	bool isSetup_;
	double currTime_;
	double runSpeed_;
	double runLength_;
	Status status_;
	Status nextStatus_;
	QVector<PlaybackCommand> cmds_;

};

/*!	\brief	This controlls experimental playback
 *
 */
class PlaybackController : public QObject
{
	Q_OBJECT
public:
	PlaybackController();
	virtual ~PlaybackController();

	QString loadSession(QString filename);

	QSharedPointer<Picto::VisualTarget> getVisualTarget();
	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> getOutputSignalControllers();
	QSharedPointer<Picto::RenderingTarget> getRenderingTarget();
	double getRunLength();
	void aboutToQuit();
	
public slots:
	void play();
	void pause();
	void stop();
	void jumpToTime(double time);


signals:
	void timeChanged(double time);
	void loadedTo(double maxBehavioral,double maxNeural);
	void loading(bool isLoading);
	void percentLoaded(double percent);
	void runsUpdated(QStringList runs);
	void statusChanged(int status);
	void finishedPlayback();
	void error(QString msg);

public slots:
	void setRunSpeed(double value);
	void selectRun(int index);


private:
	bool waitingForTransition();
	QTimer stateUpdateTimer_;
	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<Picto::Experiment> experiment_;
	QSharedPointer<DesignRoot> designRoot_;
	QSharedPointer<PlaybackThread> playbackThread_;
	QSharedPointer<PlaybackStateUpdater> playbackUpdater_;
	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> outSigControllers_;
	PlaybackControllerData data_;
	bool expRunning_;
private slots:
	void newRunLength(double length);
	void setCurrTime(double time);
	void setup();
	void update();
	void runExperiment();
	

};


#endif