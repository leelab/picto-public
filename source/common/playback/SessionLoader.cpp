#include <QMutexLocker>
#include "SessionLoader.h"
using namespace Picto;

SessionLoader::SessionLoader(QSharedPointer<SessionState> sessState) :
sessionState_(sessState)
{
	minBehav_ = 0;
	maxBehav_ = 0;
	minNeural_ = 0;
	maxNeural_ = 0;
	runStart_ = 0;
	runEnd_ = 0;
	currTime_ = 0;
	runSpeed_ = 1;
	loadSpeed_ = 1;
	loadPeriod_ = 10;
	forwardBuffer_ = 6000;
	backBuffer_ = 600;
	runReset_ = false;
	runIndex_ = -1;
	insufficientData_ = false;

	mutex_ = QSharedPointer<QMutex>(new QMutex(QMutex::Recursive));
	start();
}

SessionLoader::~SessionLoader()
{
}

QStringList SessionLoader::getRunNames()
{
	QMutexLocker locker(mutex_.data());
	QStringList returnVal;
	runs_ = loadRunData();
	foreach(RunData runData,runs_)
	{
		returnVal.append(runData.name_);
	}
	return returnVal;
}

void SessionLoader::restart()
{
	QMutexLocker locker(mutex_.data());
	currTime_ = 0;
	runReset_ = true;
}

bool SessionLoader::loadRun(int runIndex)
{
	QMutexLocker locker(mutex_.data());
	runs_ = loadRunData();
	if(runIndex < 0 || runIndex >= runs_.size())
		return false;
	runIndex_ = runIndex;
	restart();
}

bool SessionLoader::setCurrentTime(double time)
{
	qDebug(QString("Loader: Current Time Set:%1").arg(time).toAscii());
	double realTime = speedTimer_.restart()/1000.0;
	QMutexLocker locker(mutex_.data());
	time += runStart_;
	if(!runLoaded())
		return false;
	if(time < runStart_)
		return true;
	if(time <= currTime_)
		return true;
	if(runEnd_ && time > runEnd_)
		time = runEnd_;
	currTime_ = time;
	if(realTime != 0 && !insufficientData_)
	{
		double offsetTime = time - currTime_;
		double newRunSpeed = offsetTime/realTime;
		runSpeed_ = runSpeed_ + 0.1*(offsetTime/realTime-runSpeed_);
		if(runSpeed_ < 1)
			runSpeed_ = 1;
		qDebug(QString("Loader: Run Speed:%1").arg(runSpeed_).toAscii());
	}
	if(!insufficientData_ && !dataLoadedForTime(currTime_))
	{
		insufficientData_ = true;
		qDebug(QString("Loader: Needs Data").toAscii());
	}
	return true;
}

bool SessionLoader::runLoaded()
{
	QMutexLocker locker(mutex_.data());
	return runStart_ != runEnd_;
}

double SessionLoader::getMinBehavTime()
{
	QMutexLocker locker(mutex_.data());
	return minBehav_-runStart_;
}

double SessionLoader::getMaxBehavTime()
{
	QMutexLocker locker(mutex_.data());
	return maxBehav_-runStart_;
}

double SessionLoader::getMinNeuralTime()
{
	QMutexLocker locker(mutex_.data());
	return minNeural_-runStart_;
}

double SessionLoader::getMaxNeuralTime()
{
	QMutexLocker locker(mutex_.data());
	return maxNeural_-runStart_;
}

double SessionLoader::runDuration()
{
	QMutexLocker locker(mutex_.data());
	if(!runEnd_)
		return 10000000000000;
	return runEnd_-runStart_;
}

bool SessionLoader::dataIsReady()
{
	QMutexLocker locker(mutex_.data());
	return !insufficientData_;
}

void SessionLoader::run()
{
	//We establish the timer_ here so that we can start and stop it from this thread
	//if it was created/deleted in the constructor/destructor it wouldn't be directly accessible
	loadCaller_ = QSharedPointer<QTimer>(new QTimer());
	loadCaller_->setInterval(1);
	connect(loadCaller_.data(), SIGNAL(timeout()), this, SLOT(loadData()), Qt::DirectConnection);
	loadCaller_->start();

	QThread::exec();
}

