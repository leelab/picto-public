#include "AnalysisLfpData.h"
#include "../memleakdetect.h"

namespace Picto
{
AnalysisLfpData::AnalysisLfpData()
: 
  AnalysisDataSource()
{
	zeroTime_ = 0;
}

/*! \brief Creates a new AnalysisLfpData object and returns a shared Asset pointer to it.
 */
QSharedPointer<Asset> AnalysisLfpData::Create()
{
	return QSharedPointer<Asset>(new AnalysisLfpData());
}

void AnalysisLfpData::enteredScope()
{
	AnalysisDataSource::enteredScope();
	zeroLatestFrame();
	lfpReader_ = getDesignConfig()->getLfpReader();
}

/*! \brief Sets the time of the latest frame to zero.  LFP times will be returned with respect to this latest frame time.
 */
void AnalysisLfpData::zeroLatestFrame()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return;
	double latestTime = getLatestRunTime();
	zeroTime_ = latestTime;
}

//! Returns the zero time of this element, represented in global behavioral time
double AnalysisLfpData::getZeroTime() const
{
	return zeroTime_;
}

/*! \brief Returns a list of the channels on which lfp data was captured during this run.
 */
QVariantList AnalysisLfpData::getChannels()
{
	if(!lfpReader_)
		return QVariantList();
	return lfpReader_->getChannels();
}

/*! \brief Returns the sample period for lfp data capture.
 */
double AnalysisLfpData::getSamplePeriod()
{
	if(!lfpReader_)
		return 0;
	return lfpReader_->getSamplePeriod();
}

/*! \brief Returns the time of the latest lfp data sample.
 */
double AnalysisLfpData::getLatestTime()
{
	if(!lfpReader_)
		return -1;
	double result = lfpReader_->getLatestTime() - zeroTime_;
	return result;
}

/*! \brief Returns the latest lfp value on the channel.
 */
double AnalysisLfpData::getLatestValue(int channel)
{
	if(!lfpReader_)
		return 0;
	return lfpReader_->getLatestValue(channel);
}

/*! \brief Returns an list of the latest values ordered according to the channel order returned from getChannels().
 */
QVariantList AnalysisLfpData::getLatestValue()
{
	if(!lfpReader_)
		return QVariantList();
	QVariantList channels = lfpReader_->getChannels();
	QVariantList result;
	foreach(QVariant channel,channels)
	{
		result.append(getLatestValue(channel.toInt()));
	}
	return result;
}

/*! \brief Returns the time at which the next signal will be read.
 *	\note In a live test, this will not return a valid value.
 */
double AnalysisLfpData::getNextTime()
{
	if(!lfpReader_)
		return -1;
	double result = lfpReader_->getNextTime() - zeroTime_;
	return result;
}

/*! \brief Returns the next value that will be read for the input channel.
 *	\note In a live test, this will not return a valid value.
 */
double AnalysisLfpData::getNextValue(int channel)
{
	if(!lfpReader_)
		return 0;
	return lfpReader_->getNextValue(channel);
}

/*! \brief Returns a list of the next values to be read ordered according to the channel order returned from getChannels().
 *	\note In a live test, this will not return valid values.
 */
QVariantList AnalysisLfpData::getNextValue()
{
	if(!lfpReader_)
		return QVariantList();
	QVariantList channels = lfpReader_->getChannels();
	QVariantList result;
	foreach(QVariant channel,channels)
	{
		result.append(getNextValue(channel.toInt()));
	}
	return result;
}

/*! \brief Returns a list of lfp sample times that occured with times > the input # sec before the latest frame and <= the
 *	latest frame time.
 */
QVariantList AnalysisLfpData::getPrevTimes(double secsPreceding)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double minRunTime = getLatestRunTime()-secsPreceding;
	Q_ASSERT(lfpReader_);
	if(!lfpReader_)
		return QVariantList();
	QVariantList returnVal = lfpReader_->getTimesSince(minRunTime);
	if(zeroTime_)
	{
		for(int i=0;i<returnVal.size();i++)
			returnVal[i] = returnVal[i].toDouble() - zeroTime_;
	}
	return returnVal;
}

/*! \brief Returns a list of lfp sample times that will occur with times > the latest frame time and <= the input # sec
 *	after the latest frame.
 *	\note In a live test, this will not return valid values.
 */
QVariantList AnalysisLfpData::getNextTimes(double secsFollowing)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double maxRunTime = getLatestRunTime()+secsFollowing;
	Q_ASSERT(lfpReader_);
	if(!lfpReader_)
		return QVariantList();
	QVariantList returnVal = lfpReader_->getTimesUntil(maxRunTime);
	if(zeroTime_)
	{
		for(int i=0;i<returnVal.size();i++)
			returnVal[i] = returnVal[i].toDouble() - zeroTime_;
	}
	return returnVal;
}

/*! \brief Functions like getPrevTimes() except that the input time is an absolute time with respect to this element's
 *	zero time instead of an offset.
 */
QVariantList AnalysisLfpData::getTimesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevTimes(offsetTime);
}

/*! \brief Functions like getNextTimes() except that the input time is an absolute time with respect to this element's
 *	zero time instead of an offset.
 */
