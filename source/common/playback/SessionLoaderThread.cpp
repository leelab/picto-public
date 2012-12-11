#include <QTimer>
#include "SessionLoaderThread.h"
using namespace Picto;

SessionLoaderThread::SessionLoaderThread()
{
}

void SessionLoaderThread::run()
{
	//We establish the timer_ here so that we can start and stop it from this thread
	//if it was created/deleted in the constructor/destructor it wouldn't be directly accessible
	QTimer* loadCaller = new QTimer(this);
	loadCaller->setInterval(0);
	connect(loadCaller, SIGNAL(timeout()), this, SIGNAL(doLoad()), Qt::DirectConnection);
	loadCaller->start();

	QThread::exec();
}