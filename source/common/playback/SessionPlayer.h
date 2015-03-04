#ifndef _SESSIONPLAYER_H_
#define _SESSIONPLAYER_H_
#include <QObject>
#include <QHash>

#include "SessionState.h"
#include "FileSessionLoader.h"

namespace Picto {
/*! \brief Controls the timing and ordering of played back events during Picto Session playback.
 *	\details This class works closely with a SessionState to playback Session events in a well defined and tightly controlled
 *	order. Its public interface is fairly simple, with restart() being used to restart the current run and stepToTime() being
 *	used to move through it.  A great deal of thought has been put into the exact ordering of played back events.  In particular
 *	the interleaving system for events that occured as part of the StateMachine (Property value changes, Transition traversals)
 *	and events that occured outside of StateMachine control (Signal data, Spikes, LFP, Reward) needed to be clearly defined.
 *	For more detail on how this is handled, see step().
 *
 *	It should also be pointed out that the SessionPlayer does not support "reverse playback" behavior.  When stepToTime()
 *	recieves an input with a time that is lower than the latest time, the Session Run is restarted and fast forwarded to 
 *	that position.  This is due to the need to support the Analysis system.  For more detail, see stepToTime().
 *
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SessionPlayer : public QObject
#else
class SessionPlayer : public  QObject
#endif
{
	Q_OBJECT
public:
	SessionPlayer(QSharedPointer<SessionState> sessState,QSharedPointer<FileSessionLoader> sessLoader);
	virtual ~SessionPlayer();

	void restart();
	bool stepToTime(double time);
	double getTime();
signals:
	/*! \brief Emitted when playback of a Session Run starts or is restarted.  taskName is the name of the task
	 *	that was Run, runName is the name that was saved for this Run during the Experimental Session.
	 */
	void startingRun(QString taskName,QString runName);
	/*! \brief Emitted when loading of Session data starts or finishes.  startingLoad indicates if the event is 
	 *	a load start (true) or a load finish (false).
	 */
	void loading(bool startingLoad);
	/*! \brief Emitted when playback of the current Run ends.
	*/
	void reachedEnd();
protected:

private:
	bool stepForward(double lookForward);
	bool stepToNextFrame(double lookForward);
	bool step(double lookForward);
	void markLoading(bool load);
	QSharedPointer<SessionState> sessionState_;
	QSharedPointer<FileSessionLoader> sessionLoader_;
	PlaybackIndex lastIndex_;
	PlaybackIndex nextFrame_;
	bool processing_;
	bool reachedEnd_;
	bool loading_;

	QSharedPointer<DataState> getNextTriggerState(double lookForward);

};

}; //namespace Picto
#endif