QVariantList AnalysisLfpData::getTimesUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextTimes(offsetTime);
}

/*! \brief Returns a list of lfp values for the input channel that occured with times > the input # sec before the latest
 *	frame and <= the latest frame time.
 */
QVariantList AnalysisLfpData::getPrevValues(int channel,double secsPreceding)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double minRunTime = getLatestRunTime()-secsPreceding;
	Q_ASSERT(lfpReader_);
	if(!lfpReader_)
		return QVariantList();
	QVariantList returnVal = lfpReader_->getValuesSince(channel,minRunTime);
	return returnVal;
}

/*! \brief Returns a list of lfp values for the input channel that will occur with times > the latest frame time
 *	and <= the input # sec after the latest frame and <= the latest frame time.
 */
QVariantList AnalysisLfpData::getNextValues(int channel,double secsFollowing)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double maxRunTime = getLatestRunTime()+secsFollowing;
	Q_ASSERT(lfpReader_);
	if(!lfpReader_)
		return QVariantList();
	QVariantList returnVal = lfpReader_->getValuesUntil(channel,maxRunTime);
	return returnVal;
}

/*! \brief Functions like getPrevValues() except that the input time is an absolute time with respect to this element's
 *	zero time instead of an offset.
 */
QVariantList AnalysisLfpData::getValuesSince(int channel,double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevValues(channel,offsetTime);
}

/*! \brief Functions like getNextValues() except that the input time is an absolute time with respect to this element's
 *	zero time instead of an offset.
 */
QVariantList AnalysisLfpData::getValuesUntil(int channel,double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextValues(channel,offsetTime);
}

/*! \brief Functions like getPrevValues() except that the input time is an absolute time with respect to this element's
 *	zero time instead of an offset.
 */
QVariantList AnalysisLfpData::getValuesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevValues(offsetTime);
}

/*! \brief Functions like getNextValues() except that the input time is an absolute time with respect to this element's
 *	zero time instead of an offset.
 */
QVariantList AnalysisLfpData::getValuesUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextValues(offsetTime);
}

/*! \brief Returns a list of lfp value lists (where sub lists are ordered like the result of getChannels()) that will
 *	occur with times > the input # sec before the latest frame time and <= the latest frame time. Times should be
 *	incremented by one getSamplePeriod() for every new entry in the outer list.
 */
QVariantList AnalysisLfpData::getPrevValues(double secsPreceding)
{
	QVariantList channels = getChannels();
	QList<QVariantList> resultsByName;
	foreach(QVariant channel,channels)
	{
		resultsByName.append(getPrevValues(channel.toInt(),secsPreceding));
	}
	//Make sure all lists are the same size
	for(int i=1;i<resultsByName.size();i++)
	{
		if(resultsByName[i].size() != resultsByName[i-1].size())
		{
			Q_ASSERT(resultsByName[i].size() == resultsByName[i-1].size());
			return QVariantList();
		}
	}
	//Put together result list
	QVariantList result;
	QVariantList singleRow;
	if(resultsByName.size())
	{
		result.reserve(resultsByName[0].size());
		for(int i=0;i<resultsByName[0].size();i++)
		{
			singleRow.clear();
			for(int j=0;j<resultsByName.size();j++)
			{
				singleRow.append(resultsByName[j][i]);
			}
			result.append(QVariant(singleRow));
		}
	}
	return result;
}

/*! \brief Returns a list of lfp value lists (where sub lists are ordered like the result of getChannels()) that will
 *	occur with times > the latest frame time and <= the input # sec after the latest frame and > the latest frame time.
 *	Times should be incremented by one getSamplePeriod() for every new entry in the outer list.
 */
QVariantList AnalysisLfpData::getNextValues(double secsFollowing)
{
	QVariantList channels = getChannels();
	QList<QVariantList> resultsByName;
	foreach(QVariant channel,channels)
	{
		resultsByName.append(getNextValues(channel.toInt(),secsFollowing));
	}
	//Make sure all lists are the same size
	for(int i=1;i<resultsByName.size();i++)
	{
		if(resultsByName[i].size() != resultsByName[i-1].size())
		{
			Q_ASSERT(resultsByName[i].size() == resultsByName[i-1].size());
			return QVariantList();
		}
	}
	//Put together result list
	QVariantList result;
	QVariantList singleRow;
	if(resultsByName.size())
	{
		result.reserve(resultsByName[0].size());
		for(int i=0;i<resultsByName[0].size();i++)
		{
			singleRow.clear();
			for(int j=0;j<resultsByName.size();j++)
			{
				singleRow.append(resultsByName[j][i]);
			}
			result.append(QVariant(singleRow));
		}
	}
	return result;
}

void AnalysisLfpData::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool AnalysisLfpData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}

/*! \brief Returns the latest frame time with respect to the beginning of the run.
 */
double AnalysisLfpData::getLatestRunTime()
{
	double latestTime = getDesignConfig()->getFrameReader()->getLatestTime();
	if(latestTime < 0)
		latestTime = 0;
	return latestTime;
}
} //namespace Picto