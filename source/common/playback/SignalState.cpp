#include "SignalState.h"
#include "PlaybackData.h"
using namespace Picto;

SignalState::SignalState(QString name,QStringList subChanNames) :
name_(name),
subChanNames_(subChanNames),
numSubChans_(subChanNames_.size())
{
	Q_ASSERT(numSubChans_);
}

bool SignalState::setSignal(double time,qulonglong dataId,double sampPeriod,QByteArray dataArray)
{
	int numEntries = dataArray.size()/sizeof(float);
	float* floatArray = reinterpret_cast<float*>(dataArray.data());
	QVector<float> vals(numSubChans_);
	int subChanInd;
	double currTime;
	for(int i=0;i<numEntries;i++)
	{
		subChanInd = i%numSubChans_;
		vals[subChanInd] = floatArray[i];
		if(subChanInd == numSubChans_-1)
		{
			currTime = time + sampPeriod*i/numSubChans_;
			setValue(QSharedPointer<IndexedData>(new PlaybackData<PlaybackSignalData>(PlaybackSignalData(vals),currTime)));
		}
	}
	return true;
}

void SignalState::triggerValueChange(bool reverse,bool last)
{
	if(!reverse || last)
	{
		QSharedPointer<PlaybackData<PlaybackSignalData>> currVal = getCurrentValue().staticCast<PlaybackData<PlaybackSignalData>>();
		if(currVal)
			emit signalChanged(name_,subChanNames_,currVal->data_.vals_);
		else	//If this is the case, we've moved back before the first value
			emit signalChanged(name_,subChanNames_,QVector<float>());
	}
}

void SignalState::requestMoreData(PlaybackIndex currLast,PlaybackIndex to)
{
	emit needsData(currLast,to);
}
void SignalState::requestNextData(PlaybackIndex currLast,bool backward)
{
	emit needsNextData(currLast,backward);
}