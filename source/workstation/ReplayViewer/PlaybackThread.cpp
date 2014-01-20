#include <QTimer>
#include "PlaybackThread.h"

PlaybackThread::PlaybackThread()
{
}

/*! \brief Impliments QThread::run() to emit an init() signal, then emit an update() signal repeatedly.
 *	\details This is meant to be used with other objects so that they can run some of their functions
 *	in this thread.  To do this, they would use QObject::moveToThread(), then attach slots to 
 *	the init() and update() signals accordingly.
 *	\note Objects that want functions to run in this thread need to use QObject::moveToThread() because if
 *	they didn't Qt would simply queue up calls to init() and update() and run them in the event queue of
 *	the thread that the object belongs to.
 */
void PlaybackThread::run()
{
	emit init();
	//We establish the timer_ here so that we can start and stop it from this thread
	//if it was created/deleted in the constructor/destructor it wouldn't be directly accessible
	QTimer* updateCaller = new QTimer(this);
	updateCaller->setInterval(0);
	connect(updateCaller, SIGNAL(timeout()), this, SIGNAL(update()), Qt::DirectConnection);
	updateCaller->start();
	QThread::exec();
}