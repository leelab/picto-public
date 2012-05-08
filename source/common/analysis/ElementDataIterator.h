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

protected:
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);
};

}; //namespace Picto
#endif