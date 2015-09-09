#include "AnalysisSignalData.h"
#include "../memleakdetect.h"

namespace Picto
{
/*! \brief Creates a new AnalysisSignalData object.
 *	\details Adds a SignalName property for defining which signal this object will gather data from.
 */
AnalysisSignalData::AnalysisSignalData()
: 
  AnalysisDataSource()
{
	AddDefinableProperty(QVariant::String,"SignalName","Position");
	zeroTime_ = 0;
}

/*! \brief Creates a new AnalysisSignalData object and returns a shared Asset pointer to it.
 */
QSharedPointer<Asset> AnalysisSignalData::Create()
{
	return QSharedPointer<Asset>(new AnalysisSignalData());
}

void AnalysisSignalData::enteredScope()
{
	AnalysisDataSource::enteredScope();
	zeroLatestFrame();
	signalReader_ = getDesignConfig()->getSignalReader(propertyContainer_->getPropertyValue("SignalName").toString());
}

/*! \brief Sets the time of the latest frame to zero.  Signal times will be returned with respect to this latest frame.
 */
void AnalysisSignalData::zeroLatestFrame()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return;
	double latestTime = getLatestRunTime();
	zeroTime_ = latestTime;
}

//! Returns the zero time of this element, represented in global behavioral time
double AnalysisSignalData::getZeroTime() const
{
	return zeroTime_;
}

/*! \brief Returns a list of the signal's sub component names, ie. x,y
 */
QStringList AnalysisSignalData::getComponentNames()
{
	if(!signalReader_)
		return QStringList();
	return signalReader_->getComponentNames();
}

/*! \brief Returns the sample period for this signal in seconds.
 */
double AnalysisSignalData::getSamplePeriod()
{
	if(!signalReader_)
		return 0;
	return signalReader_->getSamplePeriod();
}

/*! \brief Returns the time of the latest signal to have been read in seconds
 */
double AnalysisSignalData::getLatestTime()
{
	if(!signalReader_)
		return -1;
	double result = signalReader_->getLatestTime() - zeroTime_;
	return result;
}

/*! \brief Returns the latest value of the input signal component
 */
double AnalysisSignalData::getLatestValue(QString componentName)
{
	if(!signalReader_)
		return 0;
	return signalReader_->getLatestValue(componentName);
}

/*! Returns a list of the latest values ordered according to the sub channel order returned from getComponentNames()
 */
QVariantList AnalysisSignalData::getLatestValue()
{
	if(!signalReader_)
		return QVariantList();
	QStringList subChannels = signalReader_->getComponentNames();
	QVariantList result;
	foreach(QString subChannel,subChannels)
	{
		result.append(getLatestValue(subChannel));
	}
	return result;
}

/*! \brief Returns the time that the next signal will be read.
 *	\note In a live test, this will not return a valid value.
 */
double AnalysisSignalData::getNextTime()
{
	if(!signalReader_)
		return -1;
	double result = signalReader_->getNextTime() - zeroTime_;
	return result;
}

/*! \brief Returns the next value that will be read for the input signal component
 *	\note In a live test, this will not return a valid value.
 */
double AnalysisSignalData::getNextValue(QString componentName)
{
	if(!signalReader_)
		return 0;
	return signalReader_->getNextValue(componentName);
}

/*! \brief Returns an array of the next values to be read ordered according to the sub channel order returned from
 *	getComponentNames()
 *	\note In a live test, this will not return a valid value.
 */
QVariantList AnalysisSignalData::getNextValue()
{
	if(!signalReader_)
		return QVariantList();
	QStringList subChannels = signalReader_->getComponentNames();
	QVariantList result;
	foreach(QString subChannel,subChannels)
	{
		result.append(getNextValue(subChannel));
	}
	return result;
}

/*! \brief Returns a list of signal read times that occured with times > the input # sec before the latest frame
 *	and <= the latest frame time.
 */
QVariantList AnalysisSignalData::getPrevTimes(double secsPreceding)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double minRunTime = getLatestRunTime()-secsPreceding;
	Q_ASSERT(signalReader_);
	if(!signalReader_)
		return QVariantList();
	QVariantList returnVal = signalReader_->getTimesSince(minRunTime);
	if(zeroTime_)
	{
		for(int i=0;i<returnVal.size();i++)
			returnVal[i] = returnVal[i].toDouble() - zeroTime_;
	}
	return returnVal;
}

/*! \brief Returns a list of signal read times that will occur with times > the latest frame time and <= the input # sec
 *	after the latest frame.
 *	\note In a live test, this will not return a valid value.
 */
