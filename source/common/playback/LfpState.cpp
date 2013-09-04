#include "LfpState.h"
#include "../storage/DataSourceInfo.h"
#include "PlaybackData.h"
using namespace Picto;

LfpState::LfpState()
{
}

void LfpState::setDatabase(QSqlDatabase session)
{
	runStart_ = runEnd_ = curr_ = -1;
	session_ = session;
	data_.clear();
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(session_));

	//Set up lfp hash
	query_->prepare("SELECT DISTINCT channel FROM lfp ORDER BY channel");
	if(!query_->exec() || !query_->next())
	{
		return;
	}
	int index = 0;
	do
	{
		chanIndexMap_[query_->value(0).toInt()] = index++;
		channels_.append(query_->value(0).toInt());
	} while(query_->next());

	//Get lfp resolution
	sampPeriod_ = 0;
	query_->prepare("SELECT value FROM sessioninfo WHERE key='DataSource'");
	if(!query_->exec())
	{
		return;
	}
	while(query_->next())
	{
		DataSourceInfo info;
		info.fromXml(query_->value(0).toString());
		if(info.getName() == "lfp")
		{
			sampPeriod_ = info.getResolution();
			break;
		}
	}
	if(sampPeriod_ <= 0)
		return;

	//Get minimum lfp time
	query_->prepare("SELECT timestamp from lfp ORDER BY timestamp LIMIT 1");
	if(!query_->exec() || !query_->next())
	{
		return;
	}
	double minTime = query_->value(0).toDouble();

	//Get maximum lfp time
	//Start by getting highest timestamp entries for all channels
	query_->prepare("SELECT DISTINCT channel,timestamp,data FROM lfp ORDER BY timestamp DESC LIMIT (SELECT COUNT(channel) FROM lfp)");
	if(!query_->exec())
	{
		return;
	}
	double maxTime = minTime-1;
	while(query_->next())
	{
		double currHighest = query_->value(1).toDouble() + (sampPeriod_ * query_->value(2).toByteArray().size()/sizeof(float));
		if(currHighest > maxTime)
			maxTime = currHighest;
	}
	Q_ASSERT(maxTime >= minTime);

	//Create data list with an entry for every sample time from minTime to maxTime
	data_.reserve(int(maxTime-minTime)/sampPeriod_);
	for(double time = minTime;time <= maxTime;time+=sampPeriod_)
	{
		data_.append(PlaybackLFPData(time));
	}
	int numChannels = chanIndexMap_.size();
	for(QList<PlaybackLFPData>::Iterator iter = data_.begin();iter != data_.end();iter++)
	{
		iter->initializeValArray(numChannels);
	}

	//Go through each channel in database and fill data list
	foreach(int channel,chanIndexMap_.keys())
	{
		int valueIndex = chanIndexMap_[channel];
		//Get data and load it into this channel's list from the lfp hash
		query_->prepare("SELECT timestamp,data FROM lfp WHERE channel=:channel "
		"ORDER BY timestamp");
		query_->bindValue(":channel",channel);
		if(!query_->exec())
		{
			Q_ASSERT(false);
			return;
		}
		QList<PlaybackLFPData>::Iterator dataIt = data_.begin();
		double startTime;
		double currTime;
		PlaybackLFPData* currObject;
		QList<PlaybackLFPData>::Iterator curr;
		float latestValue = 0;

		while(query_->next())
		{
			startTime = query_->value(0).toDouble();
			QByteArray dataArray = query_->value(1).toByteArray();
			const float* dataArrayStart = reinterpret_cast<const float*>(dataArray.constData());
			int dataArraySize = dataArray.size()/sizeof(float);
			for(int i=0;i<dataArraySize;i++)
			{
				currTime = startTime + i*sampPeriod_;
				while( (dataIt != data_.end()) && (dataIt->time_ <= currTime) )
				{
					dataIt->setValue(valueIndex,latestValue);
					dataIt++;
				}
				Q_ASSERT(dataIt != data_.begin());
				curr = (dataIt-1);
				latestValue = *(dataArrayStart + i);
				Q_ASSERT(currTime - curr->time_ <= sampPeriod_);
				curr->setValue(valueIndex,latestValue);
			}
		}
	}
}
void LfpState::startRun(double runStartTime,double runEndTime)
{
	Q_ASSERT(session_.isOpen());
	runStart_ = runStartTime;
	runEnd_ = runEndTime;
	//Move curr_ to first property in run
	curr_ = -1;
	PlaybackIndex nextIndex = getNextIndex();
	while(nextIndex.isValid() && nextIndex.time() < 0)
	{
		curr_++;
		nextIndex = getNextIndex();
	}
}

PlaybackIndex LfpState::getCurrentIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0)
		return PlaybackIndex();
	return globalToRunIndex(PlaybackIndex::minForTime(data_[curr_].time_));
}

PlaybackIndex LfpState::getNextIndex(double lookForwardTime)
{
	PlaybackIndex returnVal = getNextIndex();
	if(returnVal.time() > lookForwardTime)
		return PlaybackIndex();
	return returnVal;
}

