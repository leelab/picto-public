#include "LiveLfpReader.h"
using namespace Picto;

LiveLfpReader::LiveLfpReader(QList<int> channels,double samplePeriod)
:	samplePeriod_(samplePeriod)
{
	Q_ASSERT(channels.size());
	QList<int> localChannels = channels;
	qSort(localChannels);
	for(int i=0;i<localChannels.size();i++)
	{
		chanIndexMap_[localChannels[i]] = i;
		channels_.append(localChannels[i]);
	}
	numChannels_ = localChannels.size();

	for(int i=0;i<numChannels_;i++)
	{
		periods_.append(1.0 - (.1*double(i)));
		amplitudes_.append(5+i);
	}
}

void LiveLfpReader::setRunStart()
{
	data_.clear();
}

void LiveLfpReader::createVirtualLfpData(double time)
{
	if(time < 0)
		return;
	if(data_.isEmpty())
		data_.append(time);
	while(data_.last()+samplePeriod_ <= time)
	{
		data_.append(data_.last()+samplePeriod_);
	}
}

void LiveLfpReader::setRunEnd()
{
	data_.clear();
}
QVariantList LiveLfpReader::getChannels()
{
	return channels_;
}
double LiveLfpReader::getSamplePeriod()
{
	return samplePeriod_;
}

double LiveLfpReader::getLatestTime()
{
	return data_.last();
}

double LiveLfpReader::getLatestValue(int channel)
{
	return getValue(channel,data_.last());
}

double LiveLfpReader::getNextTime()
{
	return -1;
}
double LiveLfpReader::getNextValue(QString channel)
{
	return 0;
}

QVariantList LiveLfpReader::getValuesSince(int channel,double time)
{
	if(!data_.size())
		return QVariantList();
	if(!chanIndexMap_.contains(channel))
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QVector<double>::iterator iter = qUpperBound<QVector<double>::iterator,double>(data_.begin(),data_.end(),time);
	QVariantList returnVal;
	for(;iter < data_.end();iter++)
	{
		returnVal.append(getValue(channel,*iter));
	}
	return returnVal;
}

QVariantList LiveLfpReader::getValuesUntil(int channel,double time)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}

QVariantList LiveLfpReader::getTimesSince(double time)
{
	if(!data_.size())
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QVector<double>::iterator iter = qUpperBound<QVector<double>::iterator,double>(data_.begin(),data_.end(),time);
	QVariantList returnVal;
	for(;iter < data_.end();iter++)
	{
		returnVal.append(*iter);
	}
	return returnVal;
}

QVariantList LiveLfpReader::getTimesUntil(double time)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}

float LiveLfpReader::getValue(int channel, double time)
{
	int index = chanIndexMap_[channel];
	return amplitudes_[index] * sin(6.28318530718*time/periods_[index]);
}