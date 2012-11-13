#ifndef _SESSIONPLAYER_H_
#define _SESSIONPLAYER_H_
#include <QObject>
#include <QHash>

#include "SessionState.h"

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
	SessionPlayer(QSharedPointer<SessionState> sessState);
	virtual ~SessionPlayer();

	bool stepForward();
	bool stepToNextFrame();
	bool stepBack();
	bool stepToPrevFrame();
	bool stepToTime(double time);

protected:

private:
	bool step(bool backward = false);
	QSharedPointer<SessionState> sessionState_;
	bool started_;
	PlaybackIndex lastIndex_;

	QSharedPointer<DataState> getNextTriggerState(bool backward);

private slots:
	void sessionStateReset();

};

}; //namespace Picto
#endif