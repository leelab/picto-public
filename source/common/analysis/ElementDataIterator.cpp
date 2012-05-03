#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "ElementDataIterator.h"
using namespace Picto;

ElementDataIterator::ElementDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session)
{
	qsEngine_ = qsEngine;
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	totalQueries_ = 0;
	readQueries_ = 0;
	updateTotalQueryCount();
}

ElementDataIterator::~ElementDataIterator()
{

}

QSharedPointer<ElementData> ElementDataIterator::getNextElement()
{
	if(elements_.size())
		return elements_.takeFirst();
	//Maybe the session wasn't over last time.  Try getting a new traversal.
	updateElementList();
	if(elements_.size())
		return elements_.takeFirst();
	//No new data, return an empty EventOrderIndex()
	return QSharedPointer<ElementData>();
}

void ElementDataIterator::updateElementList()
{
	if(readQueries_ >= totalQueries_)
		return;
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);

	//Get traversal list.
	query.prepare(QString("SELECT t.dataid, f.time, tl.destination, e.path FROM transitions t, frames f, "
		"transitionlookup tl, elementlookup e WHERE t.transid=tl.assetid AND t.frameid=f.dataid AND "
		"e.assetid=tl.parent AND t.dataid > :lastdataid ORDER BY t.dataid"));
	query.bindValue(":lastdataid",lastSessionDataId_);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}
	qulonglong lastDataId = lastSessionDataId_;
	while(query.next()){
		elements_.append(QSharedPointer<ElementData>(new ElementData(qsEngine_,query.value(0).toLongLong(),query.value(1).toDouble(),query.value(3).toString()+"::"+query.value(2).toString())));
		lastDataId = query.value(0).toLongLong();
		readQueries_++;
	}
	updateTotalQueryCount();
	
	lastSessionDataId_ = lastDataId;
}

void ElementDataIterator::updateTotalQueryCount()
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	QString queryString = QString("SELECT COUNT(dataid) FROM transitions");
	query.prepare(queryString);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}

	if(query.next())
		totalQueries_ = query.value(0).toInt();
}