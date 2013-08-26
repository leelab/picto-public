#include "SignalState.h"
#include "PlaybackData.h"
using namespace Picto;

SignalState::SignalState(QString name,QString tableName,QStringList subChanNames,double sampPeriod) :
name_(name),
tableName_(tableName),
subChanNames_(subChanNames),
numSubChans_(subChanNames_.size()),
sampPeriod_(sampPeriod)
{
	Q_ASSERT(numSubChans_);
	for(int i=0;i<subChanNames_.size();i++)
	{
		subChanIndexLookup_[subChanNames_[i]] = i;
	}
}

void SignalState::setDatabase(QSqlDatabase session)
{
	runStart_ = runEnd_ = curr_ = -1;
	session_ = session;
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(session_));
	query_->exec(QString("SELECT COUNT(*) FROM %1").arg(tableName_));
	if(!query_->exec() || !query_->next())
	{
		Q_ASSERT(false);
		return;
	}
	data_.resize(query_->value(0).toInt());

	query_->exec(QString("SELECT f.time,s.offsettime,s.data "
			"FROM %1 s,frames f WHERE f.dataid=s.frameid ORDER BY f.dataid").arg(tableName_));

	if(!query_->exec())
	{
		Q_ASSERT(false);
		return;
	}
	int assetId;
	PlaybackIndex index;
	QString val;
	int arrayIndex = 0;
	QByteArray byteArray;
	int numEntries;
	float* floatArray;
	while(query_->next())
	{
		//Note: With signals, the definition is such that offsetTime after the frameTime of frameId is when the first signal data was read.
		data_[arrayIndex] = PlaybackSignalData(query_->value(0).toDouble()+query_->value(1).toDouble());
		byteArray = query_->value(2).toByteArray();
		numEntries = byteArray.size()/sizeof(float);
		floatArray = reinterpret_cast<float*>(byteArray.data());
		data_[arrayIndex].vals_.resize(numEntries);
		for(int i=0;i<numEntries;i++)
		{
			data_[arrayIndex].vals_[i] = floatArray[i];
		}
		arrayIndex++;
	}
	Q_ASSERT(data_.size() == arrayIndex);
}

void SignalState::startRun(double runStartTime,double runEndTime)
{
	Q_ASSERT(session_.isOpen());
	runStart_ = runStartTime;
	runEnd_ = runEndTime;
	//Move curr_ to first property in run
	curr_ = -1;
	currSub_ = -1;
	PlaybackIndex nextIndex = getNextIndex();
	while(nextIndex.isValid() && nextIndex.time() < 0)
	{
		goToNext();
		nextIndex = getNextIndex();
	}
}

PlaybackIndex SignalState::getCurrentIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(currSub_ < 0)
		return PlaybackIndex();
	Q_ASSERT(curr_ >= 0);
	return globalToRunIndex(PlaybackIndex::minForTime(data_[curr_].time_+double(currSub_)*sampPeriod_));
}

PlaybackIndex SignalState::getNextIndex(double lookForwardTime)
{
	PlaybackIndex returnVal = getNextIndex();
	if(returnVal.time() > lookForwardTime)
		return PlaybackIndex();
	return returnVal;
}

void SignalState::moveToIndex(PlaybackIndex index)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(index >= getCurrentIndex());
	PlaybackIndex nextIndex = getNextIndex();
	QVector<float> sigVec(numSubChans_);
	while(nextIndex.isValid() && nextIndex <= index)
	{
		goToNext();
		for(int i=0;i<numSubChans_;i++)
		{
			sigVec[i]=data_[curr_].vals_[currSub_*numSubChans_+i];
		}
		emit signalChanged(name_,subChanNames_,sigVec);
		nextIndex = getNextIndex();
	}
}

QString SignalState::getName()
{
	return name_;
}

QStringList SignalState::getComponentNames()
{
	return subChanNames_;
}

double SignalState::getSamplePeriod()
{
	return sampPeriod_;
}

double SignalState::getLatestTime()
{
	return getCurrentIndex().time();
}

double SignalState::getLatestValue(QString channel)
{
	if(!subChanIndexLookup_.contains(channel))
		return 0;
	int index = subChanIndexLookup_[channel];
	return data_[curr_].vals_[currSub_*numSubChans_+index];
}

double SignalState::getNextTime()
{
	return getNextIndex().time();
}

