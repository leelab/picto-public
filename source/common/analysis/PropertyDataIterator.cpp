#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "PropertyDataIterator.h"
using namespace Picto;

PropertyDataIterator::PropertyDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session,QString propertyPath)
: AnalysisDataIterator(qsEngine,session)
{
	propertyId_ = getPropertyId(propertyPath);
}

PropertyDataIterator::~PropertyDataIterator()
{

}

QString PropertyDataIterator::propertyDescriptor()
{
	return "PropId:"+QString::number(propertyId_);
}

bool PropertyDataIterator::prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows)
{
	if(propertyId_ <= 0)
	{
		return false;
	}
	query->prepare("SELECT p.value, p.dataid, f.time FROM properties p, frames f "
		"WHERE p.assetid=:assetid AND f.dataid=p.frameid AND p.dataid > :lastdataid "
		"AND f.time <= :stoptime ORDER BY p.dataid LIMIT :maxrows");
	query->bindValue(":assetid",propertyId_);
	query->bindValue(":lastdataid",lastDataId);
	query->bindValue(":stoptime",stopTime);
	query->bindValue(":maxrows",maxRows);
	return true;
}

bool PropertyDataIterator::prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime)
{
	if(propertyId_ <= 0)
		return false;
	query->prepare("SELECT p.value, p.dataid, f.time FROM properties p, frames f "
		"WHERE p.assetid=:assetid AND f.dataid=p.frameid AND f.time < :beforetime "
		"ORDER BY p.dataid DESC LIMIT 1");
	query->bindValue(":assetid",propertyId_);
	query->bindValue(":beforetime",beforeTime);
	return true;
}

void PropertyDataIterator::prepareSqlQueryForTotalRowCount(QSqlQuery* query)
{
	query->prepare("SELECT COUNT(dataid) FROM properties WHERE assetid=:assetid");
	query->bindValue(":assetid",propertyId_);
}

qulonglong PropertyDataIterator::readOutRecordData(QSqlRecord* record)
{
	QSharedPointer<AnalysisValue> val = createNextAnalysisValue(EventOrderIndex(record->value(2).toDouble(),record->value(1).toLongLong(),EventOrderIndex::BEHAVIORAL));
	val->scriptVal.setProperty("time",record->value(2).toDouble());
	val->scriptVal.setProperty("value",record->value(0).toString());
	return val->index.dataId_;
}

int PropertyDataIterator::getElementId(QString path)
{
	QSqlQuery query = getSessionQuery();
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

int PropertyDataIterator::getPropertyId(QString fullPath)
{
	if(fullPath.isEmpty())
	{
		Q_ASSERT_X(false,"PropertyDataIterator::getPropertyId","No path was entered for the requested property");
		return 0;
	}
	QString parentPath = getParentElementPath(fullPath);
	if(parentPath.isEmpty())
	{
		Q_ASSERT_X(false,"PropertyDataIterator::getPropertyId","Property path did not include a parent");
		return 0;
	}
	int parentId = getElementId(parentPath);
	if(parentId == 0)
	{
		Q_ASSERT_X(false,"PropertyDataIterator::getPropertyId","Property parent at path " + parentPath.toLatin1() + " could not be found");
		return 0;
	}
	QString propName = getPropertyName(fullPath);
	if(propName.isEmpty())
	{
		Q_ASSERT_X(false,"PropertyDataIterator::getPropertyId","A property with parent path "+parentPath.toLatin1()+" and no name was requested.");
		return 0;
	}

	QSqlQuery query = getSessionQuery();
	query.setForwardOnly(true);
	query.prepare("SELECT assetid FROM propertylookup WHERE parent=:parentId AND name=:name");
	query.bindValue(":parentId",parentId);
	query.bindValue(":name",propName);
	bool success = query.exec();

	if(!success || !query.next())
	{
		Q_ASSERT_X(false,"PropertyDataIterator::getPropertyId","Failed to find property at path "+fullPath.toLatin1()+".");
		return 0;
	}
	else
	{
		return query.value(0).toInt();		
	}
}

QString PropertyDataIterator::getParentElementPath(QString fullPath)
{
	QStringList splitPath = fullPath.split("::");
	splitPath.takeLast();
	return splitPath.join("::");
}

QString PropertyDataIterator::getPropertyName(QString fullPath)
{
	QStringList splitPath = fullPath.split("::");
	return splitPath.takeLast();
}