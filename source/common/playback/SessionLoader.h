#ifndef _SESSIONLOADER_H_
#define _SESSIONLOADER_H_
#include <QObject>
#include <QHash>
#include <QtConcurrentRun>
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

	virtual QStringList getRunNames() = 0;
	virtual bool loadRun(int runIndex) = 0;

signals:
	void loading(bool isLoading);


protected:
	virtual void childLoadData(PlaybackDataType type,PlaybackIndex currLast,PlaybackIndex to) = 0;
	virtual void childLoadNextData(PlaybackDataType type,PlaybackIndex currLast,bool backward) = 0;
	QSharedPointer<SessionState> sessionState_;

private slots:
	void loadData(PlaybackDataType type,PlaybackIndex currLast,PlaybackIndex to);
	void loadNextData(PlaybackDataType type,PlaybackIndex currLast,bool backward);

private:
	QHash<PlaybackDataType,QFuture<void>> loadProcesses_;
};

}; //namespace Picto
#endif