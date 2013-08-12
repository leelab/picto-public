#include "LiveFrameReader.h"
using namespace Picto;

void LiveFrameReader::setRunStart()
{
	frameTimes_.clear();
	runStart_ = 0;
}

void LiveFrameReader::setLatestFrameTime(double time)
{
	if(frameTimes_.isEmpty())
		runStart_ = time;
	frameTimes_.append(time-runStart_);
}

void LiveFrameReader::setRunEnd()
{
	frameTimes_.clear();
	runStart_ = 0;
}

double LiveFrameReader::getPrevTime()
{
	if(frameTimes_.size() < 2)
		return 0;
	return frameTimes_[frameTimes_.size()-2];
}
double LiveFrameReader::getLatestTime()
{
	if(!frameTimes_.size())
		return 0;
	return frameTimes_[frameTimes_.size()-1];
}
double LiveFrameReader::getNextTime()
{
	return -1;
}
QVariantList LiveFrameReader::getTimesSince(double time)
{
	if(!frameTimes_.size())
		return QVariantList();
	double afterTime = time;
	double latestTime = getLatestTime();
	if(afterTime >= latestTime)
		return QVariantList();
	QVector<double>::iterator iter = qUpperBound<QVector<double>::iterator,double>(frameTimes_.begin(),frameTimes_.end(),time);
	QVariantList returnVal;
	for(;iter < frameTimes_.end();iter++)
	{
		returnVal.append(*iter);
	}
	return returnVal;
}

QVariantList LiveFrameReader::getTimesUntil(double time)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}