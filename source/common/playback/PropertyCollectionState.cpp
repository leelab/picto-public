#include "PropertyCollectionState.h"
#include "PlaybackData.h"
using namespace Picto;

PropertyCollectionState::PropertyCollectionState() :
DataState(true),
transmitChangeSig_(true)
{

}

PropertyCollectionState::~PropertyCollectionState()
{

}

void PropertyCollectionState::reset()
{
	DataState::reset();
	propLookup_.clear();
}

void PropertyCollectionState::setBoundTimes(double minTime,double maxTime)
{
	DataState::setBoundTimes(minTime,maxTime);
	foreach(QSharedPointer<PropertyState> propState,propLookup_)
	{
		propState->setBoundTimes(minTime,maxTime);
	}
}

void PropertyCollectionState::setFinishedLoading()
{
	DataState::setFinishedLoading();
	foreach(QSharedPointer<PropertyState> propState,propLookup_)
	{
		propState->setFinishedLoading();
	}
}

void PropertyCollectionState::clearDataBefore(double time)
{
	DataState::clearDataBefore(time);
	foreach(QSharedPointer<PropertyState> propState,propLookup_)
	{
		propState->clearDataBefore(time);
	}
}

QSharedPointer<PropertyReader> PropertyCollectionState::getPropertyReader(int propId)
{
	if(propLookup_.contains(propId)) 
		return propLookup_[propId];
	return QSharedPointer<PropertyReader>();
}

bool PropertyCollectionState::setPropertyValue(double time,qulonglong dataId,int propId,QString value)
{
	if(!propLookup_.contains(propId))
	{
		QSharedPointer<PropertyState> newPropState = QSharedPointer<PropertyState>(new PropertyState(propId));
		propLookup_[propId] = newPropState;
		connect(newPropState.data(),SIGNAL(propertyChanged(int,QString)),this,SLOT(subPropertyChanged(int,QString)));
	}
	setValue(QSharedPointer<IndexedData>(new PlaybackData<int>(propId,time,dataId)));
	propLookup_[propId]->setPropertyValue(time,dataId,value);
	return true;
}

void  PropertyCollectionState::triggerValueChange(bool reverse,bool last)
{
	if(reverse)
	{	//When moving in reverse, we must check the last property (the one that is 
		//next in time) and tell the PropertyState for that property to move back
		//to its previous value.
		QSharedPointer<PlaybackData<int>> nextVal = getNextValue(-1).staticCast<PlaybackData<int>>();
		Q_ASSERT(nextVal);
		Q_ASSERT(propLookup_.contains(nextVal->data_));
		//Only transmit change signals from underlying properties to the outside
		//if this is the last triggerValueChange (ie. This brings us to the index
		//that was set in DataState::setCurrentIndex().
		transmitChangeSig_ = last;
		QSharedPointer<IndexedData> currVal = getCurrentValue();
		if(currVal)
			propLookup_.value(nextVal->data_)->setCurrentIndex(getCurrentValue()->index_);
		else	//If this is the case, we've moved back before the first value
			propLookup_.value(nextVal->data_)->setCurrentIndex(PlaybackIndex());
		transmitChangeSig_ = true;
	}
	else
	{	//Tell the PropertyState for the property that just changed to update
		//its value.
		QSharedPointer<PlaybackData<int>> currVal = getCurrentValue().staticCast<PlaybackData<int>>();
		Q_ASSERT(currVal);
		Q_ASSERT(propLookup_.contains(currVal->data_));
		propLookup_.value(currVal->data_)->setCurrentIndex(currVal->index_);
	}
}

void PropertyCollectionState::requestMoreData(PlaybackIndex currLast,PlaybackIndex to)
{
	emit needsData(currLast,to);
}
void PropertyCollectionState::requestNextData(PlaybackIndex currLast,bool backward)
{
	emit needsNextData(currLast,backward);
}

void PropertyCollectionState::subPropertyChanged(int propId, QString value)
{
	if(transmitChangeSig_)
		emit propertyChanged(propId,value);
}