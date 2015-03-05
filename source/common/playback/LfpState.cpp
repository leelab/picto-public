#include "LfpState.h"
#include "../storage/DataSourceInfo.h"
#include "../storage/alignmentinfo.h"
using namespace Picto;

LfpState::LfpState(bool enabled) :
enabled_(enabled)
{

}

/*! \brief Enables/Disables this LfpState to make memory / load time savings possible when lfp data is not needed.
 *	\details This function could take quite some time if LFP data wasn't loaded and it is used to enable lfp data.
 *	This is because in this case all lfp data will be loaded before the function returns.
 *
 *	lfpLoadProgress() signals are emitted as part of this function to indicate when the lfp data loading or 
 *	unloading is complete.
 *	\sa getEnabled()
 */
void LfpState::setEnabled(bool enable)
{
	if(enable)
	{
		if(!enabled_ && session_.isValid())
		{
			if(!loadData())
			{
				emit lfpLoadProgress(100);
				resetDataVariables();
				return;
			}
		}
	}
	else
	{
		if(enabled_)
		{
			resetDataVariables();
		}
	}
	enabled_ = enable;
	emit lfpLoadProgress(100);
}

/*! \brief Returns true if this LFPState is currently set to return lfp data, false otherwise.
 *	\sa setEnabled()
 */
bool LfpState::getEnabled()
{
	return enabled_;
}

void LfpState::setDatabase(QSqlDatabase session)
{
	runStart_ = runEnd_ = -1;
	session_ = session;
	resetDataVariables();

	if(!enabled_)
		return;

	if(!loadData())
		resetDataVariables();
	emit lfpLoadProgress(100);
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
	return globalToRunIndex(PlaybackIndex::minForTime(globalTimeFromArrayIndex(curr_)));
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
			emit lfpChanged(iter.key(),getDataValue(curr_,iter.value()));
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
	return getDataValue(curr_,index);
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
	if(curr_+1 >= numValues_)
		return 0;
	return getDataValue(curr_+1,index);
}

//Returns signal values for the input sub channel with times > the input time.
QVariantList LfpState::getValuesSince(int channel,double time)
{
	Q_ASSERT(runStart_ >= 0);
	if(!chanIndexMap_.contains(channel))
		return QVariantList();
	int chanIndex = chanIndexMap_[channel];
	if(!data_.size() || !data_[chanIndex].size())
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	int startPos = arrayIndexFromGlobalTime(afterTime+runStart_);
	if(globalTimeFromArrayIndex(startPos) <= afterTime+runStart_)
		startPos++;
	QVariantList returnVal;
	for(int pos = startPos;pos <= curr_;pos++)
	{
		returnVal.append(getDataValue(pos,chanIndex));
	}
	return returnVal;
}

QVariantList LfpState::getValuesUntil(int channel,double time)
{
	Q_ASSERT(runStart_ >= 0);
	if(!chanIndexMap_.contains(channel))
		return QVariantList();
	int chanIndex = chanIndexMap_[channel];
	if(!data_.size() || !data_[chanIndex].size())
		return QVariantList();
	double upToTime = time;
	double latestTime = getLatestTime();
	if(upToTime <= latestTime)
		return QVariantList();
	int endPos = arrayIndexFromGlobalTime(upToTime+runStart_);
	if(globalTimeFromArrayIndex(endPos) > upToTime+runStart_)
		endPos--;
	QVariantList returnVal;
	for(int pos = curr_+1;(pos <= endPos) && (pos < numValues_);pos++)
	{
		returnVal.append(getDataValue(pos,chanIndex));
	}
	return returnVal;
}

QVariantList LfpState::getTimesSince(double time)
{
	Q_ASSERT(runStart_ >= 0);
	if(!data_.size())
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	int startPos = arrayIndexFromGlobalTime(afterTime+runStart_);
	if(globalTimeFromArrayIndex(startPos) <= afterTime+runStart_)
		startPos++;
	QVariantList returnVal;
	for(int pos = startPos;pos <= curr_;pos++)
	{
		returnVal.append(globalTimeFromArrayIndex(pos) - runStart_);
	}
	return returnVal;
}

