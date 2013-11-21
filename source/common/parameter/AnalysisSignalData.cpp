#include "AnalysisSignalData.h"
#include "../memleakdetect.h"

namespace Picto
{
AnalysisSignalData::AnalysisSignalData()
: 
  AnalysisDataSource()
{
	AddDefinableProperty(QVariant::String,"SignalName","Position");
	zeroTime_ = 0;
}

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

//Sets the time of the latest frame to zero.  Reward times will be returned with respect to
//this latest frame.
void AnalysisSignalData::zeroLatestFrame()
{
	Q_ASSERT(!getDesignConfig()->getFrameReader().isNull());
	if(!getDesignConfig()->getFrameReader())
		return;
	double latestTime = getLatestRunTime();
	zeroTime_ = latestTime;
}

//Returns the sub channels of the signal
QStringList AnalysisSignalData::getComponentNames()
{
	if(!signalReader_)
		return QStringList();
	return signalReader_->getComponentNames();
}

//Returns the sample period for this signal.
double AnalysisSignalData::getSamplePeriod()
{
	if(!signalReader_)
		return 0;
	return signalReader_->getSamplePeriod();
}

//Returns the time of the latest reward to have been supplied
double AnalysisSignalData::getLatestTime()
{
	if(!signalReader_)
		return -1;
	double result = signalReader_->getLatestTime() - zeroTime_;
	return result;
}

//Returns the latest value of the input subchannel
double AnalysisSignalData::getLatestValue(QString componentName)
{
	if(!signalReader_)
		return 0;
	return signalReader_->getLatestValue(componentName);
}

//Returns an array of the latest values ordered according to the sub channel order returned from getComponentNames()
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

//Returns the time of the next signal to have be read
double AnalysisSignalData::getNextTime()
{
	if(!signalReader_)
		return -1;
	double result = signalReader_->getNextTime() - zeroTime_;
	return result;
}

//Returns the next value that will be read for the input signal component
double AnalysisSignalData::getNextValue(QString componentName)
{
	if(!signalReader_)
		return 0;
	return signalReader_->getNextValue(componentName);
}

//Returns an array of the next values to be read ordered according to the sub channel order returned from getComponentNames()
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

//Returns a list of signal read times that occured with times > the input # sec before the latest frame and <= the latest frame time
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

//Returns a list of signal valuesread times that will occur with times > the latest frame time and <= the input # sec after the latest frame
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

QVariantList AnalysisSignalData::getTimesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevTimes(offsetTime);
}

QVariantList AnalysisSignalData::getTimesUntil(double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextTimes(offsetTime);
}

//Returns a list of signal values for the input sub channel that occured with times > the input # sec before the latest frame and <= the latest frame time
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
//Returns a list of signal values for the input sub channel that will occur with times > the latest frame time and <= the input # sec after the latest frame and <= the latest frame time
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
//Returns a list of signal values for all subcomponents (ordered like the result of getComponentNames())
//that will occur with times > the input # sec before the latest frame time and <= the latest 
//frame time. Times should be incremented by one getSamplePeriod() for every 
//getComponentNames().length() entries.
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
//Returns a list of signal values for all subcomponents (ordered like the result of getComponentNames())
//that will occur with times > the latest frame time and <= the input # sec after the latest frame 
//and <= the latest frame time. Times should be incremented by one getSamplePeriod() for every 
//getComponentNames().length() entries.
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

QVariantList AnalysisSignalData::getValuesSince(QString componentName,double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevValues(componentName,offsetTime);
}
QVariantList AnalysisSignalData::getValuesUntil(QString componentName,double upToTime)
{
	double globalTime = upToTime+zeroTime_;
	double offsetTime = globalTime - getLatestRunTime();
	return getNextValues(componentName,offsetTime);
}
QVariantList AnalysisSignalData::getValuesSince(double beyondTime)
{
	double globalTime = beyondTime+zeroTime_;
	double offsetTime = getLatestRunTime()-globalTime;
	return getPrevValues(offsetTime);
}
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

double AnalysisSignalData::getLatestRunTime()
{
	double latestTime = getDesignConfig()->getFrameReader()->getLatestTime();
	if(latestTime < 0)
		latestTime = 0;
	return latestTime;
}
} //namespace Picto