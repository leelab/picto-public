#ifndef _FRAME_DATA_ITERATOR_H_
#define _FRAME_DATA_ITERATOR_H_
#include "AnalysisDataIterator.h"

namespace Picto {
class FrameDataIterator : public AnalysisDataIterator
{
	Q_OBJECT
public:
	FrameDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~FrameDataIterator();
	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::BEHAVIORAL;};

protected:
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows);
	virtual bool prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);
};

}; //namespace Picto
#endif