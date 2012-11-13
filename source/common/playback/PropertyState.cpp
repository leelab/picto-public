#include "PropertyState.h"
#include "PlaybackData.h"
using namespace Picto;

PropertyState::PropertyState(int propId) :
propId_(propId)
{
}
bool PropertyState::setPropertyValue(double time,qulonglong dataId,QString value)
{
	setValue(QSharedPointer<IndexedData>(new PlaybackData<QString>(value,time,dataId)));
	return true;
}

void PropertyState::triggerValueChange(bool reverse,bool last)
{
	QSharedPointer<PlaybackData<QString>> currVal = getCurrentValue().staticCast<PlaybackData<QString>>();
	if(currVal)
		emit propertyChanged(propId_,currVal->data_);
	else //If this is the case, we've moved back before the first value
		emit propertyChanged(propId_,"");
}

void PropertyState::requestMoreData(PlaybackIndex currLast,PlaybackIndex to)
{
}
void PropertyState::requestNextData(PlaybackIndex currLast,bool backward)
{
}