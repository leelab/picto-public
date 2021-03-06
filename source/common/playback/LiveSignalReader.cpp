#include "LiveSignalReader.h"
using namespace Picto;

/*! \brief Constructs a new LiveSignalReader.
 *	\details name is the name of the signal to be tracked by this LiveSignalReader (ie. "Position").
 *	subChanNames is a list of the names of the subchannels of the signal tracked by this object
 *	(ie. "x","y").  samplePeriod is the time per sample for this signal channel.
 */
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
/*! \brief Called when a new Run Starts to let this object know that it should clear out its signal data list and start again.
*/
void LiveSignalReader::setRunStart()
{
	signalData_.clear();
}
/*! \brief Called to add new signal data to this object so that it can 
*	be included in future data requests.
*	\details time is the time that the data was read.  vals is a list of floats
*	ordered like getComponentNames() with the signal values (They are already calibrated according
*	to the current Experiment calibration values).
*/
void LiveSignalReader::setLatestSignalData(double time,QVector<float> vals)
{ 
	signalData_.append(PlaybackSignalData(time,vals));
}
/*! \brief Called when a Run Ends to let this object know that it can clear out its signal data list.
*/
void LiveSignalReader::setRunEnd()
{
	signalData_.clear();
}
/*! \brief Returns the name of the Signal tracked by this LiveSignalReader (ie. "Position").
*/
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

QVariantList LiveSignalReader::getValuesUntil(QString channel,double)
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

QVariantList LiveSignalReader::getTimesUntil(double)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}