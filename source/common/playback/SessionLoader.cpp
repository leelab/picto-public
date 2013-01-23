//#include <QMutexLocker>
//#include "SessionLoader.h"
//using namespace Picto;
//
//SessionLoader::SessionLoader(QSharedPointer<SessionState> sessState) :
//sessionState_(sessState),
//loaderThread_(new SessionLoaderThread())
//{
//	minBehav_ = 0;
//	maxBehav_ = 0;
//	minNeural_ = 0;
//	maxNeural_ = 0;
//	runStart_ = 0;
//	runEnd_ = 0;
//	currTime_ = 0;
//	procTime_ = 0;
//	forwardBuffer_ = 6000;
//	backBuffer_ = 600;
//	runReset_ = false;
//	runIndex_ = -1;
//	loaded_ = false;
//
//	mutex_ = QSharedPointer<QMutex>(new QMutex(QMutex::Recursive));
//	connect(loaderThread_.data(),SIGNAL(doLoad()),this,SLOT(loadData()));
//}
//
//SessionLoader::~SessionLoader()
//{
//	QMutexLocker locker(mutex_.data());
//	Q_ASSERT_X(!loaded_,"SessionLoader::~SessionLoader","Unload() must be called before deleting a SessionLoader");
//}
//
//QStringList SessionLoader::getRunNames()
//{
//	QMutexLocker locker(mutex_.data());
//	QStringList returnVal;
//	runs_ = loadRunData();
//	foreach(RunData runData,runs_)
//	{
//		returnVal.append(runData.name_);
//	}
//	return returnVal;
//}
//
//void SessionLoader::restart()
//{
//	QMutexLocker locker(mutex_.data());
//	currTime_ = 0;
//	runReset_ = true;
//}
//
//bool SessionLoader::loadRun(int runIndex)
//{
//	//End loaderThread if its already running.
//	loaderThread_->quit();
//	loaderThread_->wait();
//
//	//Restart loaderThread and move this object's event loop to that thread.
//	loaderThread_->start();
//	moveToThread(loaderThread_.data());
//
//	//Reset runIndex and call restart to reinitialize variables
//	QMutexLocker locker(mutex_.data());
//	runs_ = loadRunData();
//	if(runIndex < 0 || runIndex >= runs_.size())
//		return false;
//	runIndex_ = runIndex;
//	loaded_ = true;
//	restart();
//}
//
//bool SessionLoader::setCurrentTime(double time)
//{
//	qDebug(QString("Loader: Current Time Set:%1").arg(time).toLatin1());
//	double realTime = speedTimer_.restart()/1000.0;
//	QMutexLocker locker(mutex_.data());
//	time += runStart_;
//	if(!runLoaded())
//		return false;
//	if(time < runStart_)
//		return true;
//	if(time <= currTime_)
//		return true;
//	if(runEnd_ && time > runEnd_)
//		time = runEnd_;
//	currTime_ = time;
//	return true;
//}
//
//void SessionLoader::setProcessedTime(double time)
//{
//	QMutexLocker locker(mutex_.data());
//	time += runStart_;
//	if(!runLoaded())
//		return;
//	if(time < runStart_)
//		return;
//	if(time > currTime_)
//		return;
//	procTime_ = time;
//}
//
//bool SessionLoader::runLoaded()
//{
//	QMutexLocker locker(mutex_.data());
//	return runStart_ != runEnd_;
//}
//
//double SessionLoader::getMinBehavTime()
//{
//	QMutexLocker locker(mutex_.data());
//	return minBehav_-runStart_;
//}
//
//double SessionLoader::getMaxBehavTime()
//{
//	QMutexLocker locker(mutex_.data());
//	return maxBehav_-runStart_;
//}
//
//double SessionLoader::getMinNeuralTime()
//{
//	QMutexLocker locker(mutex_.data());
//	return minNeural_-runStart_;
//}
//
//double SessionLoader::getMaxNeuralTime()
//{
//	QMutexLocker locker(mutex_.data());
//	return maxNeural_-runStart_;
//}
//
//double SessionLoader::currRunDuration()
//{
//	QMutexLocker locker(mutex_.data());
//	if(!runEnd_)
//		return 10000000000000;
//	return runEnd_-runStart_;
//}
//
//double SessionLoader::runDuration(int index)
//{
//	QMutexLocker locker(mutex_.data());
//	runs_ = loadRunData();
//	if(index < 0 || index >= runs_.size())
//		return -1;
//	if(runs_[index].endTime_ < 0)
//		return -1;
//	return 	runs_[index].endTime_-runs_[index].startTime_;
//}
//
//bool SessionLoader::dataIsReady(double time)
//{
//	QMutexLocker locker(mutex_.data());
//	time = time+runStart_;
//	if(
//		(	time > maxBehav_
//		||	time > maxNeural_)
//		&& time < currRunDuration())
//		return false;
//	if(
//		time < minBehav_
//		||	time < minNeural_
//		)
//		return false;
//	return true;
//}
//
//void SessionLoader::unload()
//{
//	loaderThread_->quit();
//	loaderThread_->wait();
//	QMutexLocker locker(mutex_.data());
//	loaded_ = false;
//	locker.unlock();
//}
//
//void SessionLoader::loadData()
//{
//	QMutexLocker locker(mutex_.data());
//	Q_ASSERT(loaded_);
//	if(runReset_)
//	{
//		runStart_ = runs_[runIndex_].startTime_;
//		runEnd_ = runs_[runIndex_].endTime_;
//		if(runEnd_ < 0)
//			runEnd_ = 0;
//		currTime_ = 0;
//		procTime_ = 0;
//		
//		//Subtract tiny value from start times to be sure that property changes 
//		//in the first frame of the run are included
//		maxBehav_ = runStart_-.000001;
//		minBehav_ = runStart_-.000001;
//		maxNeural_ = runStart_-.000001;
//		minNeural_ = runStart_-.000001;
//		runReset_ = false;
//		locker.unlock();
//		sessionState_->reset();
//		loadInitData(runStart_);
//		locker.relock();
//	}
//	if(!runLoaded())
//		return;
//	double maxTime = currTime_+forwardBuffer_;
//	if(runEnd_ && maxTime > runEnd_)
//		maxTime = runEnd_;
//
//	double loadTimePeriod = 0;
//	double newMaxBehav = maxBehav_;
//	double newMinBehav = minBehav_;
//	double newMaxNeural = maxNeural_;
//	double newMinNeural = minNeural_;
//	locker.unlock();
//	double currBehavLookback = procTime_ - minBehav_;
//	double currNeuralLookback = procTime_ - minNeural_;
//	if(currBehavLookback > backBuffer_)
//		newMinBehav = procTime_-backBuffer_;
//	if(currNeuralLookback > backBuffer_)
//		newMinNeural = procTime_-backBuffer_;
//	if(maxTime > maxBehav_)
//	{
//		qDebug(QString("Loader: Load Behav Data From: %1 Until Time:%2").arg(maxBehav_-runStart_).arg(maxTime-runStart_).toLatin1());
//		newMaxBehav = loadBehavData(maxBehav_,maxTime,runStart_);
//	}
//	if(maxTime > maxNeural_)
//	{
//		qDebug(QString("Loader: Load Neura Data From: %1 Until Time:%2").arg(maxNeural_-runStart_).arg(maxTime-runStart_).toLatin1());
//		newMaxNeural = loadNeuralData(maxNeural_,maxTime,runStart_);
//	}
//	if(newMinBehav != minBehav_)
//	{
//		qDebug(QString("Loader: Clear Behav Data Before: %1").arg(newMinBehav-runStart_).toLatin1());
//		sessionState_->clearBehavioralData(newMinBehav-runStart_,true);
//	}
//	if(newMinNeural != minNeural_)
//	{
//		qDebug(QString("Loader: Clear Neura Data Before: %1").arg(newMinNeural-runStart_).toLatin1());
//		sessionState_->clearNeuralData(newMinNeural-runStart_,true);
//	}
//
//	if(maxBehav_ != newMaxBehav || minBehav_ != newMinBehav)
//	{
//		qDebug(QString("Loader: Set Behav Bounds From: %1 to: %2").arg(newMinBehav-runStart_).arg(newMaxBehav-runStart_).toLatin1());
//		sessionState_->setBehavioralBounds(newMinBehav-runStart_,newMaxBehav-runStart_);
//	}
//	if(maxNeural_ != newMaxNeural || minNeural_ != newMinNeural)
//	{
//		qDebug(QString("Loader: Set Neura Bounds From: %1 to: %2").arg(newMinNeural-runStart_).arg(newMaxNeural-runStart_).toLatin1());
//		sessionState_->setNeuralBounds(newMinNeural-runStart_,newMaxNeural-runStart_);
//	}
//	if(runEnd_ && maxTime >= runEnd_)
//	{	//Hmm.. Maybe neural data won't have arrived yet.  Look into this.
//		sessionState_->setBehavioralFinished();
//		sessionState_->setNeuralFinished();
//	}
//	double bIncrease = newMaxBehav-maxBehav_;
//	double nIncrease = newMaxNeural-maxNeural_;
//	loadTimePeriod = bIncrease<nIncrease?bIncrease:nIncrease;
//
//	locker.relock();
//	maxBehav_ = newMaxBehav;
//	minBehav_ = newMinBehav;
//	maxNeural_ = newMaxNeural;
//	minNeural_ = newMinNeural;
//}