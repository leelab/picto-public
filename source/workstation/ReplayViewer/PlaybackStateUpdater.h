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
	//Updates the picto state to the latest values
	virtual bool updateState();

	bool setFile(QString filePath);
	QSharedPointer<DesignRoot> getDesignRoot();

	QStringList getRuns();
	bool loadRun(int index);
	bool pause();
	bool play();
	bool stop();
	void setPlaybackSpeed(double speed);
signals:
	//triggered when values change
	void propertyChanged(int propId, QString value);
	void transitionActivated(int transId);
	void framePresented(double time);
	void rewardSupplied(double time,int duration,int channel);
	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);
	void loading(bool isLoading);
	void finishedPlayback();

private:
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
	bool runLoaded_;
private slots:
	void reachedEnd();
};


}; //namespace Picto

#endif
