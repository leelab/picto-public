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

/*! \brief Creates a new AnalysisRewardData object and returns a shared Asset pointer to it.
*/
QSharedPointer<Asset> AnalysisRewardData::Create()
{
	return QSharedPointer<Asset>(new AnalysisRewardData());
}

void AnalysisRewardData::enteredScope()
{
	AnalysisDataSource::enteredScope();
	zeroLatestFrame();
}

/*! \brief Sets the time of the latest frame to zero.  Reward times will be returned with respect to
 *	this latest frame.
 */
void AnalysisRewardData::zeroLatestFrame()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return;
	double latestTime = getLatestRunTime();
	zeroTime_ = latestTime;
}

/*! \brief Returns the time of the latest reward to have been supplied.
 */
double AnalysisRewardData::getLatestTime()
{
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return -1;
	double result = getDesignConfig()->getRewardReader()->getLatestTime() - zeroTime_;
	return result;
}

/*! \brief Returns the time of the next reward to be supplied.
 *	\note In a live test, this will not return a valid value.
 */
double AnalysisRewardData::getNextTime()
{
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return -1;
	double result = getDesignConfig()->getRewardReader()->getNextTime() - zeroTime_;
	return result;
}

/*! \brief Returns the duration of the latest reward to have been supplied.
*/
int AnalysisRewardData::getLatestDuration()
{
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return -1;
	return getDesignConfig()->getRewardReader()->getLatestDuration();
}

/*! \brief Returns the duration of the next reward to be supplied.
 *	\note In a live test, this will not return a valid value.
 */
int AnalysisRewardData::getNextDuration()
{
	Q_ASSERT(!getDesignConfig()->getRewardReader().isNull());
	if(!getDesignConfig()->getRewardReader())
		return -1;
	return getDesignConfig()->getRewardReader()->getNextDuration();
}
/*! \brief Returns a list of reward times that occured with times > the input # sec before the latest frame and <= the latest frame time.
*/
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
/*! \brief Returns a list of reward times that occured with times <= the input # sec after the latest frame and > the latest frame time.
 *	\note In a live test, this will not return a valid value.
 */
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

/*! \brief Functions like getPrevTimes() except that the input time is an absolute time with respect to this element's zero time instead of an offset.
*/
QVariantList AnalysisRewardData::getTimesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevTimes(offsetTime);
}
/*! \brief Functions like getNextTimes() except that the input time is an absolute time with respect to this element's zero time instead of an offset.
*/
QVariantList AnalysisRewardData::getTimesUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextTimes(offsetTime);
}

/*! \brief Returns a list of reward durations that occured with times > the input # sec before the latest frame and <= the latest frame time.
*/
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

/*! \brief Returns a list of reward durations that occured with times <= the input # sec after the latest frame and > the latest frame time.
 *	\note In a live test, this will not return a valid value.
*/
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
/*! \brief Functions like getPrevDurations() except that the input time is an absolute time with respect to this element's zero time instead of an offset.
*/
QVariantList AnalysisRewardData::getDurationsSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevDurations(offsetTime);
}
/*! \brief Functions like getNextDurations() except that the input time is an absolute time with respect to this element's zero time instead of an offset.
 */
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

/*! \brief Returns the latest frame time with respect to the beginning of the run.
*/
double AnalysisRewardData::getLatestRunTime()
{
	double latestTime = getDesignConfig()->getFrameReader()->getLatestTime();
	if(latestTime < 0)
		latestTime = 0;
	return latestTime;
}
} //namespace Picto