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

bool FrameDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId)
{
	query->prepare("SELECT f.dataid, f.time FROM frames f WHERE f.dataid > :lastdataid ORDER BY f.dataid LIMIT 10000");
	query->bindValue(":lastdataid",lastDataId);
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
	return record->value(0).toLongLong();
}