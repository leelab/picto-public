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

bool ElementDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows)
{
	query->prepare(QString("SELECT t.dataid, f.time, tl.destination, e.path FROM transitions t, frames f, "
		"transitionlookup tl, elementlookup e WHERE t.transid=tl.assetid AND t.frameid=f.dataid AND "
		"e.assetid=tl.parent AND t.dataid > :lastdataid AND f.time <= :stoptime ORDER BY t.dataid LIMIT :maxrows"));
	query->bindValue(":lastdataid",lastDataId);
	query->bindValue(":stoptime",stopTime);
	query->bindValue(":maxrows",maxRows);
	return true;
}

bool ElementDataIterator::prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime)
{
	query->prepare(QString("SELECT t.dataid, f.time, tl.destination, e.path FROM transitions t, frames f, "
		"transitionlookup tl, elementlookup e WHERE t.transid=tl.assetid AND t.frameid=f.dataid AND "
		"e.assetid=tl.parent AND f.time < :beforetime ORDER BY t.dataid DESC LIMIT 1"));
	query->bindValue(":beforetime",beforeTime);
	return true;
}

void ElementDataIterator::prepareSqlQueryForTotalRowCount(QSqlQuery* query)
{
	query->prepare("SELECT COUNT(t.dataid) FROM transitions t");
}

qulonglong ElementDataIterator::readOutRecordData(QSqlRecord* record)
{
	QSharedPointer<AnalysisValue> val = createNextAnalysisValue(EventOrderIndex(record->value(1).toDouble(),record->value(0).toLongLong(),EventOrderIndex::BEHAVIORAL));
	val->scriptVal.setProperty("time",record->value(1).toDouble());
	val->scriptVal.setProperty("name",record->value(3).toString()+"::"+record->value(2).toString());
	return val->index.dataId_;
}