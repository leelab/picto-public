#include "PropertyCollectionState.h"
using namespace Picto;

PropertyCollectionState::PropertyCollectionState() :
DataState(true),
transmitChangeSig_(true)
{

}

PropertyCollectionState::~PropertyCollectionState()
{

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
	setValue(QSharedPointer<IndexedData<qulonglong>>(new PropertyCollectionData(0.0,dataId,propId)));
	propLookup_[propId]->setPropertyValue(time,dataId,value);
	return true;
}

void  PropertyCollectionState::triggerValueChange(bool reverse,bool last)
{
	if(reverse)
	{	//When moving in reverse, we must check the last property (the one that is 
		//next in time) and tell the PropertyState for that property to move back
		//to its previous value.
		QSharedPointer<PropertyCollectionData> nextVal = getNextValue().staticCast<PropertyCollectionData>();
		Q_ASSERT(nextVal);
		Q_ASSERT(propLookup_.contains(nextVal->propId_));
		//Only transmit change signals from underlying properties to the outside
		//if this is the last triggerValueChange (ie. This brings us to the index
		//that was set in DataState::setCurrentIndex().
		transmitChangeSig_ = last;
		propLookup_.value(nextVal->propId_)->setCurrentIndex(nextVal->index_);
		transmitChangeSig_ = true;
	}
	else
	{	//Tell the PropertyState for the property that just changed to update
		//its value.
		QSharedPointer<PropertyCollectionData> currVal = getCurrentValue().staticCast<PropertyCollectionData>();
		Q_ASSERT(currVal);
		Q_ASSERT(propLookup_.contains(currVal->propId_));
		propLookup_.value(currVal->propId_)->setCurrentIndex(currVal->index_);
	}
}

void PropertyCollectionState::requestMoreData(qulonglong index)
{}
void PropertyCollectionState::requestMoreDataByTime(double time)
{}

void PropertyCollectionState::subPropertyChanged(int propId, QString value)
{
	if(transmitChangeSig_)
		emit propertyChanged(propId,value);
}