#include "TransitionState.h"
#include "PlaybackData.h"
using namespace Picto;

bool TransitionState::setTransition(double time,qulonglong dataId,int transId)
{
	setValue(QSharedPointer<IndexedData>(new PlaybackData<PlaybackTransData>(PlaybackTransData(transId),time,dataId)));
	return true;
}

void TransitionState::triggerValueChange(bool reverse,bool last)
{
	if(!reverse || last)
	{
		QSharedPointer<PlaybackData<PlaybackTransData>> currVal = getCurrentValue().staticCast<PlaybackData<PlaybackTransData>>();
		if(currVal)
			emit transitionActivated(currVal->data_.transId_);
		else	//If this is the case, we've moved back before the first value
			emit transitionActivated(0);
	}
}

void TransitionState::requestMoreData(PlaybackIndex currLast,PlaybackIndex to)
{
	emit needsData(currLast,to);
}
void TransitionState::requestNextData(PlaybackIndex currLast,bool backward)
{
	emit needsNextData(currLast,backward);
}