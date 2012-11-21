#include <QCoreApplication>

#include "PlaybackStateUpdater.h"
#include "../../common/memleakdetect.h"

using namespace Picto;

PlaybackStateUpdater::PlaybackStateUpdater()
{
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
	return sessionPlayer_->stepToTime(runToTime);
}

bool PlaybackStateUpdater::setFile(QString filePath)
{
	sessionState_ = QSharedPointer<SessionState>(new SessionState());
	fileSessionLoader_ = QSharedPointer<FileSessionLoader>(new FileSessionLoader(sessionState_));
	sessionPlayer_ = QSharedPointer<SessionPlayer>(new SessionPlayer(sessionState_));

	connect(sessionState_.data(),SIGNAL(propertyChanged(int,QString)),this,SIGNAL(propertyChanged(int,QString)));
	connect(sessionState_.data(),SIGNAL(transitionActivated(int)),this,SIGNAL(transitionActivated(int)));
	connect(sessionState_.data(),SIGNAL(framePresented(double)),this,SIGNAL(framePresented(double)));
	connect(sessionState_.data(),SIGNAL(rewardSupplied(double,int,int)),this,SIGNAL(rewardSupplied(double,int,int)));
	connect(sessionState_.data(),SIGNAL(signalChanged(QString,QStringList,QVector<float>)),this,SIGNAL(signalChanged(QString,QStringList,QVector<float>)));
	connect(fileSessionLoader_.data(),SIGNAL(loading(bool)),this,SIGNAL(loading(bool)));
	
	paused_ = false;
	timerOffset_ = 0.0;
	playbackSpeed_ = 1.0;
	firstResumeFrame_ = true;

	//Load session file to file loader
	if(!fileSessionLoader_->setFile(filePath))
		return false;

	//Load the session file's design root
	designRoot_ = QSharedPointer<DesignRoot>(new DesignRoot());
	if(!designRoot_->resetDesignRoot(QString(fileSessionLoader_->getDesignDefinition())))
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

bool PlaybackStateUpdater::pause()
{
	paused_ = true;
	firstResumeFrame_ = true;
	return true;
}

bool PlaybackStateUpdater::resume()
{
	paused_ = false;
	return true;
}

bool PlaybackStateUpdater::rewindToStart()
{
	paused_ = false;
	firstResumeFrame_ = true;
	return sessionState_->reset();
}

void PlaybackStateUpdater::setPlaybackSpeed(double speed)
{
	if(speed <= 0)
		return;
	firstResumeFrame_ = true;
	playbackSpeed_ = speed;	
}