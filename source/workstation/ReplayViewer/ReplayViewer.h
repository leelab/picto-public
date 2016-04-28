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
#include "../DataViewer/ViewSelectionFrame.h"

class QLabel;
class QDoubleSpinBox;
class QAction;
class QToolBar;
class QComboBox;
class QSlider;
class QProgressBar;
class QLCDNumber;

/*!	\brief  Defines the UI for the ReplayViewer.
 *
 *	\details The ReplayViewer includes a number of components.  It has a main display area where Session playback is viewed.
 *	There is a timing bar underneath the main area that shows the current playback position within a Session Run and allows
 *	the user to move around within the Session to different times.  A speed slider is provided alongside the main area to
 *	define the playback speed.  Sessions are loaded by pressing a button in the toolbar.  A single or multiple Sessions may
 *	be loaded from the dialog that pops up.  A RunSelectorWidget is used to select which Runs will be played back or
 *	analyzed.  Multiple Runs can be selected in this Viewer from the multiple Sessions loaded to define a batch analysis.
 *	An AnalysisSelectorWidget is used to define which Analyses to run during Run playback, be they local Analyses, built
 *	into the Session's Design, or imported Analyses that are added to the Design at playback time.  Finally, an
 *	OutputWidgetHolder is used to hold the AnalysisOutputWidgets that contain the output from the analyses.  This widget
 *	allows the user to save the output to a file path.  Alternatively, the user can use a "run to end" button when running a
 *	Session run.  When they do this, a message box will give the user the option to save the Analysis Output automatically
 *	at the end of the Run, and they can set up the save file path there.  This is particularly useful when analyzing a batch
 *	of Runs, in which case all Runs' outputs are saved to the same parent directory.
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class ReplayViewer : public Viewer
{
	Q_OBJECT
public:
	ReplayViewer(QWidget *parent=0);
	virtual ~ReplayViewer(){};
	QString type() { return "Replay"; };

public slots:
	//Called just before displaying the viewer
	void init();
	//Called just after the user switches out of the viewer
	void deinit();
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
	QSharedPointer<PlaybackStateUpdater> playbackUpdater_;
	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> outSigControllers_;

	RecordingVisualTargetHost *visualTargetHost_;
	QVector<QWidget *> outputSignalsWidgets_;
	ViewSelectionFrame *viewSelectionFrame_;

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
	//Zoom slider isn't actually useful for testing and we need to complicate the mouse signal input code to make it work
	//correctly, so its disabled here for now.
	//QSlider *zoomSlider_;	
	
	QAction *toggleRecord_;
	QLCDNumber *recordTime_;
	QAction *restartRecord_;
	QAction *saveRecording_;

	QToolBar* testToolbar_;
	QToolBar* recordToolbar_;

	Picto::AnalysisSelectorWidget* analysisSelector_;
	OutputWidgetHolder* outputWidgetHolder_;

	//If playback is not yet paused, system should pause playback while a jump location is being selected.
	bool pausedFromJump_;
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
	void taskChanged(QString newTask);

};

#endif