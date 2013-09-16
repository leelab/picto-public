#include "AnalysisRewardData.h"
#include "../memleakdetect.h"

namespace Picto
{
AnalysisRewardData::AnalysisRewardData()
: 
  AnalysisDataSource()
{
	zeroTime_ = 0;
}

QSharedPointer<Asset> AnalysisRewardData::Create()
{
	return QSharedPointer<Asset>(new AnalysisRewardData());
}

void AnalysisRewardData::reset()
{
	AnalysisDataSource::reset();
	zeroLatestFrame();
}

//Sets the time of the latest frame to zero.  Reward times will be returned with respect to
//this latest frame.
void AnalysisRewardData::zeroLatestFrame()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return;
	double latestTime = getLatestRunTime();
	zeroTime_ = latestTime;
}

//Returns the time of the latest reward to have been supplied
double AnalysisRewardData::getLatestTime()
{
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return -1;
	double result = getDesignConfig()->getRewardReader()->getLatestTime() - zeroTime_;
	return result;
}

//Returns the time of the next reward to be supplied.
double AnalysisRewardData::getNextTime()
{
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return -1;
	double result = getDesignConfig()->getRewardReader()->getNextTime() - zeroTime_;
	return result;
}

//Returns the duration of the latest reward to have been supplied
int AnalysisRewardData::getLatestDuration()
{
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return -1;
	return getDesignConfig()->getRewardReader()->getLatestDuration();
}

//Returns the duration of the next reward to be supplied.
int AnalysisRewardData::getNextDuration()
{
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return -1;
	return getDesignConfig()->getRewardReader()->getNextDuration();
}

QVariantList AnalysisRewardData::getPrevTimes(double secsPreceding)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double minRunTime = getLatestRunTime()-secsPreceding;
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getRewardReader()->getTimesSince(minRunTime);
	if(zeroTime_)
	{
		for(int i=0;i<returnVal.size();i++)
			returnVal[i] = returnVal[i].toDouble() - zeroTime_;
	}
	return returnVal;
}

QVariantList AnalysisRewardData::getNextTimes(double secsFollowing)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double maxRunTime = getLatestRunTime()+secsFollowing;
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getRewardReader()->getTimesUntil(maxRunTime);
	if(zeroTime_)
	{
		for(int i=0;i<returnVal.size();i++)
			returnVal[i] = returnVal[i].toDouble() - zeroTime_;
	}
	return returnVal;
}

QVariantList AnalysisRewardData::getTimesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevTimes(offsetTime);
}

QVariantList AnalysisRewardData::getTimesUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextTimes(offsetTime);
}

//Returns a list of reward durations that occured with times > the input # sec before the latest frame and <= the latest frame time.
QVariantList AnalysisRewardData::getPrevDurations(double secsPreceding)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double minRunTime = getLatestRunTime()-secsPreceding;
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getRewardReader()->getDurationsSince(minRunTime);
	return returnVal;
}

//Returns a list of reward durations that occured with times <= the input # sec after the latest frame and > the latest frame time.
QVariantList AnalysisRewardData::getNextDurations(double secsFollowing)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double maxRunTime = getLatestRunTime()+secsFollowing;
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getRewardReader()->getDurationsUntil(maxRunTime);
	return returnVal;
}

QVariantList AnalysisRewardData::getDurationsSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevDurations(offsetTime);
}
QVariantList AnalysisRewardData::getDurationsUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextDurations(offsetTime);
}

void AnalysisRewardData::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool AnalysisRewardData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}

double AnalysisRewardData::getLatestRunTime()
{
	double latestTime = getDesignConfig()->getFrameReader()->getLatestTime();
	if(latestTime < 0)
		latestTime = 0;
	return latestTime;
}
} //namespace Picto