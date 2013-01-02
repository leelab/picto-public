#include <QCoreApplication>

#include "PlaybackStateUpdater.h"
#include "../../common/memleakdetect.h"

using namespace Picto;

PlaybackStateUpdater::PlaybackStateUpdater()
{
	paused_ = true;
	waiting_ = false;
	firstResumeFrame_ = true;
	timerOffset_ = 0;
	currRunLength_ = -1;
	lastMaxBehav_ = 0;
	lastMaxNeural_ = 0;
	playbackSpeed_ = 1.0;
}

PlaybackStateUpdater::~PlaybackStateUpdater()
{
	//if(fileSessionLoader_)
	//	fileSessionLoader_->unload();
}

bool PlaybackStateUpdater::updateState()
{
	if(!designRoot_)
		return false;
	double runToTime = timerOffset_;
	if(!paused_ /*&& !waiting_*/ && !firstResumeFrame_)
	{
		runToTime += playbackSpeed_*timer_.elapsed()/1000.0;
	}
	emitLoadTimeSignals();
	//if(!waiting_ /*&& !fileSessionLoader_->dataIsReady(runToTime)*/)
	//{
	//	timerOffset_ = runToTime;
	//	waiting_ = true;
	//}
	if(sessionPlayer_->stepToTime(runToTime))
	{
		/*if(waiting_)
		{
			firstResumeFrame_ = true;
			waiting_ = false;
		}*/
		if(!paused_ && firstResumeFrame_)
		{
			timer_.restart();
			firstResumeFrame_ = false;
		}
	}
	return true;
}

bool PlaybackStateUpdater::setFile(QString filePath)
{
	stop();
	sessionState_ = QSharedPointer<SessionState>(new SessionState());
	//if(fileSessionLoader_)
	//	fileSessionLoader_->unload();
	fileSessionLoader_ = QSharedPointer<FileSessionLoader>(new FileSessionLoader(sessionState_));
	sessionPlayer_ = QSharedPointer<SessionPlayer>(new SessionPlayer(sessionState_,fileSessionLoader_));

	connect(sessionState_.data(),SIGNAL(propertyChanged(int,QString)),this,SIGNAL(propertyChanged(int,QString)));
	connect(sessionState_.data(),SIGNAL(transitionActivated(int)),this,SIGNAL(transitionActivated(int)));
	connect(sessionState_.data(),SIGNAL(framePresented(double)),this,SIGNAL(framePresented(double)));
	connect(sessionState_.data(),SIGNAL(rewardSupplied(double,int,int)),this,SIGNAL(rewardSupplied(double,int,int)));
	connect(sessionState_.data(),SIGNAL(signalChanged(QString,QStringList,QVector<float>)),this,SIGNAL(signalChanged(QString,QStringList,QVector<float>)));
	connect(sessionPlayer_.data(),SIGNAL(reachedEnd()),this,SLOT(reachedEnd()));
	connect(sessionPlayer_.data(),SIGNAL(loading(bool)),this,SIGNAL(loading(bool)));
	connect(fileSessionLoader_.data(),SIGNAL(percentLoaded(double)),this,SIGNAL(percentLoaded(double)));


	//Load session file to file loader
	if(!fileSessionLoader_->setFile(filePath))
		return false;

	//Load the session file's design root
	designRoot_ = fileSessionLoader_->getDesignRoot();
	if(!designRoot_)
	{
		Q_ASSERT(false);	//This would mean that somehow the session had a bad design in it.
		return false;
	}
	loadRun(0);
	return true;
}

QSharedPointer<DesignRoot> PlaybackStateUpdater::getDesignRoot()
{
	return designRoot_;
}

QStringList PlaybackStateUpdater::getRuns()
{
	if(!fileSessionLoader_)
		return QStringList();
	return fileSessionLoader_->getRunNames();
}

double PlaybackStateUpdater::getRunLength()
{
	if(!fileSessionLoader_)
		return -1;
	return currRunLength_;
}

bool PlaybackStateUpdater::loadRun(int index)
{
	if(!fileSessionLoader_)
		return false;
	paused_ = false;
	timerOffset_ = 0.0;
	firstResumeFrame_ = true;
	runLoaded_ = false;
	currRunLength_ = fileSessionLoader_->runDuration(index);
	return fileSessionLoader_->loadRun(index);
}

bool PlaybackStateUpdater::pause()
{
	paused_ = true;
	firstResumeFrame_ = true;
	timerOffset_ = sessionPlayer_->getTime();
	return true;
}

bool PlaybackStateUpdater::play()
{
	paused_ = false;
	return true;
}

bool PlaybackStateUpdater::stop()
{
	paused_ = true;
	firstResumeFrame_ = true;
	timerOffset_ = 0.0;
	if(sessionPlayer_)
		sessionPlayer_->restart();
	return true;
}

void PlaybackStateUpdater::setPlaybackSpeed(double speed)
{
	if(speed <= 0)
		return;
	if(speed == playbackSpeed_)
		return;
	firstResumeFrame_ = true;
	if(sessionPlayer_)
		timerOffset_ = sessionPlayer_->getTime();
	else
		timerOffset_ = 0;
	playbackSpeed_ = speed;	
}

double PlaybackStateUpdater::getPlaybackSpeed()
{
	return playbackSpeed_;
}

void PlaybackStateUpdater::jumpToTime(double time)
{
	if(!sessionPlayer_)
		return;
	if(time > getRunLength() || time < 0)
		return;
	timerOffset_ = time;
	firstResumeFrame_ = true;
}

void PlaybackStateUpdater::emitLoadTimeSignals()
{
	//double newMaxB = fileSessionLoader_->getMaxBehavTime();
	//double newMaxN = fileSessionLoader_->getMaxNeuralTime();
	//bool emitUpdate = false;
	//if(lastMaxBehav_ != newMaxB)
	//{
	//	lastMaxBehav_ = newMaxB;
	//	emitUpdate = true;
	//}
	//if(lastMaxNeural_ != newMaxN)
	//{
	//	lastMaxNeural_ = newMaxN;
	//	emitUpdate = true;
	//}
	//if(emitUpdate)
	//	emit loadedTo(lastMaxBehav_, lastMaxNeural_);
}

void PlaybackStateUpdater::reachedEnd()
{
	stop();
	emit finishedPlayback();
}