QVariantList LfpState::getTimesUntil(double time)
{
	Q_ASSERT(runStart_ >= 0);
	if(!data_.size())
		return QVariantList();
	double upToTime = time;
	double latestTime = getLatestTime();
	if(upToTime <= latestTime)
		return QVariantList();
	int endPos = arrayIndexFromGlobalTime(upToTime+runStart_);
	if(globalTimeFromArrayIndex(endPos) > upToTime+runStart_)
		endPos--;
	QVariantList returnVal;
	for(int pos = curr_+1;(pos <= endPos) && (pos < numValues_);pos++)
	{
		returnVal.append(globalTimeFromArrayIndex(pos) - runStart_);
	}
	return returnVal;
}

bool LfpState::loadData()
{
	resetDataVariables();
	emit lfpLoadProgress(0);
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(session_));

	//Get timestamp alignment constants
	query_->prepare("SELECT value FROM sessioninfo WHERE key=\"AlignmentInfo\"");
	if(!query_->exec() || !query_->next())
	{
		return true;
	}
	AlignmentInfo inf;
	inf.fromXml(query_->value(0).toString());
	double offsetTime = inf.getOffsetTime();
	double temporalFactor = inf.getTemporalFactor();


	//Set up lfp hash
	query_->prepare("SELECT DISTINCT channel FROM lfp ORDER BY channel");
	if(!query_->exec() || !query_->next())
	{
		return true;
	}
	int index = 0;
	do
	{
		chanIndexMap_[query_->value(0).toInt()] = index++;
		channels_.append(query_->value(0).toInt());
	} while(query_->next());
	numChannels_ = channels_.size();

	//Get lfp resolution
	sampPeriod_ = 0;
	query_->prepare("SELECT value FROM sessioninfo WHERE key='DataSource'");
	if(!query_->exec())
	{
		return false;
	}
	while(query_->next())
	{
		DataSourceInfo info;
		info.fromXml(query_->value(0).toString());
		if(info.getName() == "lfp")
		{
			sampPeriod_ = temporalFactor*info.getResolution();
			break;
		}
	}
	if(sampPeriod_ <= 0)
	{
		return false;
	}

	//Get minimum lfp time
	query_->prepare("SELECT timestamp from lfp ORDER BY timestamp LIMIT 1");
	if(!query_->exec() || !query_->next())
	{
		return true;
	}
	minLfpTime_ = offsetTime + temporalFactor*query_->value(0).toDouble();

	//Get maximum lfp time
	//Start by getting highest timestamp entries for all channels.  We could use distinct to make sure that we're
	//actually getting the highest value from each channel.  In practice; however, the DISTINCT keyword makes
	//things take a lot longer in this call and unless the neural proxy is doing something really weird if we
	//just select the last numberOfChannels timestamp entries whether or not the channels are distinct, we will get 
	//the highest time value in the experiment or certainly it will be close enough to the end of the experiment as
	//to be past the end of the last run.  The worst case here is that we miss one lfp data packet time length at 
	//the end of the session.  Currently these packets are 500ms long and as mentioned before, this worst case will
	//really never come up unless something really wierd happens with the proxy.
	query_->prepare("SELECT timestamp,data FROM lfp ORDER BY timestamp DESC LIMIT :channels");
	query_->bindValue(":channels",numChannels_);
	if(!query_->exec())
	{
		return false;
	}
	double maxTime = minLfpTime_-1;
	while(query_->next())
	{
		double currHighest = offsetTime+(temporalFactor*query_->value(0).toDouble()) + (sampPeriod_ * query_->value(1).toByteArray().size()/sizeof(float));
		if(currHighest > maxTime)
			maxTime = currHighest;
	}
	Q_ASSERT(maxTime >= minLfpTime_);

	emit lfpLoadProgress(10);

	//Create data list with an entry for every sample time from minTime to maxTime
	numValues_ = arrayIndexFromGlobalTime(maxTime)+1;
	try{
		data_.reserve(numChannels_);
		for(int i=0;i<numChannels_;i++)
		{
			data_.append(QList<float>());
			data_[i].reserve(numValues_);				//We use reserve and not resize so that we can be sure that we're allocating exactly the right amount of memory
													//tests indicate that resize actually may reserve more than the requested amount
		}
	} catch(...)
	{
		resetDataVariables();
		throw;
	}
	for(int i=0;i<numChannels_;i++)
	{
		QList<float> chanList;
		for(int j=0;j<numValues_;j++)
			chanList.append(0);
		data_[i]= chanList;
	}

	emit lfpLoadProgress(20);
	//Go through each channel in database and fill data list
	int channelNum = 0;
	foreach(int channel,chanIndexMap_.keys())
	{
		channelNum++;
		int valueIndex = chanIndexMap_[channel];
		//Get data and load it into this channel's list from the lfp hash
		query_->prepare("SELECT timestamp,data FROM lfp WHERE channel=:channel "
		"ORDER BY timestamp");
		query_->bindValue(":channel",channel);
		if(!query_->exec())
		{
			Q_ASSERT(false);
			return false;
		}
		int catchUpIndex = 0;
		double startTime;
		double currTime;
		float latestValue = 0;
		int indexForCurrTime = -1;
		while(query_->next())
		{
			startTime = offsetTime + temporalFactor*query_->value(0).toDouble();
			QByteArray dataArray = query_->value(1).toByteArray();
			const float* dataArrayStart = reinterpret_cast<const float*>(dataArray.constData());
			int dataArraySize = dataArray.size()/sizeof(float);
			for(int i=0;i<dataArraySize;i++)
			{
				currTime = startTime + i*sampPeriod_;
				indexForCurrTime = arrayIndexFromGlobalTime(currTime);
				if(indexForCurrTime >= numValues_)
					break;	//Since we sped things up above by not necessarily checking all channels before making a highest time decision
							//there is a very unlikely possiblity that there is more data available than will fit in the vector we made
							//if this is the case we just throw the data out because in any case we will be very close to the end of the
							//session and the data will almost certainly come from after the significant runs were finished.
				while(catchUpIndex <= arrayIndexFromGlobalTime(currTime))
				{
					setDataValue(catchUpIndex,valueIndex,latestValue);
					catchUpIndex++;
				}
				latestValue = *(dataArrayStart + i);
				setDataValue(arrayIndexFromGlobalTime(currTime),valueIndex,latestValue);
			}
		}
		emit lfpLoadProgress(20+(80.0*double(channelNum)/double(chanIndexMap_.keys().size())));
	}
	return true;
}

