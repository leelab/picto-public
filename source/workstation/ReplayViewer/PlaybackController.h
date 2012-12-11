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

/*!	\brief	This controlls experimental playback
 *
 */
class PlaybackController : public QObject
{
	Q_OBJECT
public:
	PlaybackController(QWidget *parent=0);
	virtual ~PlaybackController();

	QString loadSession(QString filename);

	QSharedPointer<Picto::VisualTarget> getVisualTarget();
	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> getOutputSignalControllers();
	double getRunLength();
	void aboutToQuit();
	
public slots:
	void play();
	void pause();
	void stop();
	void jumpToTime(double time);


signals:
	void timeChanged(double time);
	void loading(bool isLoading);
	void runsUpdated(QStringList runs);
	void finishedPlayback();

public slots:
	void setRunSpeed(double value);
	void selectRun(int index);
	void setCurrTime(double time);


private:
	QTimer stateUpdateTimer_;
	QMutex updateStateMutex_;
	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<Picto::Experiment> experiment_;
	QSharedPointer<DesignRoot> designRoot_;
	QSharedPointer<PlaybackThread> playbackThread_;
	QSharedPointer<PlaybackStateUpdater> playbackUpdater_;
	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> outSigControllers_;
	bool expRunning_;
	double currTime_;

	enum Status {Ending, Stopped, Running, Paused};
	Status status_;
	Status newStatus_;
private slots:
	void runExperiment();
};

#endif