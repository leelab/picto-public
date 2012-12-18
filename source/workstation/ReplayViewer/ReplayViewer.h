#ifndef REPLAYVIEWER_H_
#define REPLAYVIEWER_H_

#include "../viewer.h"
#include "../../common/engine/pictoengine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "../../common/compositor/VisualTargetHost.h"
#include "../../common/iodevices/VirtualOutputSignalController.h"
#include "PlaybackStateUpdater.h"
#include "PlaybackController.h"
#include "SpeedWidget.h"
#include "ProgressWidget.h"

class QLabel;
class QDoubleSpinBox;
class QAction;
class QToolBar;
class QComboBox;
class QSlider;

/*!	\brief	This plays an experiment back
 *
 */
class ReplayViewer : public Viewer
{
	Q_OBJECT
public:
	ReplayViewer(QWidget *parent=0);
	virtual ~ReplayViewer(){};
	QString type() { return "Test"; };

public slots:
	void init();  //Called just before displaying the viewer
	void deinit();	//Called just after the user switches out of the viewer
	bool aboutToQuit();

	void play();
	void pause();
	void stop();


private:
	void setupUi();
	void generateComboBox();

	QSharedPointer<PlaybackController> playbackController_;
	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<Picto::Experiment> experiment_;
	QSharedPointer<PlaybackStateUpdater> playbackUpdater_;
	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> outSigControllers_;

	Picto::VisualTargetHost *visualTargetHost_;
	QVector<QWidget *> outputSignalsWidgets_;

	QAction *loadSessionAction_;
	QAction *playAction_;
	QAction *pauseAction_;
	QAction *stopAction_;
	QLabel *status_;
	SpeedWidget *speed_;
	ProgressWidget *progress_;
	QComboBox *runs_;
	//QSlider *zoomSlider_;	//Zoom slider isn't actually useful for testing and we need to complicate the mouse signal input code to make it work correctly, so its disabled here for now.

	QToolBar* testToolbar_;
	bool pausedFromJump_;	//If playback is not yet paused, system should pause playback while a jump location is being selected.
	bool jumpDownRequested_;
	QString lastStatus_;

	enum Status {Ending, Stopped, Running, Paused};
private slots:

	void playbackStatusChanged(int status);
	void loadSession();
	void updateTime(double time);
	void updateRunsList(QStringList runs);
	void setCurrentRun(int index);
	void loading(bool load);
	void jumpRequested(double time);
	void userChoosingJump(bool starting);
	//void zoomChanged(int zoom);

};

#endif