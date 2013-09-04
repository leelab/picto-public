#include <QMap>
#include "SpikeState.h"
#include "PlaybackData.h"
using namespace Picto;


void SpikeState::setDatabase(QSqlDatabase session)
{
	runStart_ = runEnd_ = curr_ = -1;
	session_ = session;
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(session_));
	query_->exec("SELECT COUNT(*) FROM spikes");
	if(!query_->exec() || !query_->next())
	{
		Q_ASSERT(false);
		return;
	}
	data_.clear();
	data_.resize(query_->value(0).toInt());

	query_->exec("SELECT s.timestamp,s.channel,s.unit,s.waveform FROM spikes s "
		"ORDER BY s.timestamp");
	if(!query_->exec())
	{
		Q_ASSERT(false);
		return;
	}

	int arrayIndex = 0;
	QVector<float> waveform;
	QByteArray waveformBA;
	int channel;
	int unit;
	QMap<int,QMap<int,bool>> channelUnitsLookup;
	while(query_->next())
	{
		waveformBA = query_->value(3).toByteArray();
		waveform.clear();
		const float* pots = reinterpret_cast<const float*>(waveformBA.constData());
		for(unsigned int i=0;i<waveformBA.size()/sizeof(float);i++)
		{
			waveform.append(pots[i]);
		}
		channel = query_->value(1).toInt();
		unit = query_->value(2).toInt();
		data_[arrayIndex++] = PlaybackSpikeData(query_->value(0).toDouble(),channel,unit,waveform);
		channelUnitsLookup[channel][unit] = true;
	}
	foreach(int chan,channelUnitsLookup.keys())
	{
		channels_.append(chan);
		foreach(int un,channelUnitsLookup[chan].keys())
		{
			unitsLookup_[chan].append(un);
		}
	}
	Q_ASSERT(data_.size() == arrayIndex);
}

void SpikeState::startRun(double runStartTime,double runEndTime)
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

PlaybackIndex SpikeState::getCurrentIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0 || curr_ >= data_.size())
		return PlaybackIndex();
	return globalTimeToRunIndex(data_[curr_].time_);
}

PlaybackIndex SpikeState::getNextIndex(double lookForwardTime)
{
	PlaybackIndex returnVal = getNextIndex();
	if(returnVal.time() > lookForwardTime)
		return PlaybackIndex();
	return returnVal;
}

