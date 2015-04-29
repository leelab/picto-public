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
#include "../../common/engine/SlaveExperimentDriver.h"
using namespace Picto;

/*! \brief Defines a command to be used to change the PlaybackController State.
 *	\details A command structure like this is used to enforce a clear separation
 *	between command logic and state machine logic.  THis is important since the
 *	two run in different threads and communicate via the threadsafe PlaybackControllerData
 *	functions.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct PlaybackCommand 
{
	/*! \brief The Type of a command.*/
	enum Type {
		NoCommand,	//!< This is a null command.  It doesn't mean anything.
		PreLoad,	//!< This tells the PlaybackController to preload a session file.
		ChangeFile, //!< This tells the PlaybackController to change the loaded file/s
		ChangeRun,	//!< This tells the PlaybackController to change the currently loaded Run.
		ChangeSpeed, //!< This tells the PlaybackController to change the playback speed.
		ChangeUserType, //!< This tells the PlaybackController to change the user type, Operator or TestSubject.
		EnableLFP,	//!< This tells the PlaybackController to enable/disable LFP loading for load time optimization purposes.
		Play, //!< This tells the PlaybackController to start playing back the current Run.
		Pause, //!<	This tells the PlaybackController to pause the currently playing Run.
		Stop,	//!< This tells the PlaybackController to stop the current Run.
		Jump	//!< This tells the PlaybackController to jump to a different time.
	};
	/*! \brief The type of user looking at the played back Session, Operator or TestSubject.  
	 *	\details This defines which VisualElement objects are visible as a function of their
	 *	OperatorView and SubjectView Properties.
	 */
	enum UserType {
		Operator,
		TestSubject
	};
	PlaybackCommand(){commandType = NoCommand;};
	/*! \brief Constructs a new PlaybackCommand with the input cmd Type and a QVariant
	 *	value if applicable.
	 */
	PlaybackCommand(Type cmd,QVariant val=0){commandType = cmd;commandData=val;};
	Type commandType;	//!< The Type of this command
	QVariant commandData;	//!< Variant data that needs to be passed along with this command.
};

/*! \brief This class holds data for the PlaybackController.  Since this class ends up
 *	being called by both the GUI and playback threads, functions use mutexes and are 
 *	thread safe.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PlaybackControllerData
{
public:
	PlaybackControllerData();
	/*! \brief This might better be called State.  It is really the current run state in the PlaybackController state machine.
	 */
	enum Status 
	{
		None,			//!< The PlaybackController starts in this state and immedidately transitions to Idle.		
		Idle,			//!< The state where the PlaybackController is ready for a session file to be loaded
		PreLoading,		//!< In this state, a Session file is preloaded and its high level data is emitted in the sessionPreloaded() signal.
		Stopped,		//!< THis is much like the Idle state, except that when we're in the Stopped state, there may be a loaded or preloaded Session file.
		Loading,		//!< In this state, the entire Session file is loaded.
		Running,		//!< In this state, the session file is being played back
		Paused};		//!< In this state, the session file is in the progress of being played back but paused.
	void setAsSetup();
	bool isSetup();
	void setCurrTime(double val);
	double getCurrTime();
	void setRunSpeed(double val);
	double getRunSpeed();
	void setRunLength(double val);
	double getRunLength();
	void setLoadedFilePath(QString filePath);
	QString getLoadedFilePath();
	void setNextFilePath(QString filePath);
	QString getNextFilePath();
	void enableLfp(bool enabled);
	bool getLfpEnabled();
	void setEnabledBuiltInAnalyses(QList<QUuid> analysisList);
	void setEnabledImportedAnalyses(QList<QUuid> analysisList);
	void clearEnabledBuiltInAnalyses();
	void clearEnabledImportedAnalyses();
	QList<QUuid> getEnabledAnalyses();
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
	QString filePath_;
	QString nextFilePath_;
	bool lfpEnabled_;
	QList<QUuid> enabledAnalyses_;
	QList<QUuid> enabledImportedAnalyses_;
	Status status_;
	Status nextStatus_;
	QVector<PlaybackCommand> cmds_;

};

/*! \brief A simple struct for storing preloaded data about a Session.
 *	\details The PlaybackController preloads Session files to retrieve high level data about them without
 *	loading the entire file.  This struct stores that data.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct PreloadedSessionData
{
	PreloadedSessionData(){fileName_="";};
	/*! \brief A copy constructor.
	 */
	PreloadedSessionData(const PreloadedSessionData& other){
		fileName_ = other.fileName_;
		runs_ = other.runs_;
		notes_ = other.notes_;
		savedRuns_ = other.savedRuns_;
		analysisIds_ = other.analysisIds_;
		analysisNames_ = other.analysisNames_;
	};
	~PreloadedSessionData(){};
	QString fileName_;			//!< The file path of the Session file.
	QStringList runs_;			//!< The names of the runs in the Session.
	QStringList notes_;			//!< The notes, ordered by run for every run in the Session.
	QStringList savedRuns_;		//!< The names of the saved runs in the preloaded design.
	QList<QUuid> analysisIds_;	//!< The Associate Ids of the local Analyses that are available in the preloaded design.
	QStringList analysisNames_;	//!< The names of the local Analyses that are available in the preloaded design.
};
Q_DECLARE_METATYPE(PreloadedSessionData)

