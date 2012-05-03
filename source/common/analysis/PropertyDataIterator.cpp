#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "PropertyDataIterator.h"
using namespace Picto;

PropertyDataIterator::PropertyDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session,QString propertyPath)
{
	qsEngine_ = qsEngine;
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	propertyId_ = getPropertyId(propertyPath);
	totalQueries_ = 0;
	readQueries_ = 0;

	updateTotalQueryCount();
}

PropertyDataIterator::~PropertyDataIterator()
{

}

QSharedPointer<PropData> PropertyDataIterator::getNextPropertyChange()
{
	if(propVals_.size())
		return propVals_.takeFirst();
	//Maybe the session wasn't over last time.  Try getting new data.
	updatePropValsList();
	if(propVals_.size())
		return propVals_.takeFirst();
	//No new data, return an empty propData()
	return QSharedPointer<PropData>(new PropData(qsEngine_));
}

void PropertyDataIterator::updatePropValsList()
{
	if(!isValid())
		return;
	if(readQueries_ >= totalQueries_)
		return;
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	//Get property value list.
	query.prepare("SELECT p.value, p.dataid, f.time FROM properties p, frames f "
		"WHERE p.assetid=:assetid AND f.dataid=p.frameid AND p.dataid > :lastdataid "
		"ORDER BY p.dataid LIMIT 10000");
	query.bindValue(":assetid",propertyId_);
	query.bindValue(":lastdataid",lastSessionDataId_);
	bool success = query.exec();
	if(!success)
	{
		return;
	}
	qulonglong lastDataId = lastSessionDataId_;
	while(query.next()){
		propVals_.append(QSharedPointer<PropData>(new PropData(qsEngine_,query.value(0).toString(),query.value(1).toLongLong(),query.value(2).toDouble())));
		lastDataId = query.value(1).toLongLong();
		readQueries_++;
	}
	if(readQueries_ > totalQueries_)
		updateTotalQueryCount();

	lastSessionDataId_ = lastDataId;
}

void PropertyDataIterator::updateTotalQueryCount()
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	QString queryString = QString("SELECT COUNT(dataid) FROM properties WHERE assetid=:assetid");
	query.prepare(queryString);
	query.bindValue(":assetid",propertyId_);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}

	if(query.next())
		totalQueries_ = query.value(0).toInt();
}

int PropertyDataIterator::getElementId(QString path)
{
	if(!session_.isOpen())
		return 0;
	QSqlQuery query(session_);
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
		return 0;
	QString parentPath = getParentElementPath(fullPath);
	if(parentPath.isEmpty())
		return 0;
	int parentId = getElementId(parentPath);
	if(parentId == 0)
		return 0;
	QString propName = getPropertyName(fullPath);
	if(propName.isEmpty())
		return 0;

	QSqlQuery query(session_);
	query.setForwardOnly(true);
	query.prepare("SELECT assetid FROM propertylookup WHERE parent=:parentId AND name=:name");
	query.bindValue(":parentId",parentId);
	query.bindValue(":name",propName);
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