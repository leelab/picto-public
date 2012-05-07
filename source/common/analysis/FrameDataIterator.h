#ifndef _FRAME_DATA_ITERATOR_H_
#define _FRAME_DATA_ITERATOR_H_
#include "AnalysisDataIterator.h"

namespace Picto {
class FrameDataIterator : public AnalysisDataIterator
{
public:
	FrameDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~FrameDataIterator();


protected:
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);
};

}; //namespace Picto
#endif