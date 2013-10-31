#include "SessionState.h"
using namespace Picto;

SessionState::SessionState(bool enableLfp) :
propState_(new PropertyState()),
transState_(new TransitionState()),
frameState_(new FrameState()),
rewardState_(new RewardState()),
runNotesState_(new RunNotesState()),
spikeState_(new SpikeState()),
lfpState_(new LfpState(enableLfp))
{
	statesWithIds_.push_back(propState_);
	statesWithIds_.push_back(transState_);
	statesWithIds_.push_back(frameState_);

	statesWithTimes_.push_back(rewardState_);
	statesWithTimes_.push_back(spikeState_);
	statesWithTimes_.push_back(lfpState_);

	connect(propState_.data(),SIGNAL(propertyChanged(int,QString)),this,SIGNAL(propertyChanged(int,QString)));
	connect(transState_.data(),SIGNAL(transitionActivated(int)),this,SIGNAL(transitionActivated(int)));
	connect(frameState_.data(),SIGNAL(framePresented(double)),this,SIGNAL(framePresented(double)));
	connect(rewardState_.data(),SIGNAL(rewardSupplied(double,int,int)),this,SIGNAL(rewardSupplied(double,int,int)));
	connect(spikeState_.data(),SIGNAL(spikeEvent(double,int,int,QVector<float>)),this,SIGNAL(spikeEvent(double,int,int,QVector<float>)));
	connect(lfpState_.data(),SIGNAL(lfpChanged(int,double)),this,SIGNAL(lfpChanged(int,double)));
	connect(lfpState_.data(),SIGNAL(lfpLoadProgress(int)),this,SLOT(lfpLoadingUpdated(int)));

	currRunStart_ = currRunEnd_ = -1;
	loadedStates_ = 0;
	totalSubStates_ = 0;
	lfpPercent_ = 100;
}


SessionState::~SessionState()
{
}

void SessionState::setSessionData(QSqlDatabase session,QHash<int,bool> obsoleteAssetIds)
{
	totalSubStates_ = statesWithIds_.size()+statesWithTimes_.size()+1;
	loadedStates_ = 0;
	lfpPercent_ = 100;
	currRunStart_ = currRunEnd_ = -1;
	propState_->setObsoleteAssets(obsoleteAssetIds);
	transState_->setObsoleteAssets(obsoleteAssetIds);
	foreach(QSharedPointer<DataState> state,statesWithIds_)
	{
		state->setDatabase(session);
		loadedStates_++;
		updatePercentLoaded();
	}
	foreach(QSharedPointer<DataState> state,statesWithTimes_)
	{
		state->setDatabase(session);
		loadedStates_++;
		updatePercentLoaded();
	}
	runNotesState_->setDatabase(session);
	loadedStates_++;
	updatePercentLoaded();
}

void SessionState::startNewRun(double startTime,double endTime)
{
	if(startTime < 0 || endTime < 0)
		return;
	foreach(QSharedPointer<DataState> state,statesWithIds_)
	{
		state->startRun(startTime,endTime);
	}
	foreach(QSharedPointer<DataState> state,statesWithTimes_)
	{
		state->startRun(startTime,endTime);
	}
	runNotesState_->startRun(startTime,endTime);
	currRunStart_ = startTime;
	currRunEnd_ = endTime;
}

void SessionState::restartRun()
{
	startNewRun(currRunStart_,currRunEnd_);
}

//bool SessionState::reset()
//{
//	foreach(QSharedPointer<DataState> state,statesWithIds_)
//	{
//		state->reset();
//	}
//	foreach(QSharedPointer<DataState> state,statesWithTimes_)
//	{
//		state->reset();
//	}
//	emit wasReset();
//	return false;
//}

void SessionState::addSignal(QString name,QString tableName,QStringList subChanNames,double sampPeriod)
{
	if(!signalLookup_.contains(name))
	{
		QSharedPointer<SignalState> newSigState = QSharedPointer<SignalState>(new SignalState(name,tableName,subChanNames,sampPeriod));
		signalLookup_[name] = newSigState;
		statesWithTimes_.push_back(newSigState);
		connect(newSigState.data(),SIGNAL(signalChanged(QString,QStringList,QVector<float>)),this,SIGNAL(signalChanged(QString,QStringList,QVector<float>)));
		//connect(newSigState.data(),SIGNAL(needsData(PlaybackIndex,PlaybackIndex)),this,SLOT(needsSignalData(PlaybackIndex,PlaybackIndex)));
		//connect(newSigState.data(),SIGNAL(needsNextData(PlaybackIndex,bool)),this,SLOT(needsNextSignalData(PlaybackIndex,bool)));
	}
}

void SessionState::enableLfp(bool enable)
{
	lfpState_->setEnabled(enable);
}

bool SessionState::lfpEnabled()
{
	if(!lfpState_)
		return false;
	return lfpState_->getEnabled();
}

