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
}

PlaybackStateUpdater::~PlaybackStateUpdater()
{
}

bool PlaybackStateUpdater::updateState()
{
	if(!designRoot_)
		return false;
	double runToTime = timerOffset_;
	if(!paused_ && !waiting_ && !firstResumeFrame_)
	{
		runToTime += playbackSpeed_*timer_.elapsed()/1000.0;
	}
	if(!waiting_ && !fileSessionLoader_->dataIsReady(runToTime))
	{
		timerOffset_ = runToTime;
		waiting_ = true;
	}
	if(sessionPlayer_->stepToTime(runToTime))
	{
		if(waiting_)
		{
			firstResumeFrame_ = true;
			waiting_ = false;
		}
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
	fileSessionLoader_ = QSharedPointer<FileSessionLoader>(new FileSessionLoader(sessionState_));
	sessionPlayer_ = QSharedPointer<SessionPlayer>(new SessionPlayer(sessionState_,fileSessionLoader_));

	connect(sessionState_.data(),SIGNAL(propertyChanged(int,QString)),this,SIGNAL(propertyChanged(int,QString)));
	connect(sessionState_.data(),SIGNAL(transitionActivated(int)),this,SIGNAL(transitionActivated(int)));
	connect(sessionState_.data(),SIGNAL(framePresented(double)),this,SIGNAL(framePresented(double)));
	connect(sessionState_.data(),SIGNAL(rewardSupplied(double,int,int)),this,SIGNAL(rewardSupplied(double,int,int)));
	connect(sessionState_.data(),SIGNAL(signalChanged(QString,QStringList,QVector<float>)),this,SIGNAL(signalChanged(QString,QStringList,QVector<float>)));
	connect(sessionPlayer_.data(),SIGNAL(reachedEnd()),this,SLOT(reachedEnd()));
	connect(sessionPlayer_.data(),SIGNAL(loading(bool)),this,SIGNAL(loading(bool)));

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
	return fileSessionLoader_->runDuration();
}

bool PlaybackStateUpdater::loadRun(int index)
{
	if(!fileSessionLoader_)
		return false;
	paused_ = false;
	timerOffset_ = 0.0;
	playbackSpeed_ = 1.0;
	firstResumeFrame_ = true;
	runLoaded_ = false;
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
	firstResumeFrame_ = true;
	timerOffset_ = sessionPlayer_->getTime();
	playbackSpeed_ = speed;	
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

void PlaybackStateUpdater::reachedEnd()
{
	stop();
	emit finishedPlayback();
}