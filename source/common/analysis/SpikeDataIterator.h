#ifndef _SPIKE_DATA_ITERATOR_H_
#define _SPIKE_DATA_ITERATOR_H_
#include "AnalysisDataIterator.h"

namespace Picto {
	class SpikeDataIterator : public AnalysisDataIterator
{
	Q_OBJECT
public:
	SpikeDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~SpikeDataIterator();
	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::NEURAL;};

protected:
	virtual void updateVariableSessionConstants();
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows);
	virtual bool prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);

private:
	void getSamplePeriod();

	double samplePeriod_;
	double offsetTime_;
	double temporalFactor_;
};

}; //namespace Picto
#endif