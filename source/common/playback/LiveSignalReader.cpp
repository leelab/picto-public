#include "LiveSignalReader.h"
using namespace Picto;

LiveSignalReader::LiveSignalReader(QString name,QStringList subChanNames,double samplePeriod)
:	name_(name),
	subChanNames_(subChanNames),
	numSubChans_(subChanNames.size()),
	samplePeriod_(samplePeriod)
{
	Q_ASSERT(numSubChans_);
	for(int i=0;i<numSubChans_;i++)
	{
		subChanIndexLookup_[subChanNames_[i]] = i;
	}
}

void LiveSignalReader::setRunStart()
{
	signalData_.clear();
}

void LiveSignalReader::setLatestSignalData(double time,QVector<float> vals)
{ 
	signalData_.append(PlaybackSignalData(time,vals));
}

void LiveSignalReader::setRunEnd()
{
	signalData_.clear();
}
QString LiveSignalReader::getName()
{
	return name_;
}
QStringList LiveSignalReader::getComponentNames()
{
	return subChanNames_;
}
double LiveSignalReader::getSamplePeriod()
{
	return samplePeriod_;
}

double LiveSignalReader::getLatestTime()
{
	if(!signalData_.size())
		return 0;
	return signalData_[signalData_.size()-1].time_;
}

double LiveSignalReader::getLatestValue(QString channel)
{
	if(!signalData_.size())
		return 0;
	if(!subChanIndexLookup_.contains(channel))
		return 0;
	return signalData_[signalData_.size()-1].vals_[subChanIndexLookup_[channel]];
}

double LiveSignalReader::getNextTime()
{
	return -1;
}
double LiveSignalReader::getNextValue(QString channel)
{
	return 0;
}

QVariantList LiveSignalReader::getValuesSince(QString channel,double time)
{
	if(!signalData_.size())
		return QVariantList();
	if(!subChanIndexLookup_.contains(channel))
		return QVariantList();
	int chanIndex = subChanIndexLookup_.value(channel);
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QVector<PlaybackSignalData>::iterator iter = qUpperBound<QVector<PlaybackSignalData>::iterator,PlaybackSignalData>(signalData_.begin(),signalData_.end(),time);
	QVariantList returnVal;
	for(;iter < signalData_.end();iter++)
	{
		returnVal.append(iter->vals_[chanIndex]);
	}
	return returnVal;
}

QVariantList LiveSignalReader::getValuesUntil(QString channel,double time)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}

QVariantList LiveSignalReader::getTimesSince(double time)
{
	if(!signalData_.size())
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QVector<PlaybackSignalData>::iterator iter = qUpperBound<QVector<PlaybackSignalData>::iterator,PlaybackSignalData>(signalData_.begin(),signalData_.end(),time);
	QVariantList returnVal;
	for(;iter < signalData_.end();iter++)
	{
		returnVal.append(iter->time_);
	}
	return returnVal;
}

QVariantList LiveSignalReader::getTimesUntil(double time)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}