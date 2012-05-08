#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "TimeDataIterator.h"
using namespace Picto;

TimeDataIterator::TimeDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session,double period)
: AnalysisDataIterator(qsEngine,session)
{
	nextTime_ = 0;
	period_ = period;
}

TimeDataIterator::~TimeDataIterator()
{

}

QString TimeDataIterator::propertyDescriptor()
{
	return "Period:"+QString::number(period_);
}

bool TimeDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId)
{
	query->prepare("SELECT f.dataid, f.time FROM frames f WHERE f.dataid > :lastdataid ORDER BY f.dataid LIMIT 1");
	query->bindValue(":lastdataid",lastDataId);
	return true;
}

void TimeDataIterator::prepareSqlQueryForTotalRowCount(QSqlQuery* query)
{
	query->prepare("SELECT 1");
}

qulonglong TimeDataIterator::readOutRecordData(QSqlRecord* record)
{
	double endTime = record->value(1).toDouble();
	while(nextTime_ <= endTime)
	{
		createNextAnalysisValue(EventOrderIndex(nextTime_));
		nextTime_ += period_;
	}
	return record->value(0).toLongLong();
}