QVariantList AnalysisSignalData::getNextTimes(double secsFollowing)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double maxRunTime = getLatestRunTime()+secsFollowing;
	Q_ASSERT(signalReader_);
	if(!signalReader_)
		return QVariantList();
	QVariantList returnVal = signalReader_->getTimesUntil(maxRunTime);
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
QVariantList AnalysisSignalData::getTimesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevTimes(offsetTime);
}

/*! \brief Functions like getNextTimes() except that the input time is an absolute time with respect to this element's
 *	zero time instead of an offset.
 */
QVariantList AnalysisSignalData::getTimesUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextTimes(offsetTime);
}

/*! \brief Returns a list of signal values for the input sub component that occured with times > the input # sec before
 *	the latest frame and <= the latest frame time.
 */
QVariantList AnalysisSignalData::getPrevValues(QString componentName,double secsPreceding)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double minRunTime = getLatestRunTime()-secsPreceding;
	Q_ASSERT(signalReader_);
	if(!signalReader_)
		return QVariantList();
	QVariantList returnVal = signalReader_->getValuesSince(componentName,minRunTime);
	return returnVal;
}

/*! \brief Returns a list of signal values for the input sub component that will occur with times > the latest frame time
 *	and <= the input # sec after the latest frame
 *	\note In a live test, this will not return a valid value.
 */
QVariantList AnalysisSignalData::getNextValues(QString componentName,double secsFollowing)
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return QVariantList();
	double maxRunTime = getLatestRunTime()+secsFollowing;
	Q_ASSERT(signalReader_);
	if(!signalReader_)
		return QVariantList();
	QVariantList returnVal = signalReader_->getValuesUntil(componentName,maxRunTime);
	return returnVal;
}

/*! \brief Returns a list of signal values for all subcomponents (ordered like the result of getComponentNames()) that
 *	will occur with times > the input # sec before the latest frame time and <= the latest frame time. 
 *	\details Returned value is a list of lists such that each index of the list contains a list of signal subcomponents
 *	ordered like the result of getComponentNames(). Times should be incremented by one getSamplePeriod() for each index
 *	in the outer list.
 */
QVariantList AnalysisSignalData::getPrevValues(double secsPreceding)
{
	QStringList subChannels = getComponentNames();
	QList<QVariantList> resultsByName;
	foreach(QString subChan,subChannels)
	{
		resultsByName.append(getPrevValues(subChan,secsPreceding));
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

/*! \brief Returns a list of signal values for all subcomponents that will occur with times > the latest frame 
 *	time and <= the input # sec after the latest frame. 
 *	\details Returned value is a list of lists such that each index of the list contains a list signal subcomponents 
 *	ordered like the result of getComponentNames(). Times should be incremented by one getSamplePeriod() for each index
 *	in the outer list.
 */
QVariantList AnalysisSignalData::getNextValues(double secsFollowing)
{
	QStringList subChannels = getComponentNames();
	QList<QVariantList> resultsByName;
	foreach(QString subChan,subChannels)
	{
		resultsByName.append(getNextValues(subChan,secsFollowing));
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

/*!	\brief Functions like getPrevValues() except that the input time is an absolute time with respect to this element's
 *	zero time instead of an offset.
 */
QVariantList AnalysisSignalData::getValuesSince(QString componentName,double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevValues(componentName,offsetTime);
}

/*! \brief Functions like getNextValues() except that the input time is an absolute time with respect to this element's
 *	zero time instead of an offset.
 */
QVariantList AnalysisSignalData::getValuesUntil(QString componentName,double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextValues(componentName,offsetTime);
}

/*! \brief Functions like getPrevValues() except that the input time is an absolute time with respect to this element's
 *	zero time instead of an offset.
 */
QVariantList AnalysisSignalData::getValuesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevValues(offsetTime);
}

/*! \brief Functions like getNextValues() except that the input time is an absolute time with respect to this element's
 *	zero time instead of an offset.
 */
QVariantList AnalysisSignalData::getValuesUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextValues(offsetTime);
}

void AnalysisSignalData::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool AnalysisSignalData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}

/*! \brief Returns the latest frame time with respect to the beginning of the run.
 */
double AnalysisSignalData::getLatestRunTime()
{
	double latestTime = getDesignConfig()->getFrameReader()->getLatestTime();
	if(latestTime < 0)
		latestTime = 0;
	return latestTime;
}
} //namespace Picto