#include <QSqlQuery>
#include <QVariant>
#include <QStringList>
#include "TransitionDataIterator.h"
using namespace Picto;

TransitionDataIterator::TransitionDataIterator(QSqlDatabase session)
{
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	valid_ = true;
	totalValues_ = 0;
}

TransitionDataIterator::~TransitionDataIterator()
{

}

void TransitionDataIterator::registerTransitions(QString parentPath,QString sourceName,QString resultName, QString destinationName)
{
	int parentId = getElementId(parentPath);
	Q_ASSERT_X(parentId != 0,"TransitionDataIterator::registerTransitions","Parent element at path: " + parentPath.toAscii() + " was not found.");
	if(!parentId)
	{
		valid_ = false;
		return;
	}

	QStringList transIds = transIdString.split(",",QString::SkipEmptyParts);
	QSqlQuery query(session_);
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
	transIdString = transIds.join(",");
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

EventOrderIndex TransitionDataIterator::getNextTransitionTraversal()
{
	if(traversals_.size())
		return traversals_.takeFirst();
	//Maybe the session wasn't over last time.  Try getting a new traversal.
	updateTraversalList();
	if(traversals_.size())
		return traversals_.takeFirst();
	//No new data, return an empty EventOrderIndex()
	return EventOrderIndex();
}

void TransitionDataIterator::updateTraversalList()
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

	//Get traversal list.
	query.prepare(QString("SELECT t.dataid, f.time FROM transitions t, frames f WHERE t.transid IN (%1) AND t.frameid=f.dataid AND t.dataid > :lastdataid ORDER BY t.dataid").arg(transIdString));
	query.bindValue(":lastdataid",lastSessionDataId_);
	query.bindValue(":lastdataid",lastSessionDataId_);
	success = query.exec();
	if(!success)
	{
		return;
	}
	qulonglong lastDataId = 0;
	while(query.next()){
		traversals_.append(EventOrderIndex(query.value(0).toLongLong(),query.value(1).toDouble()));
		lastDataId = query.value(0).toLongLong();
	}
	totalValues_ = traversals_.size();
	//Update lastSessionDataId_ to either the last frame in the session or the last property dataid.  
	//Whichever is higher.
	if(lastDataId > lastFrameDataId)
		lastSessionDataId_ = lastDataId;
	else
		lastSessionDataId_ = lastFrameDataId;
}

int TransitionDataIterator::getElementId(QString path)
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