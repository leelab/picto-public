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

struct PlaybackCommand 
{
	enum Type {NoCommand, PreLoad, ChangeFile, ChangeRun, ChangeSpeed, ChangeUserType, EnableLFP, Play, Pause, Stop, Jump};
	enum UserType {Operator,TestSubject};
	PlaybackCommand(){commandType = NoCommand;};
	PlaybackCommand(Type cmd,QVariant val=0){commandType = cmd;commandData=val;};
	Type commandType;
	QVariant commandData;
};

class PlaybackControllerData
{
public:
	PlaybackControllerData();
	enum Status {None,Idle,PreLoading,Stopped,Loading,Running, Paused};
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

struct PreloadedSessionData
{
	PreloadedSessionData(){fileName_="";};
	PreloadedSessionData(const PreloadedSessionData& other){
		fileName_ = other.fileName_;
		runs_ = other.runs_;
		notes_ = other.notes_;
		savedRuns_ = other.savedRuns_;
		analysisIds_ = other.analysisIds_;
		analysisNames_ = other.analysisNames_;
	};
	~PreloadedSessionData(){};
	QString fileName_;
	QStringList runs_;
	QStringList notes_;
	QStringList savedRuns_;
	QList<QUuid> analysisIds_;
	QStringList analysisNames_;
};
Q_DECLARE_METATYPE(PreloadedSessionData)

/*!	\brief	This controlls experimental playback
 *
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
	void sessionPreloaded(PreloadedSessionData sessionData);
	void sessionPreloadFailed(QString error);
	void timeChanged(double time);
	void loading(bool isLoading);
	void percentLoaded(double percent);
	void designRootChanged();
	void statusChanged(int status);
	void finishedPlayback();
	void loadError(QString msg);



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
private slots:
	void newRunLength(double length);
	void setCurrTime(double time);
	void playbackEnded();
	void setup();
	void update();
	//void runExperiment();
	

};


#endif