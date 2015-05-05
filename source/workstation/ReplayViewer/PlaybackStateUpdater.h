#ifndef _PLAYBACKSTATEUPDATER_H_
#define _PLAYBACKSTATEUPDATER_H_
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTime>

#include "../../common/playback/FileSessionLoader.h"
#include "../../common/playback/SessionState.h"
#include "../../common/playback/SessionPlayer.h"
#include "../../common/engine/stateupdater.h"
#include "../common/design/DesignRoot.h"

namespace Picto {

/*! \brief Extends StateUpdater to update the Picto State based on a Session file.
 *	\details The Session file path is set in setFile().  A FileSessionLoader is used with a SessionState to actually load
 *	the file and a SessionPlayer is used to playback the SessionState.  
 *
 *	This class implements things like pausing, stopping, playing, etc and defines the correct times to enter into the
 *	SessionPlayer accordingly.  It implements StateUpdater so that it can be used with a SlaveExperimentDriver to drive a
 *	slave's experimental state.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PlaybackStateUpdater : public StateUpdater
{
	Q_OBJECT
public:
	PlaybackStateUpdater();
	virtual ~PlaybackStateUpdater();

	virtual bool updateState();
	virtual QSharedPointer<FrameReader> getFrameReader();
	virtual QSharedPointer<LfpReader> getLfpReader();
	virtual QSharedPointer<RewardReader> getRewardReader();
	virtual QSharedPointer<RunNotesReader> getRunNotesReader();
	virtual QSharedPointer<SpikeReader> getSpikeReader();
	virtual QStringList getSignalReaderNames();
	virtual QSharedPointer<SignalReader> getSignalReader(QString name);

	bool setFile(QString filePath);
	QSharedPointer<DesignRoot> getDesignRoot();

	QStringList getRuns();
	QStringList getSavedRuns();
	double getRunLength();
	bool loadRun(int index);
	bool pause();
	bool play();
	bool stop();
	void setPlaybackSpeed(double speed);
	double getPlaybackSpeed();
	void jumpToTime(double time);
	void enableLfp(bool enable);

signals:
	/*! \brief Emitted when Session file loading starts/stops.  The input isLoading value indicates if this is a start
	 *	or a stop.
	 */
	void loading(bool isLoading);
	/*! \brief Emitted periodically during Session loading.  percent is the percentage of loading that has occured.
	 */
	void percentLoaded(double percent);
	/*! \brief DOES NOT APPEAR TO BE USED.  But it looks like its supposed to signify that a new run has been selected
	 *	with the input length in seconds.
	 */
	void newRun(double length);
	/*! \brief Emitted when playback ends for the current run.
	 */
	void finishedPlayback();

private:
	void enableRendering(bool en);
	void suspendPlayback();
	void resumePlayback();
	bool playbackSuspended();
	double getPlaybackOffsetTime();
	QSharedPointer<FileSessionLoader> fileSessionLoader_;
	QSharedPointer<SessionState> sessionState_;
	QSharedPointer<SessionPlayer> sessionPlayer_;
	QSharedPointer<DesignRoot> designRoot_;
	bool paused_;
	bool waiting_;
	QTime timer_;
	double timerOffset_;
	double playbackSpeed_;
	bool firstResumeFrame_;
	bool renderingEnabled_;
	bool runLoaded_;
	bool enableLfp_;
	double currRunLength_;
	int currRunIndex_;
	double lastMaxBehav_;
	double lastMaxNeural_;
private slots:
	void reachedEnd();
};


}; //namespace Picto

#endif
