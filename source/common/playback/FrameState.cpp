#include "FrameState.h"
using namespace Picto;

bool FrameState::setFrame(qulonglong dataId,double frameTime)
{
	setValue(QSharedPointer<IndexedData<qulonglong>>(new IndexedData<qulonglong>(frameTime,dataId)));
	return true;
}

void FrameState::triggerValueChange(bool reverse,bool last)
{
	if(!reverse || last)
		emit framePresented(getCurrentValue()->time_);
}
void FrameState::requestMoreData(qulonglong index)
{

}
void FrameState::requestMoreDataByTime(double time)
{

}