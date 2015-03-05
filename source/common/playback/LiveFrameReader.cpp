#include "LiveFrameReader.h"
using namespace Picto;

/*! \brief Called when a new Run Starts to let this object know that it should clear out its frame list and start again.
*/
void LiveFrameReader::setRunStart()
{
	frameTimes_.clear();
}

/*! \brief Called to add a new frame time to this object so that it can 
*	be included in future data requests.
*/
void LiveFrameReader::setLatestFrameTime(double time)
{
	frameTimes_.append(time);
}

/*! \brief Called when a Run Ends to let this object know that it can clear out its frame list.
*/
void LiveFrameReader::setRunEnd()
{
	frameTimes_.clear();
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

QVariantList LiveFrameReader::getTimesUntil(double)
{
	//Since we can't see the future, we just always return an empty list.
	return QVariantList();
}