double SignalState::getNextValue(QString channel)
{
	if(!subChanIndexLookup_.contains(channel))
		return 0;
	int index = subChanIndexLookup_[channel];
	int nextCurr = curr_;
	int nextCurrSub = currSub_;
	if(!moveIndecesToNextTime(nextCurr,nextCurrSub))
		return 0;
	return data_[nextCurr].vals_[nextCurrSub*numSubChans_+index];
}

//Returns signal values for the input sub channel with times > the input time.
QVariantList SignalState::getValuesSince(QString channel,double time)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(curr_ >= 0);
	if(!subChanIndexLookup_.contains(channel))
		return QVariantList();
	int chanIndex = subChanIndexLookup_[channel];
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	double upToGlobalTime = data_[curr_].time_+double(currSub_)*sampPeriod_;
	PlaybackSignalData beyondVal = PlaybackSignalData(afterTime+runStart_);
	QVector<PlaybackSignalData>::iterator iter = qLowerBound<QVector<PlaybackSignalData>::iterator,PlaybackSignalData>(data_.begin(),data_.begin()+curr_,beyondVal);
	while(iter != data_.end() && iter->time_+(sampPeriod_ * iter->vals_.size()/numSubChans_) <= beyondVal.time_)
		iter++;
	QVariantList returnVal;
	double sampleTime;
	for(;(iter->time_ <= upToGlobalTime) && (iter != data_.end());iter++)
	{
		for(int i=0;i<iter->vals_.size();i+=numSubChans_)
		{
			sampleTime = iter->time_ + (i/numSubChans_)*sampPeriod_;
			if(sampleTime <= beyondVal.time_)
				continue;
			if(sampleTime > upToGlobalTime)
				break;
			returnVal.append(iter->vals_[i+chanIndex]);
		}
	}
	Q_ASSERT(iter != data_.end());
	return returnVal;
}

QVariantList SignalState::getValuesUntil(QString channel,double time)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(curr_ >= 0);
	if(!subChanIndexLookup_.contains(channel))
		return QVariantList();
	int chanIndex = subChanIndexLookup_[channel];
	double beforeTime = time;
	if(beforeTime <= getLatestTime())
		return QVariantList();
	double upToGlobalTime = beforeTime+runStart_;
	double currGlobalTime = data_[curr_].time_+double(currSub_)*sampPeriod_;
	QVector<PlaybackSignalData>::iterator iter = data_.begin() + curr_;
	QVariantList returnVal;
	double sampleTime;
	for(;(iter->time_ <= upToGlobalTime) && (iter != data_.end());iter++)
	{
		for(int i=0;i<iter->vals_.size();i+=numSubChans_)
		{
			sampleTime = iter->time_ + (i/numSubChans_)*sampPeriod_;
			if(sampleTime <= currGlobalTime)
				continue;
			if(sampleTime > upToGlobalTime)
				break;
			returnVal.append(iter->vals_[i+chanIndex]);
		}
	}
	Q_ASSERT(iter != data_.end());
	return returnVal;
}

void SignalState::goToNext()
{
	moveIndecesToNextTime(curr_,currSub_);
}

PlaybackIndex SignalState::getNextIndex()
{
	int nextCurr = curr_;
	int nextCurrSub = currSub_;
	if(!moveIndecesToNextTime(nextCurr,nextCurrSub))
		return PlaybackIndex();
	return globalToRunIndex(PlaybackIndex::minForTime(data_[nextCurr].time_+double(nextCurrSub)*sampPeriod_));
}

PlaybackIndex SignalState::globalToRunIndex(PlaybackIndex index)
{
	PlaybackIndex returnVal = index;
	returnVal.time() -= runStart_;
	return returnVal;
}

bool SignalState::moveIndecesToNextTime(int& outerIndex, int& innerIndex)
{
	Q_ASSERT(runStart_ >= 0);
	int nextOuterIndex = outerIndex;
	int nextInnerIndex = innerIndex;
	if(nextOuterIndex < 0 && data_.size())
	{
		nextOuterIndex = 0;
		nextInnerIndex = -1;
	}
	if(nextInnerIndex >= data_[nextOuterIndex].vals_.size()/numSubChans_-1)
	{
		if(nextOuterIndex >= data_.size()-1)
			return false;
		nextOuterIndex++;
		nextInnerIndex = -1;
	}
	nextInnerIndex++;
	outerIndex = nextOuterIndex;
	innerIndex = nextInnerIndex;
	return true;
}