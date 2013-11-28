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
/*! \brief Creates a new AnalysisSpikeData object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> AnalysisSpikeData::Create()
{
	return QSharedPointer<Asset>(new AnalysisSpikeData());
}

void AnalysisSpikeData::enteredScope()
{
	AnalysisDataSource::enteredScope();
	zeroLatestFrame();
}
/*! \brief Sets the time of the latest frame to zero.  Spike times will be returned with respect to
 *	this latest frame.
*/
void AnalysisSpikeData::zeroLatestFrame()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return;
	double latestTime = getLatestRunTime();
	zeroTime_ = latestTime;
}
/*! \brief Returns the sample period in seconds used by the spike data acquisition system
*/
double AnalysisSpikeData::getSamplePeriod()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	return getDesignConfig()->getSpikeReader()->getSamplePeriod();
}
/*! \brief Returns the time of the latest spike to have been read.
*/
double AnalysisSpikeData::getLatestTime()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	double result = getDesignConfig()->getSpikeReader()->getLatestTime() - zeroTime_;
	return result;
}

/*! \brief Returns the channel of the latest spike to have been read.
*/
int AnalysisSpikeData::getLatestChannel()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	int result = getDesignConfig()->getSpikeReader()->getLatestChannel();
	return result;
}

/*! \brief Returns the unit of the latest spike to have been read.
*/
int AnalysisSpikeData::getLatestUnit()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	int result = getDesignConfig()->getSpikeReader()->getLatestUnit();
	return result;
}

/*! \brief Returns the waveform of the latest spike to have been read.
*/
QVariantList AnalysisSpikeData::getLatestWaveform()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList result = getDesignConfig()->getSpikeReader()->getLatestWaveform();
	return result;
}

/*! \brief Returns the time of the next spike to be read.
 *	\note In a live test, this will not return a valid value.
 */
double AnalysisSpikeData::getNextTime()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	double result = getDesignConfig()->getSpikeReader()->getNextTime() - zeroTime_;
	return result;
}

/*! \brief Returns the channel of the next spike to be read.
 *	\note In a live test, this will not return a valid value.
 */
int AnalysisSpikeData::getNextChannel()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	int result = getDesignConfig()->getSpikeReader()->getNextChannel();
	return result;
}

/*! \brief Returns the unit of the next spike to be read.
 *	\note In a live test, this will not return a valid value.
 */
int AnalysisSpikeData::getNextUnit()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return -1;
	int result = getDesignConfig()->getSpikeReader()->getNextUnit();
	return result;
}

/*! \brief Returns the waveform of the next spike to be read.
 *	\note In a live test, this will not return a valid value.
 */
QVariantList AnalysisSpikeData::getNextWaveform()
{
	Q_ASSERT(!getDesignConfig()->getSpikeReader().isNull());
	if(!getDesignConfig()->getSpikeReader())
		return QVariantList();
	QVariantList result = getDesignConfig()->getSpikeReader()->getNextWaveform();
	return result;
}
/*! \brief Returns a list of spike times that occured with times > the input # sec before the latest frame and <= the latest frame time.
*/
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

/*! \brief Returns a list of the channels of spikes that occured with times > the input # sec before the latest frame and <= the latest frame time.
*/
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

/*! \brief Returns a list of the units of spikes that occured with times > the input # sec before the latest frame and <= the latest frame time.
*/
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

/*! \brief Returns a list of the waveforms of spikes that occured with times > the input # sec before the latest frame and <= the latest frame time.
 *	\details This is a list of lists, such that the ith spike's waveform will be at the ith index
 *	of the returned list, and that value will itself be a list.
*/
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
/*! \brief Returns a list of spike times that will occur with times <= the input # sec after the latest frame and > the latest frame time.
 *	\note In a live test, this will not return a valid value.
 */
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

/*! \brief Returns a list of channels for spikes that will occur with times <= the input # sec after the latest frame and > the latest frame time.
 *	\note In a live test, this will not return a valid value.
 */
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

/*! \brief Returns a list of units for spikes that will occur with times <= the input # sec after the latest frame and > the latest frame time.
 *	\note In a live test, this will not return a valid value.
 */
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

/*! \brief Returns a list of waveforms for spikes that will occur with times <= the input # sec after the latest frame and > the latest frame time.
 *	\details This is a list of lists, such that the ith spike's waveform will be at the ith index
 *	of the returned list, and that value will itself be a list. 
 *	\note In a live test, this will not return a valid value.
 */
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
/*! \brief Functions like getPrevTimes() except that the input time is an absolute time with respect to this element's zero time instead of an offset
*/
QVariantList AnalysisSpikeData::getTimesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevTimes(offsetTime);
}
/*! \brief Functions like getNextTimes() except that the input time is an absolute time with respect to this element's zero time instead of an offset
*/
QVariantList AnalysisSpikeData::getTimesUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextTimes(offsetTime);
}
/*! \brief Functions like getPrevChannels() except that the input time is an absolute time with respect to this element's zero time instead of an offset
*/
QVariantList AnalysisSpikeData::getChannelsSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevChannels(offsetTime);
}
/*! \brief Functions like getNextChannels() except that the input time is an absolute time with respect to this element's zero time instead of an offset
*/
QVariantList AnalysisSpikeData::getChannelsUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextChannels(offsetTime);
}
/*! \brief Functions like getPrevUnits() except that the input time is an absolute time with respect to this element's zero time instead of an offset
*/
QVariantList AnalysisSpikeData::getUnitsSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevUnits(offsetTime);
}
/*! \brief Functions like getNextUnits() except that the input time is an absolute time with respect to this element's zero time instead of an offset
*/
QVariantList AnalysisSpikeData::getUnitsUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextUnits(offsetTime);
}
/*! \brief Functions like getPrevWaveforms() except that the input time is an absolute time with respect to this element's zero time instead of an offset
*/
QVariantList AnalysisSpikeData::getWaveformsSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevWaveforms(offsetTime);
}
/*! \brief Functions like getNextWaveforms() except that the input time is an absolute time with respect to this element's zero time instead of an offset
*/
QVariantList AnalysisSpikeData::getWaveformsUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextWaveforms(offsetTime);
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
/*! \brief Returns the latest frame time with respect to the beginning of the run
*/
double AnalysisSpikeData::getLatestRunTime()
{
	double latestTime = getDesignConfig()->getFrameReader()->getLatestTime();
	if(latestTime < 0)
		latestTime = 0;
	return latestTime;
}
} //namespace Picto