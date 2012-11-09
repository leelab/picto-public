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
			setValue(QSharedPointer<IndexedData<double>>(new PlaybackData<double,PlaybackSignalData>(currTime,currTime,PlaybackSignalData(vals))));
		}
	}
	return true;
}

void SignalState::triggerValueChange(bool reverse,bool last)
{
	if(!reverse || last)
	{
		PlaybackSignalData* data = &getCurrentValue().staticCast<PlaybackData<double,PlaybackSignalData>>()->data_;
		emit signalChanged(name_,data->vals_);
	}
}

void SignalState::requestMoreData(double index)
{
}

void SignalState::requestMoreDataByTime(double time)
{
}