void SessionLoader::loadData()
{
	QMutexLocker locker(mutex_.data());
	if(runReset_)
	{
		runStart_ = runs_[runIndex_].startTime_;
		runEnd_ = runs_[runIndex_].endTime_;
		if(runEnd_ < 0)
			runEnd_ = 0;
		currTime_ = 0;
		maxBehav_ = runStart_;
		minBehav_ = runStart_;
		maxNeural_ = runStart_;
		minNeural_ = runStart_;
		runSpeed_ = 1.0;
		loadSpeed_ = 1.0;
		runReset_ = false;
		locker.unlock();
		sessionState_->reset();
		loadInitData(runStart_);
		locker.relock();
	}
	if(!runLoaded())
		return;
	double maxTime = currTime_+forwardBuffer_;
	if(runEnd_ && maxTime > runEnd_)
		maxTime = runEnd_;

	double loadTimePeriod = 0;
	double newMaxBehav = maxBehav_;
	double newMinBehav = minBehav_;
	double newMaxNeural = maxNeural_;
	double newMinNeural = minNeural_;
	locker.unlock();
	double currBehavLookback = currTime_ - minBehav_;
	double currNeuralLookback = currTime_ - minNeural_;
	if(currBehavLookback > backBuffer_)
		newMinBehav = currTime_-backBuffer_;
	if(currNeuralLookback > backBuffer_)
		newMinNeural = currTime_-backBuffer_;
	if(maxTime > maxBehav_)
	{
		qDebug(QString("Loader: Load Behav Data From: %1 Until Time:%2").arg(maxBehav_-runStart_).arg(maxTime-runStart_).toAscii());
		newMaxBehav = loadBehavData(maxBehav_,maxTime,runStart_);
	}
	if(maxTime > maxNeural_)
	{
		qDebug(QString("Loader: Load Neura Data From: %1 Until Time:%2").arg(maxNeural_-runStart_).arg(maxTime-runStart_).toAscii());
		newMaxNeural = loadNeuralData(maxNeural_,maxTime,runStart_);
	}
	if(newMinBehav != minBehav_)
	{
		qDebug(QString("Loader: Clear Behav Data Before: %1").arg(newMinBehav-runStart_).toAscii());
		sessionState_->clearBehavioralData(newMinBehav-runStart_,true);
	}
	if(newMinNeural != minNeural_)
	{
		qDebug(QString("Loader: Clear Neura Data Before: %1").arg(newMinNeural-runStart_).toAscii());
		sessionState_->clearNeuralData(newMinNeural-runStart_,true);
	}

	if(maxBehav_ != newMaxBehav || minBehav_ != newMinBehav)
	{
		qDebug(QString("Loader: Set Behav Bounds From: %1 to: %2").arg(newMinBehav-runStart_).arg(newMaxBehav-runStart_).toAscii());
		sessionState_->setBehavioralBounds(newMinBehav-runStart_,newMaxBehav-runStart_);
	}
	if(maxNeural_ != newMaxNeural || minNeural_ != newMinNeural)
	{
		qDebug(QString("Loader: Set Neura Bounds From: %1 to: %2").arg(newMinNeural-runStart_).arg(newMaxNeural-runStart_).toAscii());
		sessionState_->setNeuralBounds(newMinNeural-runStart_,newMaxNeural-runStart_);
	}
	if(runEnd_ && maxTime >= runEnd_)
	{	//Hmm.. Maybe neural data won't have arrived yet.  Look into this.
		sessionState_->setBehavioralFinished();
		sessionState_->setNeuralFinished();
	}
	double bIncrease = newMaxBehav-maxBehav_;
	double nIncrease = newMaxNeural-maxNeural_;
	loadTimePeriod = bIncrease<nIncrease?bIncrease:nIncrease;

	locker.relock();
	maxBehav_ = newMaxBehav;
	minBehav_ = newMinBehav;
	maxNeural_ = newMaxNeural;
	minNeural_ = newMinNeural;
	if(insufficientData_ && dataLoadedForTime(currTime_))
	{
		insufficientData_ = false;
		qDebug(QString("Loader: Done loading").toAscii());
	}

	//double loadDelay = loadTimer_.restart()/1000.0;
	//if(loadDelay > 0)
	//{
	//	loadPeriod_ += .1 * (loadDelay - loadPeriod_);
	//	if(loadTimePeriod > 0)
	//	{
	//		double newLoadSpeed = loadTimePeriod/loadDelay;
	//		loadSpeed_ += .1 * (loadTimePeriod/loadDelay - loadSpeed_);
	//	}
	//}
	//if(loadSpeed_ > 0 && runSpeed_ > 0)
	//{
	//	double minMaxTime = maxBehav_>maxNeural_?maxNeural_:maxBehav_;
	//	//
	//	//r = run speed	 (experimental secs played per real sec)
	//	//l = load speed (experimental secs loaded per real sec)
	//	//n = experimental secs to load by next time here
	//	//a = desired real secs that loading should be ahead at next time here
	//	//b = current experimental secs that loading is ahead
	//	//
	//	//
	//	//real time when we get back here = n/l
	//	//exp play time when we get back here = r*n/l
	//	//
	//	//n should be "a" real secs ahead when we arrive back here
	//	//n = (r*n/l)+(a*r)-b
	//	//Solve for n:
	//	//n = ((a*r)-b)/(1-(r/l))
	//	double a = 30;
	//	double r = runSpeed_;
	//	double l = loadSpeed_;
	//	double b = minMaxTime - currTime_;
	//	if(l >= 0 && ((1-(r/l)) != 0))
	//	{
	//		double n = ((a*r)-b)/(1-(r/l));
	//		forwardBuffer_ = n;
	//		if(forwardBuffer_ < 0)
	//			forwardBuffer_ = 0;
	//		qDebug(QString("Loader: a: %1, r: %2, l: %3\n"
	//			"b:%4, n:%5, forwardBuffer_:%6").arg(a).arg(r).arg(l).arg(b).arg(n).arg(forwardBuffer_).toAscii());
	//	}
	//}
}

bool SessionLoader::dataLoadedForTime(double time)
{
	QMutexLocker locker(mutex_.data());
	if(
			time > maxBehav_
		||	time > maxNeural_
		||	time < minBehav_
		||	time < minNeural_
		)
		return false;
	return true;
}	