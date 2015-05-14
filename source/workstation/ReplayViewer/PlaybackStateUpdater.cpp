#include <QCoreApplication>

#include "PlaybackStateUpdater.h"
#include "../../common/memleakdetect.h"

using namespace Picto;

PlaybackStateUpdater::PlaybackStateUpdater()
{
	paused_ = true;
	waiting_ = false;
	firstResumeFrame_ = true;
	renderingEnabled_ = true;
	timerOffset_ = 0;
	currRunLength_ = -1;
	lastMaxBehav_ = 0;
	lastMaxNeural_ = 0;
	playbackSpeed_ = 1.0;
	enableLfp_ = true;
}

PlaybackStateUpdater::~PlaybackStateUpdater()
{
	//if(fileSessionLoader_)
	//	fileSessionLoader_->unload();
}

/*! \brief Implements StateUpdater::updateState() to update playback of the underlying Session.
 *	\details This should be called frequently, (like every 10ms or so, under a frame period), it updates the SessionPlayer
 *	according to the current PlaybackStateUpdater settings.  In regular run mode, it will keep on stepping the
 *	SessionPlayer forward.  When paused, the SessionPlayer won't be stepped.  When jumping to a later time, the function
 *	will step much more quickly through the the Session while disabling rendering.
 */
bool PlaybackStateUpdater::updateState()
{
	if(!designRoot_)
		return false;
	double runToTime = timerOffset_ + getPlaybackOffsetTime();
	if(	playbackSuspended()  && (runToTime != sessionPlayer_->getTime()) )
	{
		//We are stepping the session player even though we're not playing back.
		//Rendering should be disabled.
		enableRendering(false);
	}
	if(sessionPlayer_->stepToTime(runToTime))
	{
		//Once we have caught up to runToTime rendering should always be enabled.
		enableRendering(true);

		if(!paused_ && playbackSuspended())
		{
			resumePlayback();
		}
	}
	return true;
}

QSharedPointer<FrameReader> PlaybackStateUpdater::getFrameReader()
{
	Q_ASSERT(sessionState_);
	return sessionState_->getFrameReader();
}

QSharedPointer<LfpReader> PlaybackStateUpdater::getLfpReader()
{
	Q_ASSERT(sessionState_);
	return sessionState_->getLfpReader();
}

QSharedPointer<RewardReader> PlaybackStateUpdater::getRewardReader()
{
	Q_ASSERT(sessionState_);
	return sessionState_->getRewardReader();
}

QSharedPointer<RunNotesReader> PlaybackStateUpdater::getRunNotesReader()
{
	Q_ASSERT(sessionState_);
	return sessionState_->getRunNotesReader();
}

QSharedPointer<SpikeReader> PlaybackStateUpdater::getSpikeReader()
{
	Q_ASSERT(sessionState_);
	return sessionState_->getSpikeReader();
}

QStringList PlaybackStateUpdater::getSignalReaderNames()
{
	Q_ASSERT(sessionState_);
	return sessionState_->getSignalReaderNames();
}

QSharedPointer<SignalReader>  PlaybackStateUpdater::getSignalReader(QString name)
{
	Q_ASSERT(sessionState_);
	return sessionState_->getSignalReader(name);
}

/*! \brief Sets the path to the Session file that will be used for playback.
 *	\details This function sets up this object to playback the input file.
 *	\note This causes the FileSessionLoader to preload the file (ie. Load information about the Task Runs, Design, etc),
 *	but not load the file (load frame times, Property value changes, etc).  Loading of the file data is done int loadRun().
 */
