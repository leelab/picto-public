#include "AnalysisSpikeData.h"
#include "../memleakdetect.h"

namespace Picto
{
AnalysisSpikeData::AnalysisSpikeData()
: 
  AnalysisDataSource()
{
	zeroTime_ = 0;
}

QSharedPointer<Asset> AnalysisSpikeData::Create()
{
	return QSharedPointer<Asset>(new AnalysisSpikeData());
}

void AnalysisSpikeData::reset()
{
	AnalysisDataSource::reset();
	zeroLatestFrame();
}

void AnalysisSpikeData::zeroLatestFrame()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return;
	double latestTime = getLatestRunTime();
	zeroTime_ = latestTime;
}

double AnalysisSpikeData::getLatestTime()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	double result = getDesignConfig()->getSpikeReader()->getLatestTime() - zeroTime_;
	return result;
}

int AnalysisSpikeData::getLatestChannel()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	int result = getDesignConfig()->getSpikeReader()->getLatestChannel();
	return result;
}

int AnalysisSpikeData::getLatestUnit()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	int result = getDesignConfig()->getSpikeReader()->getLatestUnit();
	return result;
}

QVariantList AnalysisSpikeData::getLatestWaveform()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList result = getDesignConfig()->getSpikeReader()->getLatestWaveform();
	return result;
}

//Returns the time of the next spike that will be read.
double AnalysisSpikeData::getNextTime()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	double result = getDesignConfig()->getSpikeReader()->getNextTime() - zeroTime_;
	return result;
}

int AnalysisSpikeData::getNextChannel()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	int result = getDesignConfig()->getSpikeReader()->getNextChannel();
	return result;
}

int AnalysisSpikeData::getNextUnit()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	int result = getDesignConfig()->getSpikeReader()->getNextUnit();
	return result;
}

QVariantList AnalysisSpikeData::getNextWaveform()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList result = getDesignConfig()->getSpikeReader()->getNextWaveform();
	return result;
}

QVariantList AnalysisSpikeData::getPrevTimes(double secsPreceding)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double minRunTime = getLatestRunTime()-secsPreceding;
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getSpikeReader()->getTimesSince(minRunTime);
	if(zeroTime_)
	{
		for(int i=0;i<returnVal.size();i++)
			returnVal[i] = returnVal[i].toDouble() - zeroTime_;
	}
	return returnVal;
}

QVariantList AnalysisSpikeData::getPrevChannels(double secsPreceding)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double minRunTime = getLatestRunTime()-secsPreceding;
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getSpikeReader()->getChannelsSince(minRunTime);
	return returnVal;
}

QVariantList AnalysisSpikeData::getPrevUnits(double secsPreceding)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double minRunTime = getLatestRunTime()-secsPreceding;
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getSpikeReader()->getUnitsSince(minRunTime);
	return returnVal;
}

QVariantList AnalysisSpikeData::getPrevWaveforms(double secsPreceding)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double minRunTime = getLatestRunTime()-secsPreceding;
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getSpikeReader()->getWaveformsSince(minRunTime);
	return returnVal;
}

QVariantList AnalysisSpikeData::getNextTimes(double secsFollowing)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double maxRunTime = getLatestRunTime()+secsFollowing;
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getSpikeReader()->getTimesUntil(maxRunTime);
	if(zeroTime_)
	{
		for(int i=0;i<returnVal.size();i++)
			returnVal[i] = returnVal[i].toDouble() - zeroTime_;
	}
	return returnVal;
}

QVariantList AnalysisSpikeData::getNextChannels(double secsFollowing)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double maxRunTime = getLatestRunTime()+secsFollowing;
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getSpikeReader()->getChannelsUntil(maxRunTime);
	return returnVal;
}

QVariantList AnalysisSpikeData::getNextUnits(double secsFollowing)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double maxRunTime = getLatestRunTime()+secsFollowing;
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getSpikeReader()->getUnitsUntil(maxRunTime);
	return returnVal;
}

QVariantList AnalysisSpikeData::getNextWaveforms(double secsFollowing)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double maxRunTime = getLatestRunTime()+secsFollowing;
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList returnVal = getDesignConfig()->getSpikeReader()->getWaveformsUntil(maxRunTime);
	return returnVal;
}

void AnalysisSpikeData::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool AnalysisSpikeData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}

double AnalysisSpikeData::getLatestRunTime()
{
	double latestTime = getDesignConfig()->getFrameReader()->getLatestTime();
	if(latestTime < 0)
		latestTime = 0;
	return latestTime;
}
} //namespace Picto