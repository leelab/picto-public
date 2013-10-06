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

/*! PlaybackStateUpdater
 *
 *	Extends StateUpdater to update the Picto State based on
 *	the latest data on the Picto Server
 */

class PlaybackStateUpdater : public StateUpdater
{
	Q_OBJECT
public:
	PlaybackStateUpdater();
	virtual ~PlaybackStateUpdater();

	//StateUpdater Interface
	//Updates the picto state to the latest values
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

signals:
	void startingRun(QString taskName,QString runName);
	void endingRun();

	//triggered when values change
	void propertyValueChanged(int propId, QString value);
	void propertyInitValueChanged(int propId, QString value);
	void transitionActivated(int transId);
	void framePresented(double time);
	void rewardSupplied(double time,int duration,int channel);
	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);
	void loadedTo(double maxBehavioral,double maxNeural);
	void loading(bool isLoading);
	void percentLoaded(double percent);
	void newRun(double length);
	void finishedPlayback();
	void disableRendering(bool disable);

private:
	void emitLoadTimeSignals();
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
	double currRunLength_;
	double lastMaxBehav_;
	double lastMaxNeural_;
private slots:
	void reachedEnd();
};


}; //namespace Picto

#endif
