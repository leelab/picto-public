#ifndef _SESSIONLOADER_H_
#define _SESSIONLOADER_H_
#include <QObject>
#include <QSharedPointer>
#include "SessionState.h"

namespace Picto {
/*! \brief Component of Picto Playback system that loads data into SessionState.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SessionLoader : public QObject
#else
class SessionLoader : public  QObject
#endif
{
	Q_OBJECT
public:
	SessionLoader(QSharedPointer<SessionState> sessState);
	virtual ~SessionLoader();

	bool setFile(QString path);
	QStringList getRunNames();
	//Loads data from the data source's Run to the SessionState up to the maximum
	//available (default) or up to the input time (for speed optimization).
	//runIndex is the index in the list returned by getRunNames() of the run to be
	//loaded.
	//maxTime is defined with respect to the beginning of the run.
	bool loadRun(int runIndex,double maxTime = -1);

private:

	QSharedPointer<SessionState> sessionState_;
	double bufferTime_;
	qulonglong tempVal_;
	qulonglong tempId_;


	QVector<QPair<double,qulonglong>> props_;
	QVector<QPair<double,qulonglong>> trans_;
	QVector<QPair<double,qulonglong>> frames_;
	int propPerFrame_;
	int transPerFrame_;



private slots:
	void loadPropertyData(PlaybackIndex currLast,PlaybackIndex to);
	void loadTransitionData(PlaybackIndex currLast,PlaybackIndex to);
	void loadFrameData(PlaybackIndex currLast,PlaybackIndex to);
	void loadRewardData(PlaybackIndex currLast,PlaybackIndex to);
	void loadSignalData(PlaybackIndex currLast,PlaybackIndex to);
	void loadLFPData(PlaybackIndex currLast,PlaybackIndex to);
	void loadSpikeData(PlaybackIndex currLast,PlaybackIndex to);

	void loadNextPropertyData(PlaybackIndex currLast,bool backward);
	void loadNextTransitionData(PlaybackIndex currLast,bool backward);
	void loadNextFrameData(PlaybackIndex currLast,bool backward);
	void loadNextRewardData(PlaybackIndex currLast,bool backward);
	void loadNextSignalData(PlaybackIndex currLast,bool backward);
	void loadNextLFPData(PlaybackIndex currLast,bool backward);
	void loadNextSpikeData(PlaybackIndex currLast,bool backward);
};

}; //namespace Picto
#endif