#ifndef _TIME_DATA_ITERATOR_H_
#define _TIME_DATA_ITERATOR_H_
#include "AnalysisDataIterator.h"

namespace Picto {
class TimeDataIterator : public AnalysisDataIterator
{
	Q_OBJECT
public:
	TimeDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session,double period);
	virtual ~TimeDataIterator();
	virtual QString propertyDescriptor();
	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::OTHER;};

protected:
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows);
	virtual bool prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);

private:
	double period_;
	double nextTime_;
};

}; //namespace Picto
#endif