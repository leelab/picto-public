#include "AnalysisFrameData.h"
#include "../memleakdetect.h"

namespace Picto
{
AnalysisFrameData::AnalysisFrameData()
: 
  AnalysisDataSource()
{
	zeroTime_ = 0;
}

/*! \brief Creates a new AnalysisFrameData object and returns a shared Asset pointer to it.
 */
QSharedPointer<Asset> AnalysisFrameData::Create()
{
	return QSharedPointer<Asset>(new AnalysisFrameData());
}

void AnalysisFrameData::enteredScope()
{
	AnalysisDataSource::enteredScope();
	zeroLatestFrame();
}

//! Returns the zero time of this element, represented in global behavioral time
double AnalysisFrameData::getZeroTime() const
{
	return zeroTime_;
}

//! Sets the time of the latest frame to zero.  Other frame times will be returned with respect to this latest frame.
void AnalysisFrameData::zeroLatestFrame()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return;
	double latestTime = getLatestRunTime();
	zeroTime_ = latestTime;
}

/*!	\brief Returns the time of the latest frame to have been shown
 */
double AnalysisFrameData::getLatestTime()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return -1;
	return getDesignConfig()->getFrameReader()->getLatestTime() - zeroTime_;
}

/*!	\brief Returns the time of the frame shown before the latest one
 */
double AnalysisFrameData::getPrevTime()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return -1;
	return getDesignConfig()->getFrameReader()->getPrevTime() - zeroTime_;
}

/*!	\brief Returns the time of the next frame to be shown.
 */
double AnalysisFrameData::getNextTime()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return -1;
	return getDesignConfig()->getFrameReader()->getNextTime() - zeroTime_;
}

/*!	\brief Returns a list of frame times that occured with times > the input # sec before the latest frame and <= the
 *	latest frame time.
 */
QVariantList AnalysisFrameData::getPrevTimes(double secsPreceding)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double minRunTime = getLatestRunTime()-secsPreceding;
	QVariantList returnVal = getDesignConfig()->getFrameReader()->getTimesSince(minRunTime);
	if(zeroTime_)
	{
		for(int i=0;i<returnVal.size();i++)
			returnVal[i] = returnVal[i].toDouble() - zeroTime_;
	}
	return returnVal;
}

/*!	\brief Returns a list of frame times that occured with times <= the input # sec after the latest frame and > the
 *	latest frame time.
 */
QVariantList AnalysisFrameData::getNextTimes(double secsFollowing)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double maxRunTime = getLatestRunTime()+secsFollowing;
	QVariantList returnVal = getDesignConfig()->getFrameReader()->getTimesUntil(maxRunTime);
	if(zeroTime_)
	{
		for(int i=0;i<returnVal.size();i++)
			returnVal[i] = returnVal[i].toDouble() - zeroTime_;
	}
	return returnVal;
}

/*!	\brief Functions like getPrevTimes except that the input time is an absolute time with respect to this element's zero
 *	time instead of an offset
 */
QVariantList AnalysisFrameData::getTimesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevTimes(offsetTime);
}

/*!	\brief Functions like getNextTimes except that the input time is an absolute time with respect to this element's zero
 *	time instead of an offset
 */
QVariantList AnalysisFrameData::getTimesUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextTimes(offsetTime);
}

void AnalysisFrameData::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool AnalysisFrameData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}

/*!	\brief Returns the latest frame time with respect to the beginning of the run.
 */
double AnalysisFrameData::getLatestRunTime()
{
	double latestTime = getDesignConfig()->getFrameReader()->getLatestTime();
	if(latestTime < 0)
		latestTime = 0;
	return latestTime;
}
} //namespace Picto