bool PlaybackStateUpdater::setFile(QString filePath)
{
	stop();
	currRunIndex_ = -1;

	/*
	//Clean up old connections to eliminate warnings
	if (sessionState_)
	{
		disconnect(sessionState_.data(), SIGNAL(propertyChanged(int, QString)));
		disconnect(sessionState_.data(), SIGNAL(propertyInitValueChanged(int, QString)));
		disconnect(sessionState_.data(), SIGNAL(transitionActivated(int)));
		disconnect(sessionState_.data(), SIGNAL(framePresented(double)));
		disconnect(sessionState_.data(), SIGNAL(rewardSupplied(double, int, int)));
		disconnect(sessionState_.data(), SIGNAL(signalChanged(QString, QStringList, QVector<float>)));
	}

	if (fileSessionLoader_)
	{
		disconnect(fileSessionLoader_.data(), SIGNAL(percentLoaded(double)));
	}

	if (sessionPlayer_)
	{
		disconnect(sessionPlayer_.data(), SIGNAL(reachedEnd()));
		disconnect(sessionPlayer_.data(), SIGNAL(loading(bool)));
		disconnect(sessionPlayer_.data(), SIGNAL(startingRun(QString, QString)));
	}
	*/


	sessionState_ = QSharedPointer<SessionState>(new SessionState(enableLfp_));
	fileSessionLoader_ = QSharedPointer<FileSessionLoader>(new FileSessionLoader(sessionState_));
	sessionPlayer_ = QSharedPointer<SessionPlayer>(new SessionPlayer(sessionState_,fileSessionLoader_));

	connect(sessionState_.data(),SIGNAL(propertyChanged(int,QString)),this,SIGNAL(propertyValueChanged(int,QString)));
	connect(sessionState_.data(),SIGNAL(propertyInitValueChanged(int,QString)),this,SIGNAL(propertyInitValueChanged(int,QString)));
	connect(sessionState_.data(),SIGNAL(transitionActivated(int)),this,SIGNAL(transitionActivated(int)));
	connect(sessionState_.data(),SIGNAL(framePresented(double)),this,SIGNAL(framePresented(double)));
	connect(sessionState_.data(),SIGNAL(rewardSupplied(double,int,int)),this,SIGNAL(rewardSupplied(double,int,int)));
	connect(sessionState_.data(), SIGNAL(signalChanged(QString, QStringList, QVector<float>)), this, SIGNAL(signalChanged(QString, QStringList, QVector<float>)));
	connect(sessionPlayer_.data(),SIGNAL(reachedEnd()),this,SLOT(reachedEnd()));
	connect(sessionPlayer_.data(),SIGNAL(loading(bool)),this,SIGNAL(loading(bool)));
	connect(sessionPlayer_.data(),SIGNAL(startingRun(QString,QString)),this,SIGNAL(startingRun(QString,QString)));
	connect(fileSessionLoader_.data(), SIGNAL(percentLoaded(double)), this, SIGNAL(percentLoaded(double)));

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
	//loadRun(0);
	return true;
}

/*! \brief Returns the DesignRoot from the Session.
 */
QSharedPointer<DesignRoot> PlaybackStateUpdater::getDesignRoot()
{
	return designRoot_;
}

/*! \brief Returns a list of the Session's Run names.
 */
QStringList PlaybackStateUpdater::getRuns()
{
	if(!fileSessionLoader_)
		return QStringList();
	return fileSessionLoader_->getRunNames();
}

/*! \brief Returns a list of the Session's saved Run names.
 */
QStringList PlaybackStateUpdater::getSavedRuns()
{
	if(!fileSessionLoader_)
		return QStringList();
	return fileSessionLoader_->getSavedRunNames();
}

/*! \brief Returns the length in seconds of the current Run.
 */
double PlaybackStateUpdater::getRunLength()
{
	if(!fileSessionLoader_)
		return -1;
	return currRunLength_;
}

/*! \brief Loads the run at the input index of the Session, getting it ready for playback.
 *	\note This function calls the FileSessionLoader::loadRun() function which takes a significant amount of time because
 *	it loads the Run's Session if it has not yet been loaded.
 */
bool PlaybackStateUpdater::loadRun(int index)
{
	if(!fileSessionLoader_)
		return false;
	if(index < 0)
		return false;
	if(index >= fileSessionLoader_->getRunNames().length())
		return false;
	paused_ = false;
	timerOffset_ = 0.0;
	suspendPlayback();
	runLoaded_ = false;
	currRunIndex_ = index;
	currRunLength_ = fileSessionLoader_->runDuration(currRunIndex_);
	//Make sure the lfp enabling is set up before loading the run
	if(sessionState_->lfpEnabled() != enableLfp_)
		sessionState_->enableLfp(enableLfp_);
	//Load the run
	return fileSessionLoader_->loadRun(index);
}

