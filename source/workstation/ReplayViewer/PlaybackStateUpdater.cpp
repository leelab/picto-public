#include <QCoreApplication>

#include "PlaybackStateUpdater.h"
#include "../../common/memleakdetect.h"

using namespace Picto;

PlaybackStateUpdater::PlaybackStateUpdater()
{
	paused_ = true;
	waiting_ = false;
	firstResumeFrame_ = true;
}

bool PlaybackStateUpdater::updateState()
{
	if(!designRoot_)
		return false;
	if(paused_)
	{
		return true;
	}
	if(firstResumeFrame_)
	{
		timerOffset_ = sessionPlayer_->getTime();
		timer_.restart();
		firstResumeFrame_ = false;
		return true;
	}
	double runToTime = timerOffset_+(playbackSpeed_*timer_.elapsed()/1000.0);
	if(runToTime <= sessionPlayer_->getTime())
		return true; 
	if(!sessionPlayer_->stepToTime(runToTime))
	{
		if(!fileSessionLoader_->dataIsReady())
		{
			firstResumeFrame_ = true;
			waiting_ = true;
			emit loading(true);
		}
	}
	else
	{
		if(waiting_)
			emit loading(false);
		waiting_ = false;
	}
	return true;
}

bool PlaybackStateUpdater::setFile(QString filePath)
{
	stop();
	if(fileSessionLoader_)
	{	//If there was already a loader running, end its thread execution.
		fileSessionLoader_->quit();
		fileSessionLoader_->wait();
	}
	sessionState_ = QSharedPointer<SessionState>(new SessionState());
	fileSessionLoader_ = QSharedPointer<FileSessionLoader>(new FileSessionLoader(sessionState_));
	sessionPlayer_ = QSharedPointer<SessionPlayer>(new SessionPlayer(sessionState_,fileSessionLoader_));

	connect(sessionState_.data(),SIGNAL(propertyChanged(int,QString)),this,SIGNAL(propertyChanged(int,QString)));
	connect(sessionState_.data(),SIGNAL(transitionActivated(int)),this,SIGNAL(transitionActivated(int)));
	connect(sessionState_.data(),SIGNAL(framePresented(double)),this,SIGNAL(framePresented(double)));
	connect(sessionState_.data(),SIGNAL(rewardSupplied(double,int,int)),this,SIGNAL(rewardSupplied(double,int,int)));
	connect(sessionState_.data(),SIGNAL(signalChanged(QString,QStringList,QVector<float>)),this,SIGNAL(signalChanged(QString,QStringList,QVector<float>)));
	connect(sessionPlayer_.data(),SIGNAL(reachedEnd()),this,SLOT(reachedEnd()));

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
	if(sessionPlayer_)
		sessionPlayer_->restart();
	return true;
}

void PlaybackStateUpdater::setPlaybackSpeed(double speed)
{
	if(speed <= 0)
		return;
	firstResumeFrame_ = true;
	playbackSpeed_ = speed;	
}

void PlaybackStateUpdater::reachedEnd()
{
	stop();
	emit finishedPlayback();
}