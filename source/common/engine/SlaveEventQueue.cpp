#include <QDebug>

#include "SlaveEventQueue.h"

#include "../memleakdetect.h"

using namespace Picto;

SlaveEventQueue::SlaveEventQueue()
{
	reset();
}

/*! \brief Resets the SlaveEventQueue as if no event was ever added to it.
 */
void SlaveEventQueue::reset()
{
	firstLoc_ = 0;
	endLoc_ = 0;
	eventQueue_.clear();
	eventQueue_.insert(0, 10, SlaveEvent());

	frozenEndLoc_ = 0;
	frozenQueue_.clear();
	frozenQueue_.insert(0, 10, SlaveEvent());
}

/*! \brief Adds a SlaveEvent::PROP_VAL_CHANGE event to the SlaveEventQueue indicating that the Property with the input
 *	assetid changed its value to the input val.
 */
void SlaveEventQueue::addPropChange(const qulonglong& dataId, const int& assetId, const QString& val)
{
	frozenQueue_[getNextFrozenLoc()].setAsProp(dataId, assetId, val);
}
/*! \brief Adds a SlaveEvent::INIT_PROP_VAL_CHANGE change event to the SlaveEventQueue indicating that the Property with
 *	the input assetid changed its initValue to the input val.
 */
void SlaveEventQueue::addInitPropChange(const qulonglong& dataId, const int& assetId, const QString& val)
{
	frozenQueue_[getNextFrozenLoc()].setAsInitProp(dataId, assetId, val);
}
/*! \brief Adds a SlaveEvent::TRANS_ACTIVATED activation event to the SlaveEventQueue indicating that the Transition with
 *	the input assetId was traversed.
 */
void SlaveEventQueue::addTransActivation(const qulonglong& dataId, const int& assetId, bool remoteRunSignal)
{
	frozenQueue_[getNextFrozenLoc()].setAsTrans(dataId, assetId, remoteRunSignal);
}

/*! \brief Removes and returns the first SlaveEvent from the SlaveEventQueue.
 */
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

/*! \brief Returns the location in the circular buffer where the next SlaveEvent should be added.
 */
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

/*! \brief Returns the location in the Frozen circular buffer where the next SlaveEvent should be added.
 */
int SlaveEventQueue::getNextFrozenLoc()
{
	int returnVal = frozenEndLoc_;
	if (++frozenEndLoc_ >= frozenQueue_.size())
	{
		frozenQueue_.insert(frozenQueue_.end(), frozenQueue_.size(), SlaveEvent());
	}
	return returnVal;
}

void SlaveEventQueue::prepareEvents()
{
	//Sort FrozenQueue
	std::sort(frozenQueue_.begin(), frozenQueue_.begin() + frozenEndLoc_, [](const SlaveEvent &a, const SlaveEvent &b)
	{
		return a.dataId < b.dataId;
	});

	//Add FrozenQueue elements to eventQueue_
	for (int i = 0; i < frozenEndLoc_; i++)
	{
		eventQueue_[getNextAddLoc()] = frozenQueue_[i];
	}

	//Reset FrozenQueue
	frozenEndLoc_ = 0;
}
