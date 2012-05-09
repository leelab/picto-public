#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "FrameDataIterator.h"
using namespace Picto;

FrameDataIterator::FrameDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session)
: AnalysisDataIterator(qsEngine,session)
{
}

FrameDataIterator::~FrameDataIterator()
{

}

bool FrameDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows)
{
	query->prepare("SELECT f.dataid, f.time FROM frames f WHERE f.dataid > :lastdataid AND f.time <= :stoptime ORDER BY f.dataid LIMIT :maxrows");
	query->bindValue(":lastdataid",lastDataId);
	query->bindValue(":stoptime",stopTime);
	query->bindValue(":maxrows",maxRows);
	return true;
}

bool FrameDataIterator::prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime)
{
	query->prepare("SELECT f.dataid, f.time FROM frames f WHERE f.time < :beforetime ORDER BY f.dataid DESC LIMIT 1");
	query->bindValue(":beforetime",beforeTime);
	return true;
}

void FrameDataIterator::prepareSqlQueryForTotalRowCount(QSqlQuery* query)
{
	query->prepare("SELECT COUNT(dataid) FROM frames");
}

qulonglong FrameDataIterator::readOutRecordData(QSqlRecord* record)
{
	QSharedPointer<AnalysisValue> val = createNextAnalysisValue(EventOrderIndex(record->value(1).toDouble(),record->value(0).toLongLong(),EventOrderIndex::BEHAVIORAL));
	val->scriptVal.setProperty("time",record->value(1).toDouble());
	return val->index.dataId_;
}