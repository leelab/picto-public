#include <QTimer>
#include "PlaybackThread.h"

PlaybackThread::PlaybackThread()
{
}

void PlaybackThread::run()
{
	//We establish the timer_ here so that we can start and stop it from this thread
	//if it was created/deleted in the constructor/destructor it wouldn't be directly accessible
	QTimer* updateCaller = new QTimer(this);
	updateCaller->setInterval(0);
	connect(updateCaller, SIGNAL(timeout()), this, SIGNAL(doLoad()), Qt::DirectConnection);
	updateCaller->start();
	QThread::exec();
}