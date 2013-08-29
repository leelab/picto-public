#include "LiveSpikeReader.h"
using namespace Picto;

LiveSpikeReader::LiveSpikeReader(int maxChan,int maxUnit,int waveformSize)
{
	maxChans_=maxChan;
	maxUnits_=maxUnit;
	waveformSize_=waveformSize;
	for(int i=0;i<maxChans_;i++)
	{
		chans_.append(i);
	}
	for(int i=0;i<maxUnits_;i++)
	{
		units_.append(i);
	}
}

void LiveSpikeReader::setRunStart()
{
	spikeData_.clear();
}

void LiveSpikeReader::createVirtualSpike(double time)
{
	int chan = rand()%maxChans_;
	int unit = rand()%maxUnits_;
	QVariantList waveform;
	for(int i=0;i<waveformSize_;i++)
	{
		waveform.append((10.0*sin(double(i)*6.28318530718/double(waveformSize_)))+(double(rand()%100)/20.0));
	}
	spikeData_.append(SpikeData(time,chan,unit,waveform));
}

void LiveSpikeReader::setRunEnd()
{
	spikeData_.clear();
}

QVariantList LiveSpikeReader::getChannels()
{
	return chans_;
}

QVariantList LiveSpikeReader::getUnits(int channel)
{
	return units_;
}

double LiveSpikeReader::getLatestTime()
{
	if(!spikeData_.size())
		return 0;
	return spikeData_[spikeData_.size()-1].time_;
}
int LiveSpikeReader::getLatestChannel()
{
	if(!spikeData_.size())
		return 0;
	return spikeData_[spikeData_.size()-1].channel_;
}
int LiveSpikeReader::getLatestUnit()
{
	if(!spikeData_.size())
		return 0;
	return spikeData_[spikeData_.size()-1].unit_;
}
QVariantList LiveSpikeReader::getLatestWaveform()
{
	if(!spikeData_.size())
		return QVariantList();
	return spikeData_[spikeData_.size()-1].waveform_;
}
double LiveSpikeReader::getNextTime()
{
	return -1;
}
int LiveSpikeReader::getNextChannel()
{
	return -1;
}
int LiveSpikeReader::getNextUnit()
{
	return -1;
}
QVariantList LiveSpikeReader::getNextWaveform()
{
	return QVariantList();
}
QVariantList LiveSpikeReader::getTimesSince(double time)
{
	if(!spikeData_.size())
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QVector<SpikeData>::iterator iter = qUpperBound<QVector<SpikeData>::iterator,SpikeData>(spikeData_.begin(),spikeData_.end(),SpikeData(time,0,0,QVariantList()));
	QVariantList returnVal;
	for(;iter < spikeData_.end();iter++)
	{
		returnVal.append(iter->time_);
	}
	return returnVal;
}

QVariantList LiveSpikeReader::getTimesUntil(double)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}
QVariantList LiveSpikeReader::getChannelsSince(double time)
{
	if(!spikeData_.size())
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QVector<SpikeData>::iterator iter = qUpperBound<QVector<SpikeData>::iterator,SpikeData>(spikeData_.begin(),spikeData_.end(),SpikeData(time,0,0,QVariantList()));
	QVariantList returnVal;
	for(;iter < spikeData_.end();iter++)
	{
		returnVal.append(iter->channel_);
	}
	return returnVal;
}
QVariantList LiveSpikeReader::getChannelsUntil(double)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}
QVariantList LiveSpikeReader::getUnitsSince(double time)
{
	if(!spikeData_.size())
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QVector<SpikeData>::iterator iter = qUpperBound<QVector<SpikeData>::iterator,SpikeData>(spikeData_.begin(),spikeData_.end(),SpikeData(time,0,0,QVariantList()));
	QVariantList returnVal;
	for(;iter < spikeData_.end();iter++)
	{
		returnVal.append(iter->unit_);
	}
	return returnVal;	
}
QVariantList LiveSpikeReader::getUnitsUntil(double)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}
QVariantList LiveSpikeReader::getWaveformsSince(double time)
{
	if(!spikeData_.size())
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QVector<SpikeData>::iterator iter = qUpperBound<QVector<SpikeData>::iterator,SpikeData>(spikeData_.begin(),spikeData_.end(),SpikeData(time,0,0,QVariantList()));
	QVariantList returnVal;
	for(;iter < spikeData_.end();iter++)
	{
		returnVal.append(QVariant(iter->waveform_));
	}
	return returnVal;	
}
QVariantList LiveSpikeReader::getWaveformsUntil(double)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}