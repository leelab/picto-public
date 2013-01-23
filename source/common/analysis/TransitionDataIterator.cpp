#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "TransitionDataIterator.h"
using namespace Picto;

TransitionDataIterator::TransitionDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session)
: AnalysisDataIterator(qsEngine,session)
{
	valid_ = true;
}

TransitionDataIterator::~TransitionDataIterator()
{

}

void TransitionDataIterator::registerTransitions(QString parentPath,QString sourceName,QString resultName, QString destinationName)
{
	int parentId = getElementId(parentPath);
	Q_ASSERT_X(parentId != 0,"TransitionDataIterator::registerTransitions","Parent element at path: " + parentPath.toLatin1() + " was not found.");
	if(!parentId)
	{
		valid_ = false;
		return;
	}

	QStringList transIds = transIdString_.split(",",QString::SkipEmptyParts);
	QSqlQuery query(getSessionQuery());
	QString queryString("SELECT assetid FROM transitionlookup WHERE parent=:parentId");
	if(!sourceName.isEmpty())
		queryString.append(" AND source=:source");
	if(!resultName.isEmpty())
		queryString.append(" AND sourceresult=:result");
	if(!destinationName.isEmpty())
		queryString.append(" AND destination=:destination");
	query.prepare(queryString);
	query.bindValue(":parentId",parentId);
	if(!sourceName.isEmpty())
		query.bindValue(":source",sourceName);
	if(!resultName.isEmpty())
		query.bindValue(":sourceresult",resultName);
	if(!destinationName.isEmpty())
		query.bindValue(":destination",destinationName);
	bool success = query.exec();
	Q_ASSERT_X(success,"TransitionDataIterator::registerTransitions","Transition Query Failed.");
	if(!success)
	{
		valid_ = false;
		return;
	}
	while(query.next())
	{
		transIds.append(query.value(0).toString());		
	}
	transIdString_ = transIds.join(",");
}

void TransitionDataIterator::registerTransitionsByNode(QString nodePath,bool isSource)
{
	QStringList splitPath = nodePath.split("::",QString::SkipEmptyParts);
	QString nodeName = splitPath.takeLast();
	QString parentPath = splitPath.join("::");
	if(isSource)
		registerTransitions(parentPath,nodeName);
	else
		registerTransitions(parentPath,"","",nodeName);
}

void TransitionDataIterator::registerTransitionsByResult(QString resultPath)
{
	QStringList splitPath = resultPath.split("::",QString::SkipEmptyParts);
	QString resultName = splitPath.takeLast();
	QString sourceName = splitPath.takeLast();
	QString parentPath = splitPath.join("::");
	registerTransitions(parentPath,sourceName,resultName);
}

QString TransitionDataIterator::propertyDescriptor()
{
	return "TransIds:"+transIdString_;
}

bool TransitionDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows)
{
	Q_ASSERT(isValid());
	query->prepare(QString("SELECT t.dataid, f.time FROM transitions t, frames f WHERE t.transid IN (%1) AND t.frameid=f.dataid AND t.dataid > :lastdataid AND f.time <= :stoptime ORDER BY t.dataid LIMIT :maxrows").arg(transIdString_));
	query->bindValue(":lastdataid",lastDataId);
	query->bindValue(":stoptime",stopTime);
	query->bindValue(":maxrows",maxRows);
	return true;
}

bool TransitionDataIterator::prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime)
{
	Q_ASSERT(isValid());
	query->prepare(QString("SELECT t.dataid, f.time FROM transitions t, frames f WHERE t.transid IN (%1) AND t.frameid=f.dataid AND f.time < :beforetime ORDER BY t.dataid DESC LIMIT 1").arg(transIdString_));
	query->bindValue(":beforetime",beforeTime);
	return true;
}

void TransitionDataIterator::prepareSqlQueryForTotalRowCount(QSqlQuery* query)
{
	QString queryString = QString("SELECT COUNT(dataid) FROM transitions WHERE transid IN (%1)").arg(transIdString_);
	query->prepare(queryString);
}

qulonglong TransitionDataIterator::readOutRecordData(QSqlRecord* record)
{
	QSharedPointer<AnalysisValue> val = createNextAnalysisValue(EventOrderIndex(record->value(1).toDouble(),record->value(0).toLongLong(),EventOrderIndex::BEHAVIORAL));
	return val->index.dataId_;
}

int TransitionDataIterator::getElementId(QString path)
{
	QSqlQuery query(getSessionQuery());
	query.setForwardOnly(true);
	query.prepare("SELECT assetid FROM elementlookup WHERE path=:path");
	query.bindValue(":path",path);
	bool success = query.exec();

	if(!success || !query.next())
	{
		return 0;
	}
	else
	{
		return query.value(0).toInt();		
	}
}