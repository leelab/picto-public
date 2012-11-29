#ifndef _SESSIONPLAYER_H_
#define _SESSIONPLAYER_H_
#include <QObject>
#include <QHash>

#include "SessionState.h"
#include "SessionLoader.h"

namespace Picto {
/*! \brief Component of Picto Playback system that plays back sessions.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SessionPlayer : public QObject
#else
class SessionPlayer : public  QObject
#endif
{
	Q_OBJECT
public:
	SessionPlayer(QSharedPointer<SessionState> sessState,QSharedPointer<SessionLoader> sessLoader);
	virtual ~SessionPlayer();

	void restart();
	bool stepToTime(double time);
	double getTime();
	//If the object isProcessing, it is in the process of handling a previous command
	//and will not respond to new commands.  This can be used in reentrant cases since
	//the playback system may return processing to the event loop while it performs 
	//long processes.
	bool isProcessing();
signals:
	void reachedEnd();
protected:

private:
	bool stepForward(double lookForward);
	bool stepToNextFrame(double lookForward);
	bool step(double lookForward);
	QSharedPointer<SessionState> sessionState_;
	QSharedPointer<SessionLoader> sessionLoader_;
	PlaybackIndex lastIndex_;
	bool processing_;
	bool reachedEnd_;

	QSharedPointer<DataState> getNextTriggerState(double lookForward);

};

}; //namespace Picto
#endif