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
			"FROM %1 s,frames f WHERE f.dataid=s.frameid ORDER BY s.dataid").arg(tableName_));

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

void SignalState::goToNext()
{
	if(curr_ < 0 && data_.size())
	{
		curr_ = 0;
		currSub_ = -1;
	}
	if(currSub_ >= data_[curr_].vals_.size()/numSubChans_-1)
	{
		if(curr_ >= data_.size()-1)
			return;
		curr_++;
		currSub_ = -1;
	}
	currSub_++;
}

PlaybackIndex SignalState::getNextIndex()
{
	Q_ASSERT(runStart_ >= 0);
	int nextCurr = curr_;
	int nextCurrSub = currSub_;
	if(nextCurr < 0 && data_.size())
	{
		nextCurr = 0;
		nextCurrSub = -1;
	}
	if(nextCurrSub >= data_[nextCurr].vals_.size()/numSubChans_-1)
	{
		if(nextCurr >= data_.size()-1)
			return PlaybackIndex();
		nextCurr++;
		nextCurrSub = -1;
	}
	nextCurrSub++;
	return globalToRunIndex(PlaybackIndex::minForTime(data_[nextCurr].time_+double(nextCurrSub)*sampPeriod_));
}

PlaybackIndex SignalState::globalToRunIndex(PlaybackIndex index)
{
	PlaybackIndex returnVal = index;
	returnVal.time() -= runStart_;
	return returnVal;
}