//bool SessionState::setPropertyValue(double time,qulonglong dataId,int propId,QString value)
//{
//	return propState_->setPropertyValue(time,dataId,propId,value);
//}
//
//bool SessionState::setTransition(double time,qulonglong dataId,int transId)
//{
//	return transState_->setTransition(time,dataId,transId);
//}
//
//bool SessionState::setFrame(qulonglong dataId,double frameTime)
//{
//	return frameState_->setFrame(dataId,frameTime);
//}
//
//bool SessionState::setReward(double time,qulonglong dataId,int duration,int channel)
//{
//	return rewardState_->setReward(time,dataId,duration,channel);
//}
//
//bool SessionState::setSignal(QString name,QStringList subChanNames,double time,qulonglong dataId,double sampPeriod,QByteArray data)
//{
//	addSignal(name,subChanNames,sampPeriod);	//Adds this signal to our hash if its not already there.
//	return signalLookup_[name]->setSignal(time,dataId,sampPeriod,data);
//}
//
//bool SessionState::setLFP(qulonglong dataId,double startTime,double sampPeriod,int channel,QByteArray data)
//{
//	addLfpChannel(channel,sampPeriod);	//Adds this channel to our hash if its not already there.
//	return lfpLookup_[channel]->setLFP(dataId,startTime,channel,data);
//}
//
//bool SessionState::setSpike(qulonglong dataId,double spikeTime,int channel,int unit,QByteArray waveform)
//{
//	return spikeState_->setSpike(dataId,spikeTime,channel,unit,waveform);
//}
//
//void SessionState::setBehavioralBounds(double min,double max)
//{
//	propState_->setBoundTimes(min,max);
//	transState_->setBoundTimes(min,max);
//	frameState_->setBoundTimes(min,max);
//	rewardState_->setBoundTimes(min,max);
//	foreach(QSharedPointer<SignalState> sigState,signalLookup_)
//		sigState->setBoundTimes(min,max);
//}
//
//void SessionState::clearBehavioralData(double bound,bool before)
//{
//	if(before)
//	{
//		propState_->clearDataBefore(bound);
//		transState_->clearDataBefore(bound);
//		frameState_->clearDataBefore(bound);
//		rewardState_->clearDataBefore(bound);
//		foreach(QSharedPointer<SignalState> sigState,signalLookup_)
//			sigState->clearDataBefore(bound);
//	}
//
//}
//
//void SessionState::setBehavioralFinished()
//{
//	propState_->setFinishedLoading();
//	transState_->setFinishedLoading();
//	frameState_->setFinishedLoading();
//	rewardState_->setFinishedLoading();
//	foreach(QSharedPointer<SignalState> sigState,signalLookup_)
//		sigState->setFinishedLoading();
//}
//
//void SessionState::setNeuralBounds(double min,double max)
//{
//	spikeState_->setBoundTimes(min,max);
//	foreach(QSharedPointer<LfpState> lfpState,lfpLookup_)
//		lfpState->setBoundTimes(min,max);
//}
//
//void SessionState::clearNeuralData(double bound,bool before)
//{
//	if(before)
//	{
//		spikeState_->clearDataBefore(bound);
//		foreach(QSharedPointer<LfpState> lfpState,lfpLookup_)
//			lfpState->clearDataBefore(bound);
//	}
//}
//
//void SessionState::setNeuralFinished()
//{
//	spikeState_->setFinishedLoading();
//	foreach(QSharedPointer<LfpState> lfpState,lfpLookup_)
//		lfpState->setFinishedLoading();
//}

QSharedPointer<PropertyReader> SessionState::getPropertyReader()
{
	return propState_;
}

QSharedPointer<TransitionReader> SessionState::getTransitionReader()
{
	return transState_;
}

QSharedPointer<FrameReader> SessionState::getFrameReader()
{
	return frameState_;
}

QSharedPointer<RewardReader> SessionState::getRewardReader()
{
	return rewardState_;
}

QSharedPointer<RunNotesReader> SessionState::getRunNotesReader()
{
	return runNotesState_;
}

QStringList SessionState::getSignalReaderNames()
{
	return signalLookup_.keys();
}

QSharedPointer<SignalReader> SessionState::getSignalReader(QString name)
{
	if(signalLookup_.contains(name)) 
		return signalLookup_[name];
	return QSharedPointer<SignalState>();
}

QSharedPointer<LfpReader> SessionState::getLfpReader()
{
	return lfpState_;
}

QSharedPointer<SpikeReader> SessionState::getSpikeReader()
{
	return spikeState_;
}

QSharedPointer<PropertyState> SessionState::getPropertyState()
{
	return propState_;
}

QSharedPointer<TransitionState> SessionState::getTransitionState()
{
	return transState_;
}

QSharedPointer<FrameState> SessionState::getFrameState()
{
	return frameState_;
}

QSharedPointer<RewardState> SessionState::getRewardState()
{
	return rewardState_;
}

QSharedPointer<SignalState> SessionState::getSignalState(QString name)
{
	if(signalLookup_.contains(name)) 
		return signalLookup_[name];
	return QSharedPointer<SignalState>();
}

QSharedPointer<LfpState> SessionState::getLfpState()
{
	return lfpState_;
}

QSharedPointer<SpikeState> SessionState::getSpikeState()
{
	return spikeState_;
}

QList<QSharedPointer<DataState>> SessionState::getStatesIndexedById()
{
	return statesWithIds_;
}

QList<QSharedPointer<DataState>> SessionState::getStatesIndexedByTime()
{
	return statesWithTimes_;
}

void SessionState::updatePercentLoaded()
{
	//Handle lfp separately because its so big and so that
	//a meaningful value can appear here when it is being loaded
	//on its own
	double currLoadedStates = loadedStates_;
	if(lfpPercent_ < 100)
	{
		if(loadedStates_ == totalSubStates_)
			currLoadedStates = loadedStates_-1;
		currLoadedStates += double(lfpPercent_)/100.0;
	}
	if(totalSubStates_ > 0)
		emit percentLoaded(100.0*currLoadedStates/double(totalSubStates_));
}

void SessionState::lfpLoadingUpdated(int percent)
{
	//We handle lfp loading differently because its so big
	lfpPercent_ = percent;
	updatePercentLoaded();
}