/*!	\brief The top level object that controls Session playback using the PlaybackStateUpdater
 *	and SlaveExperimentDriver.
 *	\details The PlaybackController manages playback of Session files.  It runs in its own thread
 *	so that long operations like loading don't interfere with the UI.  Interfacing with
 *	the PlaybackController is through functions like play(), pause(), stop(), jumpToTime(),
 *	etc.  These functions communicate with the parts of the PlaybackCOntroller that run
 *	in the PlaybackController thread through a thread-safe interface handled by PlaybackControllerData.
 *	
 *	A number of signals are available for a view into what is happening inside the PlaybackController
 *	as well as a way to gather data about things like PreloadedSessionData.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PlaybackController : public QObject
{
	Q_OBJECT
public:
	PlaybackController();
	virtual ~PlaybackController();

	QString preLoadSessions(QStringList filenames);

	QSharedPointer<Picto::VisualTarget> getVisualTarget();
	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> getOutputSignalControllers();
	QSharedPointer<Picto::RenderingTarget> getRenderingTarget();
	QSharedPointer<DesignRoot> getDesignRoot();
	double getRunLength();
	QString getLoadedFilePath();
	void aboutToQuit();
	void play(QList<QUuid> activeAnalyses,QStringList importAnalyses);
	void pause(QList<QUuid> activeAnalyses = QList<QUuid>(),QStringList importAnalyses = QStringList());

public slots:
	void stop();
	void jumpToTime(double time);
	void setRunSpeed(double value);
	void setUserToOperator();
	void setUserToSubject();
	void enableLFPLoad(bool enable);
	void selectFile(QString filePath);
	void selectRun(int index);

signals:
	/*! \brief Emitted when a Session has been preloaded.
	 *	@param sessionData A PreloadedSessionData object that includes information about the Preloaded Session.
	*/
	void sessionPreloaded(PreloadedSessionData sessionData);
	/*! \brief Emitted when preloading of a Session fails.*/
	void sessionPreloadFailed(QString error);
	/*! \brief Emitted when the current Session time changes. */
	void timeChanged(double time);
	/*! \brief Emitted when loading of a session (not preloading) starts and stops.
	 *	@param isLoading Indicates if the load is starting or stopping.
	 */
	void loading(bool isLoading);
	/*! \brief Emitted periodically during load to indicate what percent of the load is done.  This can be used with a progress bar.*/
	void percentLoaded(double percent);
	/*! \brief Emitted when the Design Root loaded in the PlaybackController changes.*/
	void designRootChanged();
	/*! \brief Emitted when the PlaybackController Status (this is really its state, it is a PlaybackCOntrollerData::Status enum value) changes.*/
	void statusChanged(int status);
	/*! \brief Emitted when playback ends.*/
	void finishedPlayback();
	/*! \brief Emitted when an error occurs during the load process. 
	 *	@param msg An error message describing the error.
	 */
	void loadError(QString msg);
	//! Emitted when loading a new task.  Used to update plots.
	void taskChanged(QString taskName);

private:
	QString activateAnalyses(QStringList analysisData);
	QTimer stateUpdateTimer_;
	QSharedPointer<Picto::RenderingTarget> renderingTarget_;
	QSharedPointer<Picto::PixmapVisualTarget> pixmapVisualTarget_;
	QSharedPointer<Picto::Engine::PictoEngine> engine_;
	QSharedPointer<Picto::Experiment> experiment_;
	QSharedPointer<DesignRoot> designRoot_;
	QSharedPointer<PlaybackThread> playbackThread_;
	QSharedPointer<PlaybackStateUpdater> playbackUpdater_;
	QSharedPointer<SlaveExperimentDriver> slaveExpDriver_;
	QVector<QSharedPointer<Picto::VirtualOutputSignalController>> outSigControllers_;
	PlaybackControllerData data_;
	QString filePath_;
	QString nextFilePath_;
	int currRun_;
	int numImportedAnalyses_;
	QHash<QString, QUuid> cachedAnalysis_;
	QHash<QString, QString> cachedAnalysisNames_;
private slots:
	void newRunLength(double length);
	void setCurrTime(double time);
	void playbackEnded();
	void setup();
	void update();
	//void runExperiment();
	

};


#endif