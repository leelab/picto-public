#ifndef _PROPERTY_DATA_ITERATOR_H_
#define _PROPERTY_DATA_ITERATOR_H_
#include "AnalysisDataIterator.h"

namespace Picto {
class PropertyDataIterator : public AnalysisDataIterator
{
public:
	PropertyDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session,QString propertyPath);
	virtual ~PropertyDataIterator();
	//bool isValid(){return propertyId_ > 0;};

protected:
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId);
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