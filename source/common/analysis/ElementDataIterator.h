#ifndef _ELEMENT_DATA_ITERATOR_H_
#define _ELEMENT_DATA_ITERATOR_H_
#include "AnalysisDataIterator.h"

namespace Picto {
class ElementDataIterator : public AnalysisDataIterator
{
	Q_OBJECT
public:
	ElementDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~ElementDataIterator();
	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::BEHAVIORAL;};

protected:
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows);
	virtual bool prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);
};

}; //namespace Picto
#endif