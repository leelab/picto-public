#include "LiveRewardReader.h"
using namespace Picto;

void LiveRewardReader::setRunStart()
{
	rewardData_.clear();
}

void LiveRewardReader::setLatestRewardData(double time,double duration)
{
	rewardData_.append(RewardData(time,duration));
}

void LiveRewardReader::setRunEnd()
{
	rewardData_.clear();
}

double LiveRewardReader::getLatestTime()
{
	if(!rewardData_.size())
		return 0;
	return rewardData_[rewardData_.size()-1].time_;
}
double LiveRewardReader::getNextTime()
{
	return -1;
}
int LiveRewardReader::getLatestDuration()
{
	if(!rewardData_.size())
		return 0;
	return rewardData_[rewardData_.size()-1].duration_;
}
int LiveRewardReader::getNextDuration()
{
	return -1;
}
QVariantList LiveRewardReader::getTimesSince(double time)
{
	if(!rewardData_.size())
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QVector<RewardData>::iterator iter = qUpperBound<QVector<RewardData>::iterator,RewardData>(rewardData_.begin(),rewardData_.end(),RewardData(time,0));
	QVariantList returnVal;
	for(;iter < rewardData_.end();iter++)
	{
		returnVal.append(iter->time_);
	}
	return returnVal;
}

QVariantList LiveRewardReader::getTimesUntil(double time)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}

QVariantList LiveRewardReader::getDurationsSince(double time)
{
	if(!rewardData_.size())
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QVector<RewardData>::iterator iter = qUpperBound<QVector<RewardData>::iterator,RewardData>(rewardData_.begin(),rewardData_.end(),RewardData(time,0));
	QVariantList returnVal;
	for(;iter < rewardData_.end();iter++)
	{
		returnVal.append(iter->duration_);
	}
	return returnVal;
}

QVariantList LiveRewardReader::getDurationsUntil(double time)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}