/*! \brief Pauses playback of the current Run.
 */
bool PlaybackStateUpdater::pause()
{
	paused_ = true;
	suspendPlayback();
	timerOffset_ = sessionPlayer_->getTime();
	return true;
}

/*! \brief Starts/resumes playback of the current Run.
 */
bool PlaybackStateUpdater::play()
{
	paused_ = false;
	return true;
}

/*! \brief Stops playback of the current Run.
 */
bool PlaybackStateUpdater::stop()
{
	paused_ = true;
	suspendPlayback();
	emit endingRun();

	timerOffset_ = 0.0;
	if(sessionPlayer_)
		sessionPlayer_->restart();
	return true;
}

/*! \brief Sets the playback speed.  Input value is the factor by which speed will be multiplied
 *	(2.0 doubles playback speed).
 */
void PlaybackStateUpdater::setPlaybackSpeed(double speed)
{
	if(speed <= 0)
		return;
	if(speed == playbackSpeed_)
		return;
	suspendPlayback();
	if(sessionPlayer_)
		timerOffset_ = sessionPlayer_->getTime();
	else
		timerOffset_ = 0;
	playbackSpeed_ = speed;	
}

/*! \brief Returns the current playback speed as a factor by which normal playback speed is multiplied 
 *	(2.0 means playback is running at double time).
 */
double PlaybackStateUpdater::getPlaybackSpeed()
{
	return playbackSpeed_;
}

/*! \brief Causes playback to jump to the input time.  During udpateState() calls.
 */
void PlaybackStateUpdater::jumpToTime(double time)
{
	if(!sessionPlayer_)
		return;
	if(time > getRunLength())
		time = getRunLength();
	if(time < 0)
		time = 0;
	timerOffset_ = time;
	suspendPlayback();
}

/*! \brief Enable/disables LFP loading.  When LFP loading is enabled, load times are significantly longer.
 */
void PlaybackStateUpdater::enableLfp(bool enable)
{
	enableLfp_ = enable;
}

/*! \brief Causes the disableRendering() signal to be emitted with an appropriate input.
 *	\details It is the responsibility of whoever connects to the disableRendering() signal to actually implement the
 *	rendering change.
 */
void PlaybackStateUpdater::enableRendering(bool en)
{
	if(renderingEnabled_ == en)
		return;
	emit disableRendering(!en);
	renderingEnabled_ = en;
}

/*! \brief This is used whenever playback needs to stop for some amount of time.  
 *	\details The function sets firstResumeFrame_ to true.  This way, when playback resumes timimg offsets won't be
 *	calculated according to the last time that the SessionPlayer was stepped forward but will be calculated according to
 *	the new resume time.
 */
void PlaybackStateUpdater::suspendPlayback()
{
	firstResumeFrame_ = true;
}
/*! \brief Used with suspendPlayback().  When this is called, if suspendPlayback() was called, the timer used to define
 *	the times sent into SessionPlayer::stepToTime() is restarted.  If we didn't do this, the timer would have kept running,
 *	and when we resumed from a pause playback would fast forward to where it would have been had pause never happened.
 *	\sa PlaybackStateUpdater::getPlaybackOffsetTime()
 */
void PlaybackStateUpdater::resumePlayback()
{
	if(firstResumeFrame_)
	{
		firstResumeFrame_ = false;
		timer_.restart();
	}
}

/*! \brief Returns true if playback was suspended.
 *	\sa suspendPlayback()
 */
bool PlaybackStateUpdater::playbackSuspended()
{
	return firstResumeFrame_;
}

/*! \brief Returns the time that should be used to offset the latest timerOffset_ value to generate the time used in
 *	SessionPlayer::stepToTime().
 */
double PlaybackStateUpdater::getPlaybackOffsetTime()
{
	if(!paused_ && !playbackSuspended())
	{
		return playbackSpeed_*timer_.elapsed()/1000.0;
	}
	return 0;
}

/*! \brief Called when the SessionPlayer indicates that a Run is over.  Stops playback and emits the finishedPlayback()
 *	signal.
 */
void PlaybackStateUpdater::reachedEnd()
{
	stop();
	emit finishedPlayback();
}