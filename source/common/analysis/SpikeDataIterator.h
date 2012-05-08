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

protected:
	virtual void updateVariableSessionConstants();
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId);
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