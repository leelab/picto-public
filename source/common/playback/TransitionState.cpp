#include "TransitionState.h"
#include "PlaybackData.h"
using namespace Picto;

bool TransitionState::setTransition(double time,qulonglong dataId,int transId)
{
	setValue(QSharedPointer<IndexedData<qulonglong>>(new PlaybackData<qulonglong,PlaybackTransData>(time,dataId,PlaybackTransData(transId))));
	return true;
}

void TransitionState::triggerValueChange(bool reverse,bool last)
{
	if(!reverse || last)
	{
		PlaybackTransData* data = &getCurrentValue().staticCast<PlaybackData<qulonglong,PlaybackTransData>>()->data_;
		emit transitionActivated(data->transId_);
	}
}

void TransitionState::requestMoreData(qulonglong index)
{
}

void TransitionState::requestMoreDataByTime(double time)
{
}
