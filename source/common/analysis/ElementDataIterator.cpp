#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "ElementDataIterator.h"
using namespace Picto;

ElementDataIterator::ElementDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session)
: AnalysisDataIterator(qsEngine,session)
{
}

ElementDataIterator::~ElementDataIterator()
{

}

bool ElementDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId)
{
	query->prepare(QString("SELECT t.dataid, f.time, tl.destination, e.path FROM transitions t, frames f, "
		"transitionlookup tl, elementlookup e WHERE t.transid=tl.assetid AND t.frameid=f.dataid AND "
		"e.assetid=tl.parent AND t.dataid > :lastdataid ORDER BY t.dataid"));
	query->bindValue(":lastdataid",lastDataId);
	return true;
}

void ElementDataIterator::prepareSqlQueryForTotalRowCount(QSqlQuery* query)
{
	query->prepare("SELECT COUNT(dataid) FROM transitions");
}

qulonglong ElementDataIterator::readOutRecordData(QSqlRecord* record)
{
	QSharedPointer<AnalysisValue> val = createNextAnalysisValue(EventOrderIndex(record->value(1).toDouble(),record->value(0).toLongLong(),EventOrderIndex::BEHAVIORAL));
	val->scriptVal.setProperty("time",record->value(1).toDouble());
	val->scriptVal.setProperty("name",record->value(3).toString()+"::"+record->value(2).toString());
	return record->value(0).toLongLong();
}