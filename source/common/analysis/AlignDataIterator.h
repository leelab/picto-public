#ifndef _ALIGN_DATA_ITERATOR_H_
#define _ALIGN_DATA_ITERATOR_H_
#include "AnalysisDataIterator.h"

namespace Picto {
class AlignDataIterator : public AnalysisDataIterator
{
	Q_OBJECT
public:
	AlignDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~AlignDataIterator();
	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::BEHAVIORAL;};

protected:
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows);
	virtual bool prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);
};

}; //namespace Picto
#endif