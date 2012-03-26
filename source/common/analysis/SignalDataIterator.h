#ifndef _SIGNAL_DATA_ITERATOR_H_
#define _SIGNAL_DATA_ITERATOR_H_
#include <QString>
#include <QLinkedList>
#include <QSqlDatabase>
#include "EventOrderIndex.h"
#include "AnalysisValue.h"

namespace Picto {
struct SignalData;
class SignalDataIterator
{
public:
	SignalDataIterator(QSqlDatabase session,QString signalName);
	virtual ~SignalDataIterator();
	bool isValid(){return numSubChans_ > 0;};

	//Gets the next property change following the last one returned.  
	//If there are no more property changes available it returns a PropData struct
	//with negative time.
	QSharedPointer<SignalData> getNextSignalVals();
	unsigned int totalValues(){return readQueries_?totalQueries_ * (readValues_/readQueries_):totalQueries_;};
	unsigned int remainingValues(){return totalValues() - readValues_ + signalVals_.size();};
	unsigned int numSubChannels(){return numSubChans_;};
	QString subChanName(int subChanIndex){if(subChanNames_.size() < subChanIndex) return ""; return subChanNames_[subChanIndex];};
	void recheckSessionData(){updateTotalQueryCount();};

private:
	void updateSignalValsList();
	void updateTotalQueryCount();
	void getSubChanInfo(QString signalName);

	qulonglong lastSessionDataId_;
	
	QLinkedList<QSharedPointer<SignalData>> signalVals_;
	QSqlDatabase session_;
	QString tableName_;
	int numSubChans_;
	QStringList subChanNames_;
	double samplePeriod_;
	unsigned int totalQueries_;
	unsigned int readValues_;
	unsigned int readQueries_;

};

struct SignalData : public AnalysisValue{
	SignalData(){}
	SignalData(qulonglong dataIndex,double time,int vals){index.dataId_ = dataIndex; index.idSource_ = EventOrderIndex::BEHAVIORAL; index.time_ = time;values.resize(vals);};
	QVector<float> values;
};

}; //namespace Picto
#endif