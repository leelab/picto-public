#ifndef REPLAYVIEWER_H_
#define REPLAYVIEWER_H_

#include <QList>
#include "../viewer.h"
#include "../../common/engine/pictoengine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "RecordingVisualTargetHost.h"
#include "../../common/iodevices/VirtualOutputSignalController.h"
#include "PlaybackStateUpdater.h"
#include "PlaybackController.h"
#include "SpeedWidget.h"
#include "ProgressWidget.h"
#include "OutputWidgetHolder.h"
#include "../../common/Designer/RunSelectorWidget.h"
#include "../../common/Designer/AnalysisSelectorWidget.h"

class QLabel;
class QDoubleSpinBox;
class QAction;
class QToolBar;
class QComboBox;
class QSlider;
class QProgressBar;
class QLCDNumber;

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

	void runNormal();
	void runToEnd();
	bool play();
	void pause();
	void stop();


private:
	void setupUi();
	void updateRecordingTarget();
	QList<QUuid> getSelectedLocalAnalyses();
	QStringList getAnalysesToImport();
	bool popRunQueueFront();
	struct PlayRunInfo
	{
		PlayRunInfo(){filePath_="";runIndex_=-1;};
		PlayRunInfo(QString filePath,int runIndex){filePath_=filePath;runIndex_=runIndex;};
		QString filePath_;
		int runIndex_;
		bool operator==(const PlayRunInfo& rhs){return (filePath_ == rhs.filePath_) && (runIndex_ == rhs.runIndex_);};
		bool operator!=(const PlayRunInfo& rhs){return !((filePath_ == rhs.filePath_) && (runIndex_ == rhs.runIndex_));};
	};
	void setRunQueue(QList<PlayRunInfo> newRunQueue);
	bool activateSelectedAnalyses();
	QList<PlayRunInfo> getSelectedPlayRunInfo();

	QSharedPointer<PlaybackController> playbackController_;
	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<Picto::Experiment> experiment_;
	QSharedPointer<PlaybackStateUpdater> playbackUpdater_;
	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> outSigControllers_;

	RecordingVisualTargetHost *visualTargetHost_;
	QVector<QWidget *> outputSignalsWidgets_;

	QList<PlayRunInfo> runQueue_;
	PlayRunInfo latestRun_;

	QAction *loadSessionAction_;
	QAction *playAction_;
	QAction *runToEndAction_;
	QAction *pauseAction_;
	QAction *stopAction_;
	SpeedWidget *speed_;
	ProgressWidget *progress_;
	QProgressBar *loadProgress_;
	RunSelectorWidget* runs_;
	QComboBox *userType_;
	QComboBox *lfpRequirements_;
	//QSlider *zoomSlider_;	//Zoom slider isn't actually useful for testing and we need to complicate the mouse signal input code to make it work correctly, so its disabled here for now.
	
	QAction *toggleRecord_;
	QLCDNumber *recordTime_;
	QAction *restartRecord_;
	QAction *saveRecording_;

	QToolBar* testToolbar_;
	QToolBar* recordToolbar_;

	Picto::AnalysisSelectorWidget* analysisSelector_;
	OutputWidgetHolder* outputWidgetHolder_;

	bool pausedFromJump_;	//If playback is not yet paused, system should pause playback while a jump location is being selected.
	bool jumpDownRequested_;
	bool recordModeOn_;
	bool calledPlayNotPause_;
	bool needsAutoSave_;
	bool useRunToEnd_;
	bool startingRun_;
	int latestStatus_;
	QString lastStatus_;

	enum Status {Ending, Stopped, Running, Paused};
private slots:

	void playbackStatusChanged(int status);
	void loadSession();
	void updateTime(double time);
	void runSelectionChanged();
	void analysisWidgetChanged();
	void setUserType(int index);
	void setLFPRequirements(int index);
	void percentLoaded(double percent);
	void jumpRequested(double time);
	void userChoosingJump(bool starting);
	void toggleRecording();
	void restartRecording();
	void saveRecording();
	void setRecordTime(double time);
	void designRootChanged();
	void preloadError(QString errorStr);
	void runStarted(QUuid runId);
	void finishedPlayback();
	void loadError(QString errorMsg);
	void sessionPreloaded(PreloadedSessionData sessionData);
	//void zoomChanged(int zoom);

};

#endif