void SpikeState::moveToIndex(PlaybackIndex index)
{
	Q_ASSERT(runStart_ >= 0);
	Q_ASSERT(index >= getCurrentIndex());
	PlaybackIndex nextIndex = getNextIndex();
	while(nextIndex.isValid() && nextIndex <= index)
	{
		curr_++;
		emit spikeEvent(globalTimeToRunIndex(data_[curr_].time_).time(),data_[curr_].channel_,data_[curr_].unit_,data_[curr_].waveform_);
		nextIndex = getNextIndex();
	}
}
QVariantList SpikeState::getChannels()
{
	return channels_;
}
QVariantList SpikeState::getUnits(int channel)
{
	if(!unitsLookup_.contains(channel))
		return QVariantList();
	return unitsLookup_.value(channel);
}
double SpikeState::getLatestTime()
{
	return getCurrentIndex().time();
}
int SpikeState::getLatestChannel()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0 || curr_ >= data_.size())
		return -1;
	return data_[curr_].channel_;
}
int SpikeState::getLatestUnit()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0 || curr_ >= data_.size())
		return -1;
	return data_[curr_].unit_;
}
QVariantList SpikeState::getLatestWaveform()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0 || curr_ >= data_.size())
		return QVariantList();
	QVariantList returnVal;
	foreach(float val,data_[curr_].waveform_)
	{
		returnVal.append(val);
	}
	return returnVal;
}
double SpikeState::getNextTime()
{
	return getNextIndex().time();
}
int SpikeState::getNextChannel()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0 || curr_ >= data_.size()-1)
		return -1;
	return data_[curr_+1].channel_;
}
int SpikeState::getNextUnit()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0 || curr_ >= data_.size()-1)
		return -1;
	return data_[curr_+1].unit_;
}
QVariantList SpikeState::getNextWaveform()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ < 0 || curr_ >= data_.size()-1)
		return QVariantList();
	QVariantList returnVal;
	foreach(float val,data_[curr_+1].waveform_)
	{
		returnVal.append(val);
	}
	return returnVal;
}
QVariantList SpikeState::getTimesSince(double time)
{
	Q_ASSERT(runStart_ >= 0);
	double afterTime = time;
	if(afterTime >= getLatestTime())
		return QVariantList();
	PlaybackIndex beyondIndex = PlaybackIndex::maxForTime(afterTime+runStart_);
	QVector<PlaybackSpikeData>::iterator iter = qUpperBound<QVector<PlaybackSpikeData>::iterator,PlaybackSpikeData>(data_.begin(),data_.begin()+curr_,PlaybackSpikeData(beyondIndex.time(),0,0,QVector<float>()));
	QVariantList returnVal;
	for(;(iter <= data_.begin()+curr_) && (iter < data_.end());iter++)
	{
		returnVal.append(globalTimeToRunIndex(iter->time_).time());
	}
	return returnVal;
}
QVariantList SpikeState::getTimesUntil(double time)
{
	Q_ASSERT(runStart_ >= 0);
	double upToTime = time;
	if(upToTime <= getLatestTime())
		return QVariantList();
	PlaybackIndex beforeIndex = PlaybackIndex::maxForTime(upToTime+runStart_);
	QVector<PlaybackSpikeData>::iterator upToIter = qUpperBound<QVector<PlaybackSpikeData>::iterator,PlaybackSpikeData>(data_.begin()+curr_,data_.end(),PlaybackSpikeData(beforeIndex.time(),0,0,QVector<float>()));
	QVariantList returnVal;
	for(QVector<PlaybackSpikeData>::iterator iter = data_.begin()+curr_+1;(iter < upToIter) && (iter < data_.end());iter++)
	{
		returnVal.append(globalTimeToRunIndex(iter->time_).time());
	}
	return returnVal;
}
QVariantList SpikeState::getChannelsSince(double time)
{
	Q_ASSERT(runStart_ >= 0);
	double afterTime = time;
	if(afterTime >= getLatestTime())
		return QVariantList();
	PlaybackIndex beyondIndex = PlaybackIndex::maxForTime(afterTime+runStart_);
	QVector<PlaybackSpikeData>::iterator iter = qUpperBound<QVector<PlaybackSpikeData>::iterator,PlaybackSpikeData>(data_.begin(),data_.begin()+curr_,PlaybackSpikeData(beyondIndex.time(),0,0,QVector<float>()));
	QVariantList returnVal;
	for(;(iter <= data_.begin()+curr_) && (iter < data_.end());iter++)
	{
		returnVal.append(iter->channel_);
	}
	return returnVal;
}
QVariantList SpikeState::getChannelsUntil(double time)
{
	Q_ASSERT(runStart_ >= 0);
	double upToTime = time;
	if(upToTime <= getLatestTime())
		return QVariantList();
	PlaybackIndex beforeIndex = PlaybackIndex::maxForTime(upToTime+runStart_);
	QVector<PlaybackSpikeData>::iterator upToIter = qUpperBound<QVector<PlaybackSpikeData>::iterator,PlaybackSpikeData>(data_.begin()+curr_,data_.end(),PlaybackSpikeData(beforeIndex.time(),0,0,QVector<float>()));
	QVariantList returnVal;
	for(QVector<PlaybackSpikeData>::iterator iter = data_.begin()+curr_+1;(iter < upToIter) && (iter < data_.end());iter++)
	{
		returnVal.append(iter->channel_);
	}
	return returnVal;
}
QVariantList SpikeState::getUnitsSince(double time)
{
	Q_ASSERT(runStart_ >= 0);
	double afterTime = time;
	if(afterTime >= getLatestTime())
		return QVariantList();
	PlaybackIndex beyondIndex = PlaybackIndex::maxForTime(afterTime+runStart_);
	QVector<PlaybackSpikeData>::iterator iter = qUpperBound<QVector<PlaybackSpikeData>::iterator,PlaybackSpikeData>(data_.begin(),data_.begin()+curr_,PlaybackSpikeData(beyondIndex.time(),0,0,QVector<float>()));
	QVariantList returnVal;
	for(;(iter <= data_.begin()+curr_) && (iter < data_.end());iter++)
	{
		returnVal.append(iter->unit_);
	}
	return returnVal;
}
QVariantList SpikeState::getUnitsUntil(double time)
{
	Q_ASSERT(runStart_ >= 0);
	double upToTime = time;
	if(upToTime <= getLatestTime())
		return QVariantList();
	PlaybackIndex beforeIndex = PlaybackIndex::maxForTime(upToTime+runStart_);
	QVector<PlaybackSpikeData>::iterator upToIter = qUpperBound<QVector<PlaybackSpikeData>::iterator,PlaybackSpikeData>(data_.begin()+curr_,data_.end(),PlaybackSpikeData(beforeIndex.time(),0,0,QVector<float>()));
	QVariantList returnVal;
	for(QVector<PlaybackSpikeData>::iterator iter = data_.begin()+curr_+1;(iter < upToIter) && (iter < data_.end());iter++)
	{
		returnVal.append(iter->unit_);
	}
	return returnVal;
}
QVariantList SpikeState::getWaveformsSince(double time)
{
	Q_ASSERT(runStart_ >= 0);
	double afterTime = time;
	if(afterTime >= getLatestTime())
		return QVariantList();
	PlaybackIndex beyondIndex = PlaybackIndex::maxForTime(afterTime+runStart_);
	QVector<PlaybackSpikeData>::iterator iter = qUpperBound<QVector<PlaybackSpikeData>::iterator,PlaybackSpikeData>(data_.begin(),data_.begin()+curr_,PlaybackSpikeData(beyondIndex.time(),0,0,QVector<float>()));
	QVariantList returnVal;
	QVariantList waveformList;
	for(;(iter <= data_.begin()+curr_) && (iter < data_.end());iter++)
	{
		waveformList.clear();
		foreach(float val,iter->waveform_)
		{
			waveformList.append(val);
		}
		returnVal.append(QVariant(waveformList));
	}
	return returnVal;
}
QVariantList SpikeState::getWaveformsUntil(double time)
{
	Q_ASSERT(runStart_ >= 0);
	double upToTime = time;
	if(upToTime <= getLatestTime())
		return QVariantList();
	PlaybackIndex beforeIndex = PlaybackIndex::maxForTime(upToTime+runStart_);
	QVector<PlaybackSpikeData>::iterator upToIter = qUpperBound<QVector<PlaybackSpikeData>::iterator,PlaybackSpikeData>(data_.begin()+curr_,data_.end(),PlaybackSpikeData(beforeIndex.time(),0,0,QVector<float>()));
	QVariantList returnVal;
	QVariantList waveformList;
	for(QVector<PlaybackSpikeData>::iterator iter = data_.begin()+curr_+1;(iter < upToIter) && (iter < data_.end());iter++)
	{
		waveformList.clear();
		foreach(float val,iter->waveform_)
		{
			waveformList.append(val);
		}
		returnVal.append(QVariant(waveformList));
	}
	return returnVal;
}
PlaybackIndex SpikeState::getNextIndex()
{
	Q_ASSERT(runStart_ >= 0);
	if(curr_ >= data_.size()-1)
		return PlaybackIndex();
	return globalTimeToRunIndex(data_[curr_+1].time_);
}

PlaybackIndex SpikeState::globalTimeToRunIndex(double time)
{
	PlaybackIndex returnVal = PlaybackIndex::minForTime(time);
	returnVal.time() -= runStart_;
	return returnVal;
}