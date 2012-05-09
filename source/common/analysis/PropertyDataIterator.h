#ifndef _PROPERTY_DATA_ITERATOR_H_
#define _PROPERTY_DATA_ITERATOR_H_
#include "AnalysisDataIterator.h"

namespace Picto {
class PropertyDataIterator : public AnalysisDataIterator
{
	Q_OBJECT
public:
	PropertyDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session,QString propertyPath);
	virtual ~PropertyDataIterator();
	virtual QString propertyDescriptor();
	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::BEHAVIORAL;};

protected:
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows);
	virtual bool prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);

private:
	int getElementId(QString path);
	int getPropertyId(QString fullPath);
	static QString getParentElementPath(QString fullPath);
	static QString getPropertyName(QString fullPath);
	int propertyId_;
};

}; //namespace Picto
#endif