void LfpState::resetDataVariables()
{
	chanIndexMap_.clear();
	channels_.clear();
	numChannels_ = 0;
	sampPeriod_ = 0;
	minLfpTime_ = 0;
	data_.clear();
	curr_ = -1;
}

PlaybackIndex LfpState::getNextIndex()
{
	if(curr_ + 1 >= numValues_)
		return PlaybackIndex();
	return globalToRunIndex(PlaybackIndex::minForTime(globalTimeFromArrayIndex(curr_+1)));
}

PlaybackIndex LfpState::globalToRunIndex(PlaybackIndex index)
{
	PlaybackIndex returnVal = index;
	returnVal.time() -= runStart_;
	return returnVal;
}

double LfpState::globalTimeFromArrayIndex(const int& index)
{
	return minLfpTime_ + (sampPeriod_ * double(index));
}

int LfpState::arrayIndexFromGlobalTime(const double& time)
{
	int returnVal = int(0.5+((time-minLfpTime_)/sampPeriod_)); //We add the 0.5 so that we're rounding to the nearest value and not rounding down
	if(returnVal < 0)
		return 0;
	return returnVal;
}

float LfpState::getDataValue(const int& arrayIndex,const int& channelIndex)
{
	Q_ASSERT(channelIndex < data_.size());
	Q_ASSERT(arrayIndex < data_[channelIndex].size());
	return data_[channelIndex][arrayIndex];
}

void LfpState::setDataValue(const int& arrayIndex,const int& channelIndex,const float& value)
{
	Q_ASSERT(channelIndex < data_.size());
	Q_ASSERT(arrayIndex < data_[channelIndex].size());
	data_[channelIndex][arrayIndex] = value;
}