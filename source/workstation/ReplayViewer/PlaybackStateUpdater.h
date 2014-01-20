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
 *	\details The Session file path is set in setFile().  A FileSessionLoader is used 
 *	with a SessionState to actually load the file and a SessionPlayer is used to
 *	playback the SessionState.  
 *
 *	This class implements things like pausing, stopping, playing, etc and defines
 *	the correct times to enter into the SessionPlayer accordingly.  It 
 *	implements StateUpdater so that it can be used with a SlaveExperimentDriver
 *	to drive a slave's experimental state.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
	/*! \brief Emitted when a new Run starts.  taskName is the name of the Task that will be run, runName is the name of this particular run as it
	 *	appears in the Run Notes tab of the Workstation.
	 */
	void startingRun(QString taskName,QString runName);
	/*! \brief Emitted when a run ends.*/
	void endingRun();

	/*!\brief Emitted when a Property value changes for a Property with an AssetId of propId and a new value of value.*/
	void propertyValueChanged(int propId, QString value);
	/*! \brief Emitted when a Property initValue changes for a Property with an AssetId of propId and a new initValue of value.*/
	void propertyInitValueChanged(int propId, QString value);
	/*! \brief Emitted when a Transition with AssetId of transId is traversed.*/
	void transitionActivated(int transId);
	/*! \brief Emitted when a new frame is presented.  time is the time at which the first phosphor appeared on the display for this frame.*/
	void framePresented(double time);
	/*! \brief Emitted when a reward is triggered.  
	 *	\details time is the time when the reward starts, duration is its duration in ms, channel is the 
	 *	reward channel on which the reward was provided.
	 */
	void rewardSupplied(double time,int duration,int channel);
	/*! \brief Emitted when new signal data comes in.
	 *	\details name is the name of the SignalChannel for which new data is available.  subChanNames is a list of names of this SIgnalChannel's
	 *	sub-channels.  vals is a list of the new values.  If subChanNames is x,y, vals will have data ordered like x1,y1,x2,y2,x3,y3,...
	 *	\note Unlike in the master, signal data only comes in once per frame, not once per SignalChannel sample period.  signalChanged() should
	 *	be emitted with all of the SignalChannel data since the previous frame after all Property value change and transition traversal signals
	 *	have been emitted and before framePresented() is emitted.
	 */
	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);
	/*! \brief Emitted when Session file loading starts/stops.  The input isLoading value indicates if this is a start or a stop.*/
	void loading(bool isLoading);
	/*! \brief Emitted periodically during Session loading.  percent is the percentage of loading that has occured.
	 */
	void percentLoaded(double percent);
	/*! \brief DOES NOT APPEAR TO BE USED.  But it looks like its supposed to signify that a new run has
	 *	been selected with the input length in seconds.
	 */
	void newRun(double length);
	/*! \brief Emitted when playback ends for the current run.
	*/
	void finishedPlayback();
	/*! \brief Emitted when rendering should be disabled/enabled.  
	 *	\details This is useful if we are skipping around in the run.  In this case all signals will still be correctly emitted in the correct
	 *	order and all data will be available on the data readers; however, the visual elements need not be displayed.
	 */
	void disableRendering(bool disable);

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
