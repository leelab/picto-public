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

QSharedPointer<Asset> AnalysisLfpData::Create()
{
	return QSharedPointer<Asset>(new AnalysisLfpData());
}

void AnalysisLfpData::reset()
{
	AnalysisDataSource::reset();
	zeroLatestFrame();
	lfpReader_ = getDesignConfig()->getLfpReader();
}

//Sets the time of the latest frame to zero.  Reward times will be returned with respect to
//this latest frame.
void AnalysisLfpData::zeroLatestFrame()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return;
	double latestTime = getLatestRunTime();
	zeroTime_ = latestTime;
}

//Returns the sub channels of the signal
QVariantList AnalysisLfpData::getChannels()
{
	if(!lfpReader_)
		return QVariantList();
	return lfpReader_->getChannels();
}

//Returns the sample period for this signal.
double AnalysisLfpData::getSamplePeriod()
{
	if(!lfpReader_)
		return 0;
	return lfpReader_->getSamplePeriod();
}

//Returns the time of the latest reward to have been supplied
double AnalysisLfpData::getLatestTime()
{
	if(!lfpReader_)
		return -1;
	double result = lfpReader_->getLatestTime() - zeroTime_;
	return result;
}

//Returns the latest value of the input subchannel
double AnalysisLfpData::getLatestValue(int channel)
{
	if(!lfpReader_)
		return 0;
	return lfpReader_->getLatestValue(channel);
}

//Returns an array of the latest values ordered according to the sub channel order returned from getComponentNames()
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

//Returns the time of the next signal to have be read
double AnalysisLfpData::getNextTime()
{
	if(!lfpReader_)
		return -1;
	double result = lfpReader_->getNextTime() - zeroTime_;
	return result;
}

//Returns the next value that will be read for the input signal component
double AnalysisLfpData::getNextValue(int channel)
{
	if(!lfpReader_)
		return 0;
	return lfpReader_->getNextValue(channel);
}

//Returns an array of the next values to be read ordered according to the sub channel order returned from getComponentNames()
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

//Returns a list of signal read times that occured with times > the input # sec before the latest frame and <= the latest frame time
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

//Returns a list of signal valuesread times that will occur with times > the latest frame time and <= the input # sec after the latest frame
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

QVariantList AnalysisLfpData::getTimesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevTimes(offsetTime);
}

QVariantList AnalysisLfpData::getTimesUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextTimes(offsetTime);
}

//Returns a list of signal values for the input sub channel that occured with times > the input # sec before the latest frame and <= the latest frame time
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
//Returns a list of signal values for the input sub channel that will occur with times > the latest frame time and <= the input # sec after the latest frame and <= the latest frame time
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

QVariantList AnalysisLfpData::getValuesSince(int channel,double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevValues(channel,offsetTime);
}
QVariantList AnalysisLfpData::getValuesUntil(int channel,double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextValues(channel,offsetTime);
}
QVariantList AnalysisLfpData::getValuesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevValues(offsetTime);
}
QVariantList AnalysisLfpData::getValuesUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextValues(offsetTime);
}

//Returns a list of signal values for all subcomponents (ordered like the result of getComponentNames())
//that will occur with times > the input # sec before the latest frame time and <= the latest 
//frame time. Times should be incremented by one getSamplePeriod() for every 
//getComponentNames().length() entries.
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
//Returns a list of signal values for all subcomponents (ordered like the result of getComponentNames())
//that will occur with times > the latest frame time and <= the input # sec after the latest frame 
//and <= the latest frame time. Times should be incremented by one getSamplePeriod() for every 
//getComponentNames().length() entries.
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

double AnalysisLfpData::getLatestRunTime()
{
	double latestTime = getDesignConfig()->getFrameReader()->getLatestTime();
	if(latestTime < 0)
		latestTime = 0;
	return latestTime;
}
} //namespace Picto