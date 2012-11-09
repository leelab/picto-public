#include "PropertyState.h"
#include "PlaybackData.h"
using namespace Picto;

PropertyState::PropertyState(int propId) :
propId_(propId)
{
}
bool PropertyState::setPropertyValue(double time,qulonglong dataId,QString value)
{
	setValue(QSharedPointer<IndexedData<qulonglong>>(new PlaybackData<qulonglong,QString>(time,dataId,value)));
	return true;
}

void PropertyState::triggerValueChange(bool reverse,bool last)
{
	emit propertyChanged(propId_,getCurrentValue().staticCast<PlaybackData<qulonglong,QString>>()->data_);
}

void PropertyState::requestMoreData(qulonglong index)
{
}

void PropertyState::requestMoreDataByTime(double time)
{
}
