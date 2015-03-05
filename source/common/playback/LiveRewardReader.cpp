#include "LiveRewardReader.h"
using namespace Picto;

/*! \brief Called when a new Run Starts to let this object know that it should clear out its reward list and start again.
*/
void LiveRewardReader::setRunStart()
{
	rewardData_.clear();
}

/*! \brief Called to add a new reward event to this object so that it can 
*	be included in future data requests.
*	\details time is the time that the reward occured, duration is the duration of the
*	reward in milliseconds.
*/
void LiveRewardReader::setLatestRewardData(double time,double duration)
{
	rewardData_.append(RewardData(time,duration));
}

/*! \brief Called when a Run Ends to let this object know that it can clear out its reward list.
*/
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

QVariantList LiveRewardReader::getTimesUntil(double)
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

QVariantList LiveRewardReader::getDurationsUntil(double)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}