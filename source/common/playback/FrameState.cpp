#include "FrameState.h"
#include "PlaybackData.h"
using namespace Picto;

bool FrameState::setFrame(qulonglong dataId,double frameTime)
{
	setValue(QSharedPointer<IndexedData>(new PlaybackData<bool>(false,frameTime,dataId)));
	return true;
}

void FrameState::triggerValueChange(bool reverse,bool last)
{
	if(!reverse || last)
	{
		QSharedPointer<IndexedData> currVal = getCurrentValue();
		if(currVal)
			emit framePresented(getCurrentValue()->index_.time());
		else //If this is the case, we've moved back before the first value
			emit framePresented(-1);
	}
}
void FrameState::requestMoreData(PlaybackIndex currLast,PlaybackIndex to)
{
	emit needsData(currLast,to);
}
void FrameState::requestNextData(PlaybackIndex currLast,bool backward)
{
	emit needsNextData(currLast,backward);
}