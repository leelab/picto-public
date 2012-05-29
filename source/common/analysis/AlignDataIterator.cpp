#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "AlignDataIterator.h"
using namespace Picto;

AlignDataIterator::AlignDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session)
: AnalysisDataIterator(qsEngine,session)
{
}

AlignDataIterator::~AlignDataIterator()
{

}

bool AlignDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows)
{
	query->prepare("SELECT b.dataid, b.timestamp, b.aligncode, b.aligneventnumber FROM behavioralalignevents b WHERE b.dataid > :lastdataid AND b.timestamp <= :stoptime ORDER BY b.dataid LIMIT :maxrows");
	query->bindValue(":lastdataid",lastDataId);
	query->bindValue(":stoptime",stopTime);
	query->bindValue(":maxrows",maxRows);
	return true;
}

bool AlignDataIterator::prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime)
{
	query->prepare("SELECT b.dataid, b.timestamp, b.aligncode FROM behavioralalignevents b WHERE b.timestamp <= :beforeTime ORDER BY b.dataid DESC LIMIT 1");
	query->bindValue(":beforetime",beforeTime);
	return true;
}

void AlignDataIterator::prepareSqlQueryForTotalRowCount(QSqlQuery* query)
{
	query->prepare("SELECT COUNT(dataid) FROM behavioralalignevents");
}

qulonglong AlignDataIterator::readOutRecordData(QSqlRecord* record)
{
	QSharedPointer<AnalysisValue> val = createNextAnalysisValue(EventOrderIndex(record->value(1).toDouble(),record->value(0).toLongLong(),EventOrderIndex::BEHAVIORAL));
	val->scriptVal.setProperty("code",record->value(2).toInt());
	val->scriptVal.setProperty("number",record->value(3).toInt());
	return val->index.dataId_;
}