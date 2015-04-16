#ifndef _PLAYBACKTHREAD_H_
#define _PLAYBACKTHREAD_H_
#include <QThread>
#include <QTimer>

/*! \brief This is used with the PlaybackController to run Session playback in a non GUI thread.
 *	\details When QThread::start() is called, run() gets called in a new thread.  run() just emits the init()
 *	signal starts an event loop during which the update() signal is emitted repeatedly.  
 *	Other objects use the PlaybackThread so that they can run some of their functions
 *	in its thread.  To do this, they use QObject::moveToThread(), then attach slots to the init() 
 *	and update() signals accordingly.
 *	\note Objects that want functions to run in this thread need to use QObject::moveToThread() because if
 *	they don't, Qt will simply queue up calls to init() and update() and run them in the event queue of
 *	the thread that the object belongs to.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PlaybackThread : public QThread
{
	Q_OBJECT
public:
	PlaybackThread();
signals:
	void init();
	void update();
protected:
	void run();
private:
	QTimer* updateCaller_;

};

#endif