void LfpState::moveToIndex(PlaybackIndex index)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(index >= getCurrentIndex());
	PlaybackIndex nextIndex = getNextIndex();
	while(nextIndex.isValid() && nextIndex <= index)
	{
		curr_++;
		for(QHash<int,int>::Iterator iter = chanIndexMap_.begin();iter!=chanIndexMap_.end();iter++)
		{
			emit lfpChanged(iter.key(),data_[curr_].getValue(iter.value()));
		}
		nextIndex = getNextIndex();
	}
}

QVariantList LfpState::getChannels()
{
	return channels_;
}

double LfpState::getSamplePeriod()
{
	return sampPeriod_;
}

double LfpState::getLatestTime()
{
	return getCurrentIndex().time();
}

double LfpState::getLatestValue(int channel)
{
	if(!chanIndexMap_.contains(channel))
		return 0;
	int index = chanIndexMap_[channel];
	return data_[curr_].getValue(index);
}

double LfpState::getNextTime()
{
	return getNextIndex().time();
}

double LfpState::getNextValue(int channel)
{
	if(!chanIndexMap_.contains(channel))
		return 0;
	int index = chanIndexMap_[channel];
	int nextCurr = curr_;
	if(curr_+1 >= data_.size())
		return 0;
	return data_[curr_+1].getValue(index);
}

//Returns signal values for the input sub channel with times > the input time.
QVariantList LfpState::getValuesSince(int channel,double time)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(curr_ >= 0);
	if(!chanIndexMap_.contains(channel))
		return QVariantList();
	int chanIndex = chanIndexMap_[channel];
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QList<PlaybackLFPData>::iterator iter = qUpperBound<QList<PlaybackLFPData>::iterator,PlaybackLFPData>(data_.begin(),data_.begin()+curr_,PlaybackLFPData(afterTime+runStart_));
	QVariantList returnVal;
	for(;(iter <= data_.begin()+curr_) && (iter < data_.end());iter++)
	{
		returnVal.append(iter->getValue(chanIndex));
	}
	return returnVal;
}

QVariantList LfpState::getValuesUntil(int channel,double time)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(curr_ >= 0);
	if(!chanIndexMap_.contains(channel))
		return QVariantList();
	int chanIndex = chanIndexMap_[channel];
	double upToTime = time;
	double latestTime = getLatestTime();
	if(upToTime <= latestTime)
		return QVariantList();
	QList<PlaybackLFPData>::iterator upToIter = qUpperBound<QList<PlaybackLFPData>::iterator,PlaybackLFPData>(data_.begin()+curr_,data_.end(),PlaybackLFPData(upToTime+runStart_));
	QVariantList returnVal;
	for(QList<PlaybackLFPData>::iterator iter = data_.begin()+curr_+1;(iter < upToIter) && (iter < data_.end());iter++)
	{
		returnVal.append(iter->getValue(chanIndex));
	}
	return returnVal;
}

QVariantList LfpState::getTimesSince(double time)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(curr_ >= 0);
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QList<PlaybackLFPData>::iterator iter = qUpperBound<QList<PlaybackLFPData>::iterator,PlaybackLFPData>(data_.begin(),data_.begin()+curr_,PlaybackLFPData(afterTime+runStart_));
	QVariantList returnVal;
	for(;(iter <= data_.begin()+curr_) && (iter < data_.end());iter++)
	{
		returnVal.append(iter->time_ - runStart_);
	}
	return returnVal;
}

QVariantList LfpState::getTimesUntil(double time)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(curr_ >= 0);
	double upToTime = time;
	double latestTime = getLatestTime();
	if(upToTime <= latestTime)
		return QVariantList();
	QList<PlaybackLFPData>::iterator upToIter = qUpperBound<QList<PlaybackLFPData>::iterator,PlaybackLFPData>(data_.begin()+curr_,data_.end(),PlaybackLFPData(upToTime+runStart_));
	QVariantList returnVal;
	for(QList<PlaybackLFPData>::iterator iter = data_.begin()+curr_+1;(iter < upToIter) && (iter < data_.end());iter++)
	{
		returnVal.append(iter->time_ - runStart_);
	}
	return returnVal;
}

PlaybackIndex LfpState::getNextIndex()
{
	if(curr_ + 1 >= data_.size())
		return PlaybackIndex();
	return globalToRunIndex(PlaybackIndex::minForTime(data_[curr_+1].time_));
}

PlaybackIndex LfpState::globalToRunIndex(PlaybackIndex index)
{
	PlaybackIndex returnVal = index;
	returnVal.time() -= runStart_;
	return returnVal;
}

LfpState::PlaybackLFPData::PlaybackLFPData()
{
	time_ = -1;
	channelVals_ = NULL;
}

LfpState::PlaybackLFPData::~PlaybackLFPData()
{
	if(channelVals_) 
		delete channelVals_;
}

LfpState::PlaybackLFPData::PlaybackLFPData(double time)
{
	time_ = time;
	channelVals_ = NULL;
}
void LfpState::PlaybackLFPData::initializeValArray(int size)
{
	channelVals_ = new float[size];
	for(int i=0;i<size;i++)
		channelVals_[i] = 0;
}
//In the interest of speed/memory we're not checking that index is the right size.  Caller should check this.
void LfpState::PlaybackLFPData::setValue(int index,float value)
{	
	channelVals_[index] = value;
}
;//In the interest of speed/memory we're not checking that index is the right size.  Caller should check this.
float LfpState::PlaybackLFPData::getValue(const int& index)
{
	return channelVals_[index];
}