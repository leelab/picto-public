#ifndef _SPIKE_DATA_ITERATOR_H_
#define _SPIKE_DATA_ITERATOR_H_
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
	SpikeDataIterator(QSqlDatabase session);
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

};

struct SpikeData : public AnalysisValue{
	SpikeData(){index.idSource_ = EventOrderIndex::NEURAL;}
	SpikeData(qulonglong dataId, 
				double time, 
				unsigned int chan, 
				unsigned int un,
				double sampPer,
				unsigned int wSize,
				QString waveDat){index.time_ = time;index.dataId_ = dataId;index.idSource_ = EventOrderIndex::NEURAL;channel=chan;unit=un;samplePeriod=sampPer;waveSize=wSize;wave=waveDat;};
	unsigned int channel;
	unsigned int unit;
	double samplePeriod;
	unsigned int waveSize;
	QString wave;
};

}; //namespace Picto
#endif