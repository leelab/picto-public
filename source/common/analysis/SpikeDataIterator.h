#ifndef _SPIKE_DATA_ITERATOR_H_
#define _SPIKE_DATA_ITERATOR_H_
#include <QScriptEngine>
#include <QString>
#include <QLinkedList>
#include <QSqlDatabase>
#include "EventOrderIndex.h"
#include "AnalysisValue.h"

namespace Picto {
struct SpikeData;
class SpikeDataIterator
{
public:
	SpikeDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~SpikeDataIterator();
	bool isValid(){return true;};

	//Gets the next property change following the last one returned.  
	//If there are no more property changes available it returns a PropData struct
	//with negative time.
	QSharedPointer<SpikeData> getNextSpikeVals();
	unsigned int totalValues(){return totalQueries_;};
	unsigned int remainingValues(){return totalValues() - readQueries_+spikeVals_.size();};
	void recheckSessionData(){updateTotalQueryCount();};

private:
	void updateSpikeValsList();
	void getSamplePeriod();
	void getAlignCoefficients();
	void updateTotalQueryCount();

	qulonglong lastSessionDataId_;
	
	QLinkedList<QSharedPointer<SpikeData>> spikeVals_;
	QSqlDatabase session_;
	QString tableName_;
	bool sessionEnded_;
	double samplePeriod_;
	double offsetTime_;
	double temporalFactor_;
	unsigned int totalQueries_;
	unsigned int readQueries_;
	QSharedPointer<QScriptEngine> qsEngine_;

};

struct SpikeData : public AnalysisValue{
	//SpikeData(){index.idSource_ = EventOrderIndex::NEURAL;}
	SpikeData(QSharedPointer<QScriptEngine> qsEngine):AnalysisValue(qsEngine){channel=-1;unit=-1;samplePeriod=-1;waveSize=-1;wave="";}
	SpikeData(QSharedPointer<QScriptEngine> qsEngine,qulonglong dataId, 
				double time, 
				unsigned int chan, 
				unsigned int un,
				double sampPer,
				unsigned int wSize,
				QString waveDat):AnalysisValue(qsEngine,EventOrderIndex(time,dataId,EventOrderIndex::NEURAL)){/*index.time_ = time;index.dataId_ = dataId;index.idSource_ = EventOrderIndex::NEURAL;*/channel=chan;unit=un;samplePeriod=sampPer;waveSize=wSize;wave=waveDat;scriptVal.setProperty("channel",chan);scriptVal.setProperty("unit",un);scriptVal.setProperty("samplePeriod",sampPer);scriptVal.setProperty("waveSize",wSize);scriptVal.setProperty("wave",waveDat);QScriptValue wArray = qsEngine->newArray(wSize);scriptVal.setProperty("waveValue",wArray);QStringList wVals = waveDat.split(",",QString::SkipEmptyParts);for(int i=0;i<wSize;i++){wArray.setProperty(i,wVals[i]);}};
	QString scaleWave(double scaleFactor = 1.0, unsigned int decimalPlaces = 6);
	unsigned int channel;
	unsigned int unit;
	double samplePeriod;
	unsigned int waveSize;
	QString wave;
};

}; //namespace Picto
#endif