#include <QSqlQuery>
#include <QVariant>
#include <QStringList>
#include "PropertyDataIterator.h"
using namespace Picto;

PropertyDataIterator::PropertyDataIterator(QSqlDatabase session,QString propertyPath)
{
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	propertyId_ = getPropertyId(propertyPath);
	totalValues_ = 0;
}

PropertyDataIterator::~PropertyDataIterator()
{

}

PropData PropertyDataIterator::getNextPropertyChange()
{
	if(propVals_.size())
		return propVals_.takeFirst();
	//Maybe the session wasn't over last time.  Try getting new data.
	updatePropValsList();
	if(propVals_.size())
		return propVals_.takeFirst();
	//No new data, return an empty propData()
	return PropData();
}

void PropertyDataIterator::updatePropValsList()
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	
	//Get dataid of last frame in session
	query.prepare("SELECT dataid FROM frames ORDER BY time DESC LIMIT 1");
	bool success = query.exec();
	if(!success || !query.next())
	{
		lastSessionDataId_ = 0;
		return;
	}
	qulonglong lastFrameDataId = query.value(0).toLongLong();

	//Get property value list.
	query.prepare("SELECT p.value, p.dataid, f.time FROM properties p, frames f "
		"WHERE p.assetid=:assetid AND f.dataid=p.frameid AND p.dataid > :lastdataid "
		"ORDER BY p.dataid");
	query.bindValue(":assetid",propertyId_);
	query.bindValue(":lastdataid",lastSessionDataId_);
	success = query.exec();
	if(!success)
	{
		return;
	}
	qulonglong lastDataId = 0;
	while(query.next()){
		propVals_.append(PropData(query.value(0).toString(),query.value(1).toLongLong(),query.value(2).toDouble()));
		lastDataId = query.value(1).toLongLong();
	}
	totalValues_ = propVals_.size();

	//Update lastSessionDataId_ to either the last frame in the session or the last property dataid.  
	//Whichever is higher.
	if(lastDataId > lastFrameDataId)
		lastSessionDataId_ = lastDataId;
	else
		lastSessionDataId_ = lastFrameDataId;
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