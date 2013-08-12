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

QSharedPointer<Asset> AnalysisFrameData::Create()
{
	return QSharedPointer<Asset>(new AnalysisFrameData());
}

void AnalysisFrameData::reset()
{
	AnalysisDataSource::reset();
	zeroLatestFrame();
}

//Sets the time of the latest frame to zero.  Other frame times will be returned with respect to
//this latest frame.
void AnalysisFrameData::zeroLatestFrame()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return;
	double latestTime = getLatestRunTime();
	zeroTime_ = latestTime;
}

//Returns the time of the latest frame to have been shown
double AnalysisFrameData::getLatestTime()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return -1;
	return getDesignConfig()->getFrameReader()->getLatestTime() - zeroTime_;
}
//Returns the time of the frame shown before the latest one
double AnalysisFrameData::getPrevTime()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return -1;
	return getDesignConfig()->getFrameReader()->getPrevTime() - zeroTime_;
}
//Returns the time of the next frame to be shown.
double AnalysisFrameData::getNextTime()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return -1;
	return getDesignConfig()->getFrameReader()->getNextTime() - zeroTime_;
}
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

double AnalysisFrameData::getLatestRunTime()
{
	double latestTime = getDesignConfig()->getFrameReader()->getLatestTime();
	if(latestTime < 0)
		latestTime = 0;
	return latestTime;
}
} //namespace Picto