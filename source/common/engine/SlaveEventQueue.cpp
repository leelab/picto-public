#include "SlaveEventQueue.h"
#include "../memleakdetect.h"

using namespace Picto;

SlaveEventQueue::SlaveEventQueue()
{
	reset();
}

void SlaveEventQueue::reset()
{
	firstLoc_ = 0;
	endLoc_ = 0;
	eventQueue_.clear();
	eventQueue_.insert(0,10,SlaveEvent());
}

void SlaveEventQueue::addPropChange(const int& assetId,const QString& val)
{
	eventQueue_[getNextAddLoc()].setAsProp(assetId,val);
}
void SlaveEventQueue::addInitPropChange(const int& assetId,const QString& val)
{
	eventQueue_[getNextAddLoc()].setAsInitProp(assetId,val);
}
void SlaveEventQueue::addTransActivation(const int& assetId)
{
	eventQueue_[getNextAddLoc()].setAsTrans(assetId);
}

SlaveEvent SlaveEventQueue::takeFirstEvent()
{
	if(firstLoc_ == endLoc_)
		return SlaveEvent();
	int returnLoc = firstLoc_;
	firstLoc_++;
	if(firstLoc_ >= eventQueue_.size())
		firstLoc_ = 0;
	return eventQueue_[returnLoc];
}

int SlaveEventQueue::getNextAddLoc()
{
	//Move endLoc_ to the next available position, looping to the beginning if necessary
	int nextLoc = endLoc_ + 1;
	if(nextLoc >= eventQueue_.size())
		nextLoc = 0;

	//If the next location is the same as the first location, insert elements in between
	//them and update them
	if(nextLoc == firstLoc_)
	{
		//Double the size of the queue by putting new elements before firstLoc_
		if(endLoc_ >= firstLoc_)
			endLoc_ += eventQueue_.size();
		firstLoc_ += eventQueue_.size();
		eventQueue_.insert(nextLoc,eventQueue_.size(),SlaveEvent());
	}
	int returnVal = endLoc_;
	endLoc_ = nextLoc;
	return returnVal;
}