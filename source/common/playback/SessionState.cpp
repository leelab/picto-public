#include "SessionState.h"
using namespace Picto;

SessionState::SessionState() :
propCollection_(new PropertyCollectionState()),
transState_(new TransitionState()),
frameState_(new FrameState()),
rewardState_(new RewardState()),
lfpState_(new LfpState()),
spikeState_(new SpikeState())
{
	statesWithIds_.push_back(propCollection_);
	statesWithIds_.push_back(transState_);
	statesWithIds_.push_back(frameState_);

	statesWithTimes_.push_back(rewardState_);
	statesWithTimes_.push_back(lfpState_);
	statesWithTimes_.push_back(spikeState_);

	connect(propCollection_.data(),SIGNAL(propertyChanged(int,QString)),this,SIGNAL(propertyChanged(int,QString)));
	connect(transState_.data(),SIGNAL(transitionActivated(int)),this,SIGNAL(transitionActivated(int)));
	connect(frameState_.data(),SIGNAL(framePresented(double)),this,SIGNAL(framePresented(double)));
	connect(rewardState_.data(),SIGNAL(rewardSupplied(int,int)),this,SIGNAL(rewardSupplied(int,int)));
	connect(lfpState_.data(),SIGNAL(lfpChanged(int,double)),this,SIGNAL(lfpChanged(int,double)));
	connect(spikeState_.data(),SIGNAL(spikeEvent(int,int,QVector<float>)),this,SIGNAL(spikeEvent(int,int,QVector<float>)));
}

SessionState::~SessionState()
{
}

bool SessionState::reset()
{
	foreach(QSharedPointer<DataState<qulonglong>> state,statesWithIds_)
	{
		state->reset();
	}
	foreach(QSharedPointer<DataState<double>> state,statesWithTimes_)
	{
		state->reset();
	}
	emit wasReset();
	return false;
}

bool SessionState::setPropertyValue(double time,qulonglong dataId,int propId,QString value)
{
	return propCollection_->setPropertyValue(time,dataId,propId,value);
}

bool SessionState::setTransition(double time,qulonglong dataId,int transId)
{
	return transState_->setTransition(time,dataId,transId);
}

bool SessionState::setFrame(qulonglong dataId,double frameTime)
{
	return frameState_->setFrame(dataId,frameTime);
}

bool SessionState::setReward(double time,qulonglong dataId,int duration,int channel)
{
	return rewardState_->setReward(time,dataId,duration,channel);
}

bool SessionState::setSignal(QString name,QStringList subChanNames,double time,qulonglong dataId,double sampPeriod,QByteArray data)
{
	if(!signalLookup_.contains(name))
	{
		QSharedPointer<SignalState> newSigState = QSharedPointer<SignalState>(new SignalState(name,subChanNames));
		signalLookup_[name] = newSigState;
		statesWithTimes_.push_back(newSigState);
		connect(newSigState.data(),SIGNAL(signalChanged(QString,QVector<float>)),this,SIGNAL(signalChanged(QString,QVector<float>)));
	}
	return signalLookup_[name]->setSignal(time,dataId,sampPeriod,data);
}

bool SessionState::setLFP(qulonglong dataId,double startTime,int channel,QByteArray data)
{
	return lfpState_->setLFP(dataId,startTime,channel,data);
}

bool SessionState::setSpike(qulonglong dataId,double spikeTime,int channel,int unit,QByteArray waveform)
{
	return spikeState_->setSpike(dataId,spikeTime,channel,unit,waveform);
}

QSharedPointer<PropertyReader> SessionState::getPropertyReader(int propId)
{
	return propCollection_->getPropertyReader(propId);
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

QSharedPointer<PropertyCollectionState> SessionState::getPropertyCollectionState()
{
	return propCollection_;
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

QList<QSharedPointer<DataState<qulonglong>>> SessionState::getStatesIndexedById()
{
	return statesWithIds_;
}

QList<QSharedPointer<DataState<double>>> SessionState::getStatesIndexedByTime()
{
	return statesWithTimes_;
}
